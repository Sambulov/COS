#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  hdl_i2c_message_t msg;
  uint8_t addr[4];
  uint8_t *data;
  uint32_t data_size;
  uint8_t mem_addr_size;
  uint8_t dev_addr;
  uint8_t retry;
  hdl_i2c_mem_state_t task_state;
  uint8_t xfer_mode;
  uint8_t xfer_state;
} hdl_i2c_mem_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_i2c_mem_var_t, *((hdl_i2c_mem_t *)0)->obj_var, HDL_I2C_MEM_VAR_SIZE, hdl_i2c_mem.h);

#define XFER_MODE_READ           0
#define XFER_MODE_WRITE          1

#define XFER_STATE_IDLE          0
#define XFER_STATE_MSG_ADDR      1
#define XFER_STATE_TX_ADDR       2
#define XFER_STATE_TX_ADDR_AWAIT 3
#define XFER_STATE_MSG_DATA      4
#define XFER_STATE_TX_DATA       5
#define XFER_STATE_TX_DATA_AWAIT 6
#define XFER_STATE_COMPLETE      7
#define XFER_STATE_FAIL          8


static uint8_t _mem_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)arg;
  hdl_i2c_t *i2c = (hdl_i2c_t *)mem->dependencies[0];
  hdl_i2c_mem_var_t *mem_var = (hdl_i2c_mem_var_t *)mem->obj_var;

  switch (mem_var->xfer_state) {
    case XFER_STATE_MSG_ADDR:
      mem_var->msg.address = mem_var->dev_addr;
      mem_var->msg.buffer = mem_var->addr;
      mem_var->msg.length = mem_var->mem_addr_size;
      mem_var->msg.options = HDL_I2C_MESSAGE_WRITE_HALT;
      mem_var->xfer_state++;
      /* fall through */
    case XFER_STATE_TX_ADDR:
    case XFER_STATE_TX_DATA:
      if(!hdl_take(i2c, mem) || !hdl_i2c_transfer(i2c, &mem_var->msg)) break;
      mem_var->xfer_state++;
      /* fall through */
    case XFER_STATE_TX_DATA_AWAIT:
    case XFER_STATE_TX_ADDR_AWAIT:
      if(!(mem_var->msg.status & HDL_I2C_MESSAGE_STATUS_COMPLETE)) break;
      if(mem_var->msg.status & HDL_I2C_MESSAGE_FAULT_MASK) {
        if(mem_var->retry--) mem_var->xfer_state = XFER_STATE_MSG_ADDR;
        else mem_var->xfer_state = XFER_STATE_FAIL;
        hdl_give(i2c, mem);
        break;
      }
      mem_var->xfer_state++;
      break;
    case XFER_STATE_MSG_DATA:
      mem_var->msg.address = mem_var->dev_addr;
      mem_var->msg.buffer = mem_var->data;
      mem_var->msg.length = mem_var->data_size;
      if(mem_var->xfer_mode == XFER_MODE_WRITE) 
        mem_var->msg.options = HDL_I2C_MESSAGE_STOP;
      else 
        mem_var->msg.options = HDL_I2C_MESSAGE_READ_STOP;
      mem_var->xfer_state++;
      break;
    case XFER_STATE_FAIL:
      mem_var->task_state |= HDL_I2C_MEM_XFER_FAIL;
      /* fall through */
    case XFER_STATE_COMPLETE:
      mem_var->task_state &= ~HDL_I2C_MEM_BUSY;
      mem_var->xfer_state = XFER_STATE_IDLE;
      hdl_give(i2c, mem);
    case XFER_STATE_IDLE:
    default:
      break;
  }
  return cancel;
}

static hdl_i2c_mem_state_t _hdl_i2c_mem_get_state(const void *desc) {
  hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)desc;
  hdl_i2c_mem_var_t *mem_var = (hdl_i2c_mem_var_t *)mem->obj_var;
  return mem_var->task_state;
}

static uint8_t _hdl_i2c_mem_xfer(const void *desc, uint8_t dev_addr, uint8_t r_addr_sz, uint32_t addr, uint8_t *data, uint32_t size, uint8_t mode) {
  hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)desc;
  hdl_i2c_mem_var_t *mem_var = (hdl_i2c_mem_var_t *)mem->obj_var;
  if((mem_var->xfer_state != XFER_STATE_IDLE) || !r_addr_sz || (r_addr_sz > 4))
    return HDL_FALSE;
  for(uint8_t i = 0, ofst = (r_addr_sz - 1) * 8; i < r_addr_sz; i++, ofst-=8) {
    mem_var->addr[i] = addr >> ofst;
  }
  mem_var->data = data;
  mem_var->data_size = size;
  mem_var->xfer_mode = mode;
  mem_var->retry = mem->config->xfer_retry;
  mem_var->mem_addr_size = r_addr_sz;
  mem_var->dev_addr = dev_addr;
  mem_var->task_state |= HDL_I2C_MEM_BUSY;
  mem_var->task_state &= ~HDL_I2C_MEM_XFER_FAIL;
  mem_var->xfer_state = XFER_STATE_MSG_ADDR;
  return HDL_TRUE;
}

static uint8_t _hdl_i2c_mem_read(const void *desc, uint8_t dev_addr, uint8_t r_addr_sz, uint32_t r_addr, uint8_t *data, uint32_t size) {
  return _hdl_i2c_mem_xfer(desc, dev_addr, r_addr_sz, r_addr, data, size, XFER_MODE_READ);
}

static uint8_t _hdl_i2c_mem_write(const void *desc, uint8_t dev_addr, uint8_t r_addr_sz, uint32_t r_addr, const uint8_t *data, uint32_t size) {
  return _hdl_i2c_mem_xfer(desc, dev_addr, r_addr_sz, r_addr, (uint8_t *)data, size, XFER_MODE_WRITE);
}

static hdl_module_state_t _hdl_i2c_mem_init(const void *desc, uint8_t enable) {
  hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)desc;
  hdl_i2c_mem_var_t *mem_var = (hdl_i2c_mem_var_t *)mem->obj_var;
  if(enable) {
    mem_var->xfer_state = XFER_STATE_IDLE;
    coroutine_add(&mem_var->worker, &_mem_worker, (void*)mem);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&mem_var->worker);
  return HDL_MODULE_UNLOADED;
}

const hdl_i2c_mem_iface_t hdl_i2c_mem_iface = {
  .init = &_hdl_i2c_mem_init,
  .read = &_hdl_i2c_mem_read,
  .write = &_hdl_i2c_mem_write,
  .state = &_hdl_i2c_mem_get_state
};

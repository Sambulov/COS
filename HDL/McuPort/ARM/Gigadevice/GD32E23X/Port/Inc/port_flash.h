#ifndef PORT_FLASH_H_
#define PORT_FLASH_H_

#define HDL_FLASH_JOB_PRV_SIZE    20

typedef enum {
  FLASH_JOB_SATE_UNKNOWN,
  FLASH_JOB_SATE_ENQUEUED,
  FLASH_JOB_SATE_EXECUTING,
  FLASH_JOB_SATE_READY,
  FLASH_JOB_SATE_FAULT,
} hdl_flash_state_t;

typedef struct {
  hdl_module_t module;
  uint32_t flash_latency;
} hdl_flash_t;

typedef struct {
  uint32_t start_addr;
  uint32_t end_addr;
  uint8_t *data;
  void (*handler)(hdl_flash_t *desc);
  PRIVATE(hw, HDL_FLASH_JOB_PRV_SIZE);
} hdl_flash_job_t;

hdl_module_state_t hdl_flash(void *desc, uint8_t enable);

hdl_flash_state_t hdl_flash_job_state(hdl_flash_t *desc, hdl_flash_job_t *job);
hdl_flash_state_t hdl_flash_job_enqueue(hdl_flash_t *desc, hdl_flash_job_t *job);
hdl_flash_state_t hdl_flash_job_cancel(hdl_flash_t *desc, hdl_flash_job_t *job);

void hdl_flash_job_read(hdl_flash_t *desc);
void hdl_flash_job_write(hdl_flash_t *desc);
void hdl_flash_job_erase(hdl_flash_t *desc);

#endif /* PORT_FLASH_H_ */

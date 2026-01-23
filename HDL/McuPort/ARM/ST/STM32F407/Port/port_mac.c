#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  uint8_t rx_desc_cnt;
  uint16_t rx_buff_len;
  ETH_DMADescTypeDef *tx_desc;
  ETH_DMADescTypeDef *rx_desc;
  ETH_TxPacketConfigTypeDef tx_config;

  // hdl_mac_cmd_message_t *cmd_msg;
  // hdl_mac_data_message_t *data_msg;
  // uint8_t cmd_state  : 4;
  // uint8_t data_state : 4;
  // uint32_t mac_ck; /* mac_ck = Pf/(div+2) */
} hdl_mac_mcu_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_mac_mcu_var_t, *((hdl_mac_mcu_t *)0)->obj_var, HDL_MAC_MCU_VAR_SIZE, port_mac.h);

static uint8_t _mac_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_mac_mcu_t *mac = (hdl_mac_mcu_t *)arg;
  hdl_mac_mcu_var_t *mac_var = (hdl_mac_mcu_var_t *)mac->obj_var;
  (void)mac_var;

  return cancel;
}

ETH_HandleTypeDef heth;


static void up_timer(void *event_trigger, void *sender, void *context) {
  (void)event_trigger; (void)sender; (void)context;
  HAL_IncTick();
}

static void _eth_mac_set_default_config(ETH_TypeDef *phy, const uint8_t *mac_addr) {
  CL_REG_MODIFY(phy->MACCR,
    ETH_MACCR_CSTF | ETH_MACCR_WD | ETH_MACCR_JD | ETH_MACCR_IFG | ETH_MACCR_CSD |
    ETH_MACCR_FES | ETH_MACCR_ROD | ETH_MACCR_LM | ETH_MACCR_DM | ETH_MACCR_IPCO |
    ETH_MACCR_RD | ETH_MACCR_APCS | ETH_MACCR_BL | ETH_MACCR_DC
    ,
    ETH_MACCR_CSTF | ETH_MACCR_IFG_96Bit | ETH_MACCR_FES | 
    ETH_MACCR_ROD | ETH_MACCR_DM | ETH_MACCR_IPCO | ETH_MACCR_RD |
    ETH_MACCR_BL_10  
  );
  uint32_t timer = 1000;
  while (timer--);
  CL_REG_MODIFY(phy->MACFCR,
    ETH_MACFCR_PT | ETH_MACFCR_ZQPD | ETH_MACFCR_PLT | ETH_MACFCR_UPFD | ETH_MACFCR_RFCE | ETH_MACFCR_TFCE
    ,
    ETH_MACFCR_ZQPD | ETH_MACFCR_PLT_Minus4
  );
  timer = 1000;
  while (timer--);
  phy->MACA0HR = ((uint32_t)mac_addr[5U] << 8U) | (uint32_t)mac_addr[4U];
  phy->MACA0LR = ((uint32_t)mac_addr[3U] << 24U) | ((uint32_t)mac_addr[2U] << 16U) | 
                            ((uint32_t)mac_addr[1U] << 8U) | mac_addr[0U];
  /* Disable MMC Interrupts */
  CL_REG_SET(phy->MACIMR, ETH_MACIMR_TSTIM | ETH_MACIMR_PMTIM);
  /* Disable Rx MMC Interrupts */
  CL_REG_SET(phy->MMCRIMR, ETH_MMCRIMR_RGUFM | ETH_MMCRIMR_RFAEM | ETH_MMCRIMR_RFCEM);
  /* Disable Tx MMC Interrupts */
  CL_REG_SET(phy->MMCTIMR, ETH_MMCTIMR_TGFM | ETH_MMCTIMR_TGFMSCM | ETH_MMCTIMR_TGFSCM);
}

static void _eth_dma_set_default_config(ETH_TypeDef *phy) {
  CL_REG_MODIFY(phy->DMAOMR,
    ETH_DMAOMR_DTCEFD | ETH_DMAOMR_RSF | ETH_DMAOMR_FTF | ETH_DMAOMR_TSF | 
    ETH_DMAOMR_TTC | ETH_DMAOMR_FEF | ETH_DMAOMR_FUGF | ETH_DMAOMR_RTC | ETH_DMAOMR_OSF |
    ETH_DMAOMR_DFRF
    ,
    ETH_DMAOMR_RSF | ETH_DMAOMR_TSF | ETH_DMAOMR_TTC_64Bytes | ETH_DMAOMR_RTC_64Bytes | ETH_DMAOMR_OSF
  );
  uint32_t timer = 1000;
  while (timer--);
  CL_REG_MODIFY(phy->DMABMR,
    ETH_DMABMR_MB | ETH_DMABMR_AAB | ETH_DMABMR_FB | ETH_DMABMR_FPM | 
    ETH_DMABMR_USP | ETH_DMABMR_RDP | ETH_DMABMR_PBL | ETH_DMABMR_EDE | 
    ETH_DMABMR_DSL | ETH_DMABMR_DA | ETH_DMABMR_RTPR
    ,
    ETH_DMABMR_AAB | ETH_DMABMR_FB | ETH_DMABMR_USP | ETH_DMABMR_RDP_32Beat | 
    ETH_DMABMR_PBL_32Beat | ETH_DMABMR_EDE | ETH_DMABMR_RTPR_1_1
  );
  timer = 1000;
  while (timer--);
}

static void _eth_stop(ETH_TypeDef *phy) {
  CL_REG_CLEAR(phy->DMAOMR, ETH_DMAOMR_ST | ETH_DMAOMR_SR | ETH_DMAOMR_FTF);
  CL_REG_CLEAR(phy->MACCR, ETH_MACCR_RE);
  uint32_t timer = 1000;
  while (timer--);
  CLEAR_BIT(phy->MACCR, ETH_MACCR_TE);
  timer = 1000;
  while (timer--);
}

static void _eth_start(ETH_TypeDef *phy) {
    CL_REG_SET(phy->MACCR, ETH_MACCR_TE);
    uint32_t timer = 1000;
    while (timer--);
    CL_REG_SET(phy->MACCR, ETH_MACCR_RE);
    CL_REG_SET(phy->DMAOMR, ETH_DMAOMR_FTF);
    timer = 1000;
    while (timer--);
    SET_BIT(phy->DMAOMR, ETH_DMAOMR_ST);
    SET_BIT(phy->DMAOMR, ETH_DMAOMR_SR);
}

static inline void _eth_rmii_select(uint32_t rmii_mode) {
  //CL_REG_SET(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
  //(void)RCC->APB2ENR;
  if(rmii_mode) rmii_mode = SYSCFG_PMC_MII_RMII_SEL;
  CL_REG_MODIFY(SYSCFG->PMC, SYSCFG_PMC_MII_RMII_SEL, rmii_mode);
  (void)SYSCFG->PMC; /* Dummy read to sync SYSCFG with ETH */
}

static inline uint8_t _eth_soft_reset(ETH_TypeDef *phy) {
  /* Ethernet Software reset */
  /* Set the SWR bit: resets all MAC subsystem internal registers and logic */
  /* After reset all the registers holds their respective reset values */
  CL_REG_SET(phy->DMABMR, ETH_DMABMR_SR);
  uint32_t time = 10000;
  while (CL_REG_GET(phy->DMABMR, ETH_DMABMR_SR) && time--);
  return (time != 0);
}

static void ETH_DMATxDescListInit(ETH_HandleTypeDef *heth)
{
  ETH_DMADescTypeDef *dmatxdesc;
  uint32_t i;

  /* Fill each DMATxDesc descriptor with the right values */
  for (i = 0; i < (uint32_t)ETH_TX_DESC_CNT; i++)
  {
    dmatxdesc = heth->Init.TxDesc + i;

    WRITE_REG(dmatxdesc->DESC0, 0x0U);
    WRITE_REG(dmatxdesc->DESC1, 0x0U);
    WRITE_REG(dmatxdesc->DESC2, 0x0U);
    WRITE_REG(dmatxdesc->DESC3, 0x0U);

    WRITE_REG(heth->TxDescList.TxDesc[i], (uint32_t)dmatxdesc);

    /* Set Second Address Chained bit */
    SET_BIT(dmatxdesc->DESC0, ETH_DMATXDESC_TCH);

    if (i < ((uint32_t)ETH_TX_DESC_CNT - 1U))
    {
      WRITE_REG(dmatxdesc->DESC3, (uint32_t)(heth->Init.TxDesc + i + 1U));
    }
    else
    {
      WRITE_REG(dmatxdesc->DESC3, (uint32_t)(heth->Init.TxDesc));
    }

    /* Set the DMA Tx descriptors checksum insertion */
    SET_BIT(dmatxdesc->DESC0, ETH_DMATXDESC_CHECKSUMTCPUDPICMPFULL);
  }

  heth->TxDescList.CurTxDesc = 0;

  /* Set Transmit Descriptor List Address */
  WRITE_REG(heth->Instance->DMATDLAR, (uint32_t) heth->Init.TxDesc);
}

static void ETH_DMARxDescListInit(ETH_HandleTypeDef *heth)
{
  ETH_DMADescTypeDef *dmarxdesc;
  uint32_t i;

  for (i = 0; i < (uint32_t)ETH_RX_DESC_CNT; i++)
  {
    dmarxdesc =  heth->Init.RxDesc + i;

    WRITE_REG(dmarxdesc->DESC0, 0x0U);
    WRITE_REG(dmarxdesc->DESC1, 0x0U);
    WRITE_REG(dmarxdesc->DESC2, 0x0U);
    WRITE_REG(dmarxdesc->DESC3, 0x0U);
    WRITE_REG(dmarxdesc->BackupAddr0, 0x0U);
    WRITE_REG(dmarxdesc->BackupAddr1, 0x0U);

    /* Set Own bit of the Rx descriptor Status */
    dmarxdesc->DESC0 = ETH_DMARXDESC_OWN;

    /* Set Buffer1 size and Second Address Chained bit */
    dmarxdesc->DESC1 = heth->Init.RxBuffLen | ETH_DMARXDESC_RCH;

    /* Enable Ethernet DMA Rx Descriptor interrupt */
    dmarxdesc->DESC1 &= ~ETH_DMARXDESC_DIC;
    /* Set Rx descritors addresses */
    WRITE_REG(heth->RxDescList.RxDesc[i], (uint32_t)dmarxdesc);

    if (i < ((uint32_t)ETH_RX_DESC_CNT - 1U))
    {
      WRITE_REG(dmarxdesc->DESC3, (uint32_t)(heth->Init.RxDesc + i + 1U));
    }
    else
    {
      WRITE_REG(dmarxdesc->DESC3, (uint32_t)(heth->Init.RxDesc));
    }
  }

  WRITE_REG(heth->RxDescList.RxDescIdx, 0U);
  WRITE_REG(heth->RxDescList.RxDescCnt, 0U);
  WRITE_REG(heth->RxDescList.RxBuildDescIdx, 0U);
  WRITE_REG(heth->RxDescList.RxBuildDescCnt, 0U);
  WRITE_REG(heth->RxDescList.ItMode, 0U);

  /* Set Receive Descriptor List Address */
  WRITE_REG(heth->Instance->DMARDLAR, (uint32_t) heth->Init.RxDesc);
}

static hdl_module_state_t _hdl_mac(const void *desc, uint8_t enable) {
  hdl_mac_mcu_t *mac = (hdl_mac_mcu_t *)desc;
  hdl_mac_mcu_var_t *mac_var = (hdl_mac_mcu_var_t *)mac->obj_var;
  ETH_TypeDef *phy = (ETH_TypeDef *)mac->config->phy;
  if(enable) {
    /* todo */
    static hdl_delegate_t up_timer_delegate = {
      .context = NULL,
      .handler = &up_timer
    };
    hdl_interrupt_request(mac->dependencies[2], &up_timer_delegate);    

    __HAL_RCC_ETHMAC_CLK_ENABLE();
    __HAL_RCC_ETHMACTX_CLK_ENABLE();
    __HAL_RCC_ETHMACRX_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    /* todo */
    static ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
    static ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
    heth.Instance = phy;
    heth.Init.MACAddr = (uint8_t *)mac->config->mac_addr;
    heth.Init.TxDesc = DMATxDscrTab;
    heth.Init.RxDesc = DMARxDscrTab;
    heth.Init.RxBuffLen = 1536;
    heth.Init.MediaInterface = HAL_ETH_RMII_MODE;


    //mac_var->tx_desc = DMATxDscrTab;
    //mac_var->rx_desc = DMARxDscrTab;
    //mac_var->rx_buff_len = 1536;


    _eth_rmii_select(mac->config->rmii_mode);

    if(!_eth_soft_reset(phy)) return HDL_MODULE_FAULT;


    HAL_ETH_Init(&heth);


    _eth_mac_set_default_config(phy, mac->config->mac_addr);
    _eth_dma_set_default_config(phy);


    /*------------------ DMA Tx Descriptors Configuration ----------------------*/
    ETH_DMATxDescListInit(&heth);

    /*------------------ DMA Rx Descriptors Configuration ----------------------*/
    ETH_DMARxDescListInit(&heth);

    _eth_stop(phy);



    hdl_clock_freq_t eth_clk;
    hdl_clock_get(mac->dependencies[0], &eth_clk);
    uint32_t clk = (eth_clk.num + (eth_clk.denom - 1)) / eth_clk.denom;
    uint32_t div;
    if (clk < 35000000U) div = ETH_MACMIIAR_CR_Div16;
    else if (clk < 60000000U) div = ETH_MACMIIAR_CR_Div26;
    else if (clk < 100000000U) div = ETH_MACMIIAR_CR_Div42;
    else if (clk < 150000000U) div = ETH_MACMIIAR_CR_Div62;
    else div = ETH_MACMIIAR_CR_Div102;

    CL_REG_MODIFY(phy->MACMIIAR, ETH_MACMIIAR_CR, div);

    /* Pass all multicast frames: needed for IPv6 protocol*/
    phy->MACFFR |= mac->config->pass_multicast? ETH_MACFFR_PM: 0;

    mac_var->tx_config.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
    mac_var->tx_config.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
    mac_var->tx_config.CRCPadCtrl = ETH_CRC_PAD_INSERT;

    coroutine_add(&mac_var->worker, &_mac_worker, mac);
    return  HDL_MODULE_ACTIVE;
  }
  __HAL_RCC_ETH_CLK_DISABLE();
  coroutine_cancel(&mac_var->worker);
  return HDL_MODULE_UNLOADED;
}

#define MDIO_MODE_WRITE    0
#define MDIO_MODE_READ     1

static uint8_t _hdl_mac_mdio_rw(const void *desc, hdl_mac_mdio_message_t *message, hdl_mac_mdio_cb_t cb, uint8_t mode) {
  (void)cb; (void)desc;
  hdl_mac_mcu_t *mac = (hdl_mac_mcu_t *)desc;
  ETH_TypeDef *phy = (ETH_TypeDef *)mac->config->phy;
  uint32_t tmpreg = CL_REG_GET(phy->MACMIIAR, ETH_MACMIIAR_CR);
  const uint8_t mii_div[] = {42, 62, 16, 26, 102};
  uint32_t time = (uint32_t)mii_div[tmpreg >> ETH_MACMIIAR_CR_Pos] * 32 * 10;
  tmpreg |= (((uint32_t)message->dev_addr << ETH_MACMIIAR_PA_Pos) & ETH_MACMIIAR_PA);
  tmpreg |= (((uint32_t)message->reg_addr << ETH_MACMIIAR_MR_Pos) & ETH_MACMIIAR_MR);
  tmpreg |= (mode == MDIO_MODE_WRITE)? ETH_MACMIIAR_MW : 0;
  tmpreg |= ETH_MACMIIAR_MB;
  phy->MACMIIDR = message->reg_val;
  phy->MACMIIAR = tmpreg;
  /* Check for the Busy flag */
  while (CL_REG_GET(phy->MACMIIAR, ETH_MACMIIAR_MB) && time--);
  if(!time) 
    return HDL_FALSE;
  if(mode == MDIO_MODE_READ) message->reg_val = phy->MACMIIDR;
  return HDL_TRUE;
}

static uint8_t _hdl_mac_mdio_read(const void *desc, hdl_mac_mdio_message_t *message, hdl_mac_mdio_cb_t cb) {
  return _hdl_mac_mdio_rw(desc, message, cb, MDIO_MODE_READ);
}

static uint8_t _hdl_mac_mdio_write(const void *desc, hdl_mac_mdio_message_t *message, hdl_mac_mdio_cb_t cb) {
  return _hdl_mac_mdio_rw(desc, message, cb, MDIO_MODE_WRITE);
}

static uint8_t _hdl_mac_get_cnf(const void *desc, hdl_mac_config_t *cnf) {
  if(cnf) {
    hdl_mac_mcu_t *mac = (hdl_mac_mcu_t *)desc;
    ETH_TypeDef *phy = (ETH_TypeDef *)mac->config->phy;
    cnf->half_duplex = !(READ_BIT(phy->MACCR, ETH_MACCR_DM));
    cnf->speed = READ_BIT(phy->MACCR, ETH_MACCR_FES)? 100000000: 10000000;
    cnf->mtu = ETH_MAX_PAYLOAD;
    uint32_t mac_h = phy->MACA0HR;
    uint32_t mac_l = phy->MACA0LR;
    cnf->mac_addr[5U] = mac_h >> 8;
    cnf->mac_addr[4U] = mac_h;
    cnf->mac_addr[3U] = mac_l >> 24;
    cnf->mac_addr[2U] = mac_l >> 16;
    cnf->mac_addr[1U] = mac_l >> 8;
    cnf->mac_addr[0U] = mac_l;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static void ETH_UpdateDescriptor(ETH_HandleTypeDef *heth)
{
  uint32_t descidx;
  uint32_t tailidx;
  uint32_t desccount;
  ETH_DMADescTypeDef *dmarxdesc;
  uint8_t *buff = NULL;
  uint8_t allocStatus = 1U;

  descidx = heth->RxDescList.RxBuildDescIdx;
  dmarxdesc = (ETH_DMADescTypeDef *)heth->RxDescList.RxDesc[descidx];
  desccount = heth->RxDescList.RxBuildDescCnt;

  while ((desccount > 0U) && (allocStatus != 0U))
  {
    /* Check if a buffer's attached the descriptor */
    if (READ_REG(dmarxdesc->BackupAddr0) == 0U)
    {
      /* Get a new buffer. */
#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)
      /*Call registered Allocate callback*/
      heth->rxAllocateCallback(&buff);
#else
      /* Allocate callback */
      HAL_ETH_RxAllocateCallback(&buff);
#endif  /* USE_HAL_ETH_REGISTER_CALLBACKS */
      if (buff == NULL)
      {
        allocStatus = 0U;
      }
      else
      {
        WRITE_REG(dmarxdesc->BackupAddr0, (uint32_t)buff);
        WRITE_REG(dmarxdesc->DESC2, (uint32_t)buff);
      }
    }

    if (allocStatus != 0U)
    {
      if (heth->RxDescList.ItMode == 0U)
      {
        WRITE_REG(dmarxdesc->DESC1, heth->Init.RxBuffLen | ETH_DMARXDESC_DIC | ETH_DMARXDESC_RCH);
      }
      else
      {
        WRITE_REG(dmarxdesc->DESC1, heth->Init.RxBuffLen | ETH_DMARXDESC_RCH);
      }

      SET_BIT(dmarxdesc->DESC0, ETH_DMARXDESC_OWN);

      /* Increment current rx descriptor index */

#define INCR_RX_DESC_INDEX(inx, offset) do {\
                                             (inx) += (offset);\
                                             if ((inx) >= (uint32_t)ETH_RX_DESC_CNT){\
                                             (inx) = ((inx) - (uint32_t)ETH_RX_DESC_CNT);}\
                                           } while (0)


      INCR_RX_DESC_INDEX(descidx, 1U);
      /* Get current descriptor address */
      dmarxdesc = (ETH_DMADescTypeDef *)heth->RxDescList.RxDesc[descidx];
      desccount--;
    }
  }

  if (heth->RxDescList.RxBuildDescCnt != desccount)
  {
    /* Set the tail pointer index */
    tailidx = (ETH_RX_DESC_CNT + descidx - 1U) % ETH_RX_DESC_CNT;

    /* DMB instruction to avoid race condition */
    __DMB();

    /* Set the Tail pointer address */
    WRITE_REG(heth->Instance->DMARPDR, ((uint32_t)(heth->Init.RxDesc + (tailidx))));

    heth->RxDescList.RxBuildDescIdx = descidx;
    heth->RxDescList.RxBuildDescCnt = desccount;
  }
}

static uint8_t _hdl_mac_set_cnf(const void *desc, hdl_mac_config_t *cnf) {
  if(cnf) {
    hdl_mac_mcu_t *mac = (hdl_mac_mcu_t *)desc;
    hdl_mac_mcu_var_t *mac_var = (hdl_mac_mcu_var_t *)mac->obj_var;
    ETH_TypeDef *phy = (ETH_TypeDef *)mac->config->phy;
    _eth_stop(phy);
    if(cnf->half_duplex) CL_REG_CLEAR(phy->MACCR, ETH_MACCR_DM);
    else CL_REG_SET(phy->MACCR, ETH_MACCR_DM);
    if(cnf->speed == 10000000) CL_REG_CLEAR(phy->MACCR, ETH_MACCR_FES);
    else if(cnf->speed == 100000000) CL_REG_SET(phy->MACCR, ETH_MACCR_FES);
    else return HDL_FALSE;
    phy->MACA0HR = ((uint32_t)cnf->mac_addr[5U] << 8U) | (uint32_t)cnf->mac_addr[4U];
    phy->MACA0LR = ((uint32_t)cnf->mac_addr[3U] << 24U) | ((uint32_t)cnf->mac_addr[2U] << 16U) | 
                             ((uint32_t)cnf->mac_addr[1U] << 8U) | cnf->mac_addr[0U];

    /* Set number of descriptors to build */
    mac_var->rx_desc_cnt = ETH_RX_DESC_CNT;
    heth.RxDescList.RxBuildDescCnt = ETH_RX_DESC_CNT;
    /* Build all descriptors */
    ETH_UpdateDescriptor(&heth);

    _eth_start(phy);
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _hdl_mac_transmit(const void *desc, ETH_BufferTypeDef *data, uint32_t total_len) {
  hdl_mac_mcu_t *mac = (hdl_mac_mcu_t *)desc;
  hdl_mac_mcu_var_t *mac_var = (hdl_mac_mcu_var_t *)mac->obj_var;
  mac_var->tx_config.Length = total_len;
  mac_var->tx_config.TxBuffer = data;
  /* todo */
  heth.gState = HAL_ETH_STATE_STARTED;
  HAL_ETH_Transmit(&heth, &mac_var->tx_config, 20);
  return HDL_TRUE;
}

static uint8_t _hdl_mac_receive(const void *desc, void **data) {
  (void)desc;
  /* todo */
  heth.gState = HAL_ETH_STATE_STARTED;
  HAL_ETH_ReadData(&heth, data);
  return HDL_TRUE;
}

const hdl_mac_iface_t hdl_mac_mcu_iface = {
  .init = &_hdl_mac,
  .read_reg = &_hdl_mac_mdio_read,
  .write_reg = &_hdl_mac_mdio_write,
  .transmit = &_hdl_mac_transmit,
  .receive = &_hdl_mac_receive,
  .get_cnf = &_hdl_mac_get_cnf,
  .set_cnf = &_hdl_mac_set_cnf
};

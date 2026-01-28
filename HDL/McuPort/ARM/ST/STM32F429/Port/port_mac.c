#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  hdl_delegate_t phy_delegate;
  uint8_t rx_desc_cnt;
  uint16_t rx_buff_len;
  ETH_DMADescTypeDef *tx_desc;
  ETH_DMADescTypeDef *rx_desc;
  ETH_TxPacketConfigTypeDef tx_config;
  hdl_mac_buffer_allocator_t buffer_allocator;
  void *buf_alloc_context;
} hdl_mac_mcu_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_mac_mcu_var_t, *((hdl_mac_mcu_t *)0)->obj_var, HDL_ETH_MCU_VAR_SIZE, port_mac.h);

#define DEPENDENCY_CLOCK 0
#define DEPENDENCY_PHY   3

static uint8_t _mac_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_mac_mcu_t *mac = (hdl_mac_mcu_t *)arg;
  hdl_mac_mcu_var_t *mac_var = (hdl_mac_mcu_var_t *)mac->obj_var;
  (void)mac_var;

  return cancel;
}

ETH_HandleTypeDef hmac;

static void up_timer(void *event_trigger, void *sender, void *context) {
  (void)event_trigger; (void)sender; (void)context;
  HAL_IncTick();
}

static void _mac_set_default_config(ETH_TypeDef *phy, const uint8_t *mac_addr) {
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

static void _mac_dma_set_default_config(ETH_TypeDef *phy) {
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

static void _mac_stop(ETH_TypeDef *phy) {
  CL_REG_CLEAR(phy->DMAOMR, ETH_DMAOMR_ST | ETH_DMAOMR_SR | ETH_DMAOMR_FTF);
  CL_REG_CLEAR(phy->MACCR, ETH_MACCR_RE);
  uint32_t timer = 1000;
  while (timer--);
  CLEAR_BIT(phy->MACCR, ETH_MACCR_TE);
  timer = 1000;
  while (timer--);
}

static void _mac_start(ETH_TypeDef *phy) {
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

static inline void _mac_rmac_select(uint32_t rmac_mode) {
  CL_REG_SET(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
  (void)RCC->APB2ENR;
  if(rmac_mode) rmac_mode = SYSCFG_PMC_MII_RMII_SEL;
  CL_REG_MODIFY(SYSCFG->PMC, SYSCFG_PMC_MII_RMII_SEL, rmac_mode);
  (void)SYSCFG->PMC; /* Dummy read to sync SYSCFG with ETH */
}

static inline uint8_t _mac_soft_reset(ETH_TypeDef *phy) {
  /* Ethernet Software reset */
  /* Set the SWR bit: resets all MAC subsystem internal registers and logic */
  /* After reset all the registers holds their respective reset values */
  CL_REG_SET(phy->DMABMR, ETH_DMABMR_SR);
  uint32_t time = 10000;
  while (CL_REG_GET(phy->DMABMR, ETH_DMABMR_SR) && time--);
  return (time != 0);
}

static void ETH_DMATxDescListInit(ETH_HandleTypeDef *hmac)
{
  ETH_DMADescTypeDef *dmatxdesc;
  uint32_t i;

  /* Fill each DMATxDesc descriptor with the right values */
  for (i = 0; i < (uint32_t)ETH_TX_DESC_CNT; i++)
  {
    dmatxdesc = hmac->Init.TxDesc + i;

    WRITE_REG(dmatxdesc->DESC0, 0x0U);
    WRITE_REG(dmatxdesc->DESC1, 0x0U);
    WRITE_REG(dmatxdesc->DESC2, 0x0U);
    WRITE_REG(dmatxdesc->DESC3, 0x0U);

    WRITE_REG(hmac->TxDescList.TxDesc[i], (uint32_t)dmatxdesc);

    /* Set Second Address Chained bit */
    SET_BIT(dmatxdesc->DESC0, ETH_DMATXDESC_TCH);

    if (i < ((uint32_t)ETH_TX_DESC_CNT - 1U))
    {
      WRITE_REG(dmatxdesc->DESC3, (uint32_t)(hmac->Init.TxDesc + i + 1U));
    }
    else
    {
      WRITE_REG(dmatxdesc->DESC3, (uint32_t)(hmac->Init.TxDesc));
    }

    /* Set the DMA Tx descriptors checksum insertion */
    SET_BIT(dmatxdesc->DESC0, ETH_DMATXDESC_CHECKSUMTCPUDPICMPFULL);
  }

  hmac->TxDescList.CurTxDesc = 0;

  /* Set Transmit Descriptor List Address */
  WRITE_REG(hmac->Instance->DMATDLAR, (uint32_t) hmac->Init.TxDesc);
}

static void ETH_DMARxDescListInit(ETH_HandleTypeDef *hmac)
{
  ETH_DMADescTypeDef *dmarxdesc;
  uint32_t i;

  for (i = 0; i < (uint32_t)ETH_RX_DESC_CNT; i++)
  {
    dmarxdesc =  hmac->Init.RxDesc + i;

    WRITE_REG(dmarxdesc->DESC0, 0x0U);
    WRITE_REG(dmarxdesc->DESC1, 0x0U);
    WRITE_REG(dmarxdesc->DESC2, 0x0U);
    WRITE_REG(dmarxdesc->DESC3, 0x0U);
    WRITE_REG(dmarxdesc->BackupAddr0, 0x0U);
    WRITE_REG(dmarxdesc->BackupAddr1, 0x0U);

    /* Set Own bit of the Rx descriptor Status */
    dmarxdesc->DESC0 = ETH_DMARXDESC_OWN;

    /* Set Buffer1 size and Second Address Chained bit */
    dmarxdesc->DESC1 = hmac->Init.RxBuffLen | ETH_DMARXDESC_RCH;

    /* Enable Ethernet DMA Rx Descriptor interrupt */
    dmarxdesc->DESC1 &= ~ETH_DMARXDESC_DIC;
    /* Set Rx descritors addresses */
    WRITE_REG(hmac->RxDescList.RxDesc[i], (uint32_t)dmarxdesc);

    if (i < ((uint32_t)ETH_RX_DESC_CNT - 1U))
    {
      WRITE_REG(dmarxdesc->DESC3, (uint32_t)(hmac->Init.RxDesc + i + 1U));
    }
    else
    {
      WRITE_REG(dmarxdesc->DESC3, (uint32_t)(hmac->Init.RxDesc));
    }
  }

  WRITE_REG(hmac->RxDescList.RxDescIdx, 0U);
  WRITE_REG(hmac->RxDescList.RxDescCnt, 0U);
  WRITE_REG(hmac->RxDescList.RxBuildDescIdx, 0U);
  WRITE_REG(hmac->RxDescList.RxBuildDescCnt, 0U);
  WRITE_REG(hmac->RxDescList.ItMode, 0U);

  /* Set Receive Descriptor List Address */
  WRITE_REG(hmac->Instance->DMARDLAR, (uint32_t) hmac->Init.RxDesc);
}

static void ETH_UpdateDescriptor(ETH_HandleTypeDef *hmac)
{
  uint32_t descidx;
  uint32_t tailidx;
  uint32_t desccount;
  ETH_DMADescTypeDef *dmarxdesc;
  uint8_t *buff = NULL;
  uint8_t allocStatus = 1U;

  descidx = hmac->RxDescList.RxBuildDescIdx;
  dmarxdesc = (ETH_DMADescTypeDef *)hmac->RxDescList.RxDesc[descidx];
  desccount = hmac->RxDescList.RxBuildDescCnt;

  while ((desccount > 0U) && (allocStatus != 0U))
  {
    /* Check if a buffer's attached the descriptor */
    if (READ_REG(dmarxdesc->BackupAddr0) == 0U)
    {
      /* Get a new buffer. */
#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)
      /*Call registered Allocate callback*/
      hmac->rxAllocateCallback(&buff);
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
      if (hmac->RxDescList.ItMode == 0U)
      {
        WRITE_REG(dmarxdesc->DESC1, hmac->Init.RxBuffLen | ETH_DMARXDESC_DIC | ETH_DMARXDESC_RCH);
      }
      else
      {
        WRITE_REG(dmarxdesc->DESC1, hmac->Init.RxBuffLen | ETH_DMARXDESC_RCH);
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
      dmarxdesc = (ETH_DMADescTypeDef *)hmac->RxDescList.RxDesc[descidx];
      desccount--;
    }
  }

  if (hmac->RxDescList.RxBuildDescCnt != desccount)
  {
    /* Set the tail pointer index */
    tailidx = (ETH_RX_DESC_CNT + descidx - 1U) % ETH_RX_DESC_CNT;

    /* DMB instruction to avoid race condition */
    __DMB();

    /* Set the Tail pointer address */
    WRITE_REG(hmac->Instance->DMARPDR, ((uint32_t)(hmac->Init.RxDesc + (tailidx))));

    hmac->RxDescList.RxBuildDescIdx = descidx;
    hmac->RxDescList.RxBuildDescCnt = desccount;
  }
}

static uint8_t _hdl_mac_set_cnf(const void *desc, hdl_mac_config_t *cnf) {
  if(cnf) {
    hdl_mac_mcu_t *mac = (hdl_mac_mcu_t *)desc;
    hdl_mac_mcu_var_t *mac_var = (hdl_mac_mcu_var_t *)mac->obj_var;
    ETH_TypeDef *phy = ETH;
    _mac_stop(phy);
    phy->MACA0HR = ((uint32_t)cnf->mac_addr[5U] << 8U) | (uint32_t)cnf->mac_addr[4U];
    phy->MACA0LR = ((uint32_t)cnf->mac_addr[3U] << 24U) | ((uint32_t)cnf->mac_addr[2U] << 16U) | 
                             ((uint32_t)cnf->mac_addr[1U] << 8U) | cnf->mac_addr[0U];

    /* Set number of descriptors to build */
    mac_var->rx_desc_cnt = ETH_RX_DESC_CNT;
    hmac.RxDescList.RxBuildDescCnt = ETH_RX_DESC_CNT;
    /* Build all descriptors */
    ETH_UpdateDescriptor(&hmac);

    _mac_start(phy);
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static void phy_event_handler(void *event_trigger, void *sender, void *context) {
  (void)sender; (void)context; 
  eth_phy_state_t *phy_state = (eth_phy_state_t *)event_trigger;
  ETH_TypeDef *mac_periph = ETH;

  _mac_stop(mac_periph);

  if(phy_state->link & ETH_PHY_DUPLEX_FULL) CL_REG_SET(mac_periph->MACCR, ETH_MACCR_DM);
  else CL_REG_CLEAR(mac_periph->MACCR, ETH_MACCR_DM);

  if((phy_state->link & ETH_PHY_SPEED) == ETH_PHY_SPEED_100M) CL_REG_SET(mac_periph->MACCR, ETH_MACCR_FES);
  else CL_REG_CLEAR(mac_periph->MACCR, ETH_MACCR_FES);

  /* Set number of descriptors to build */
  hmac.RxDescList.RxBuildDescCnt = ETH_RX_DESC_CNT;
  /* Build all descriptors */
  ETH_UpdateDescriptor(&hmac);
  _mac_start(mac_periph);
}

static hdl_module_state_t _hdl_mac(const void *desc, uint8_t enable) {
  hdl_mac_mcu_t *mac = (hdl_mac_mcu_t *)desc;
  hdl_mac_mcu_var_t *mac_var = (hdl_mac_mcu_var_t *)mac->obj_var;
  ETH_TypeDef *phy = ETH;
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
    hmac.Instance = phy;
    hmac.Init.MACAddr = (uint8_t *)mac->config->mac_addr;
    hmac.Init.TxDesc = DMATxDscrTab;
    hmac.Init.RxDesc = DMARxDscrTab;
    hmac.Init.RxBuffLen = 1536;
    hmac.Init.MediaInterface = HAL_ETH_RMII_MODE;


    //mac_var->tx_desc = DMATxDscrTab;
    //mac_var->rx_desc = DMARxDscrTab;
    //mac_var->rx_buff_len = 1536;


    _mac_rmac_select(mac->config->rmii_mode);

    if(!_mac_soft_reset(phy)) return HDL_MODULE_FAULT;


    HAL_ETH_Init(&hmac);


    _mac_set_default_config(phy, mac->config->mac_addr);
    _mac_dma_set_default_config(phy);    

    /*------------------ DMA Tx Descriptors Configuration ----------------------*/
    ETH_DMATxDescListInit(&hmac);

    /*------------------ DMA Rx Descriptors Configuration ----------------------*/
    ETH_DMARxDescListInit(&hmac);

    _mac_stop(phy);



    hdl_clock_freq_t mac_clk;
    hdl_clock_get(mac->dependencies[DEPENDENCY_CLOCK], &mac_clk);
    uint32_t clk = (mac_clk.num + (mac_clk.denom - 1)) / mac_clk.denom;
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


    hdl_eth_phy_t *phy = (hdl_eth_phy_t *)mac->dependencies[DEPENDENCY_PHY];
    mac_var->phy_delegate.context = mac;
    mac_var->phy_delegate.handler = phy_event_handler;
    hdl_eth_phy_subscribe(phy, &mac_var->phy_delegate);
    
    coroutine_add(&mac_var->worker, &_mac_worker, mac);
    return  HDL_MODULE_ACTIVE;
  }
  __HAL_RCC_ETH_CLK_DISABLE();
  coroutine_cancel(&mac_var->worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_mac_get_cnf(const void *desc, hdl_mac_config_t *cnf) {
  (void)desc;
  if(cnf) {
    //hdl_mac_mcu_t *mac = (hdl_mac_mcu_t *)desc;
    ETH_TypeDef *phy = ETH;
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

static uint8_t _hdl_mac_transmit(const void *desc, hdl_mac_buffer_t *buf, uint32_t total_len) {
  hdl_mac_mcu_t *mac = (hdl_mac_mcu_t *)desc;
  hdl_mac_mcu_var_t *mac_var = (hdl_mac_mcu_var_t *)mac->obj_var;
  mac_var->tx_config.Length = total_len;
  mac_var->tx_config.TxBuffer = (ETH_BufferTypeDef *)buf;
  /* todo */
  hmac.gState = HAL_ETH_STATE_STARTED;
  HAL_ETH_Transmit(&hmac, &mac_var->tx_config, 20);
  return HDL_TRUE;
}

static uint8_t _hdl_mac_receive(const void *desc, void **data) {
  (void)desc;
  /* todo */
  hmac.gState = HAL_ETH_STATE_STARTED;
  HAL_ETH_ReadData(&hmac, data);
  return HDL_TRUE;
}

static uint8_t _hdl_mac_get_phy(const void *desc, hdl_eth_phy_t **phy) {
  if(phy) {
    hdl_mac_mcu_t *mac = (hdl_mac_mcu_t *)desc;
    *phy = (hdl_eth_phy_t *)mac->dependencies[DEPENDENCY_PHY];
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static void _hdl_mac_set_buffer_allocator(const void *desc, hdl_mac_buffer_allocator_t allocator, void *context) {
  hdl_mac_mcu_t *mac = (hdl_mac_mcu_t *)desc;
  hdl_mac_mcu_var_t *mac_var = (hdl_mac_mcu_var_t *)mac->obj_var;
  mac_var->buffer_allocator = allocator;
  mac_var->buf_alloc_context = context;
}

const hdl_mac_iface_t hdl_mac_mcu_iface = {
  .init = &_hdl_mac,
  .transmit = &_hdl_mac_transmit,
  .receive = &_hdl_mac_receive,
  .get_cnf = &_hdl_mac_get_cnf,
  .set_cnf = &_hdl_mac_set_cnf,
  .get_phy = &_hdl_mac_get_phy,
  .set_buf_alloc = &_hdl_mac_set_buffer_allocator
};

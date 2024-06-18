#include "device_logic.h"

hdl_module_t connectors = {
    .dependencies = hdl_module_dependencies(
    /***********************************************************
     *                        X1 Connector
    ***********************************************************/
        &mod_di_module_address_1.module, &mod_di_module_address_2.module, &mod_di_module_address_3.module,
        &mod_di_module_address_4.module, &mod_di_module_address_5.module, &mod_di_external_periph_irq_1.module,
        &mod_di_external_periph_irq_2.module, &mod_do_external_output_1.module, &mod_do_external_output_2.module,
    /***********************************************************
     *                        Other
    ***********************************************************/
        &mod_do_pci_switch.module, &mod_do_lte_reset.module
    )
};

void connector_init() {
    hdl_enable(&connectors);
}

void connector_lte_reset(uint8_t enable) {
    if(enable)
        HDL_GPIO_SET_ACTIVE(&mod_do_lte_reset);
    else
        HDL_GPIO_SET_INACTIVE(&mod_do_lte_reset);
}

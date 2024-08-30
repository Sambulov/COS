#ifndef APP_H_
#define APP_H_

#include "hdl.h"
#include "ms5194t.h"
#include "uspd_ain_port.h"

#include "mig.h"

extern hdl_uspd_ain_port_t hdl_uspd_ain_port1;
extern hdl_uspd_ain_port_t hdl_uspd_ain_port2;
extern hdl_uspd_ain_port_t hdl_uspd_ain_port3;
extern hdl_uspd_ain_port_t hdl_uspd_ain_port4;

extern hdl_module_t mod_app;

void main();

#endif /* APP_H_ */

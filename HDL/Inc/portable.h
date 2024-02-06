#ifndef PORTABLE_H_
#define PORTABLE_H_

#if defined ( NUM46X )
  #include "../../MCU/NUM46X/Sys/inc/NuMicro.h"
#endif

#if defined ( GD32E23X )
  #include <stddef.h>
  #include "../../MCU/GD32E23X/Sys/inc/gd32e23x.h"
#endif

void vSystemInit(void);

void vSystemTask(void);

#endif // PORTABLE_H_

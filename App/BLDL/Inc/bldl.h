/*
    Busunes Logic Drivers Layer(BLDL)
    Project specific drivers.
*/

#ifndef BLDL_H_
#define BLDL_H_

#include "hdl.h"

/* TODO: SMARCV2P1 export as build parameter like board revision */
#define SMARCV2P1

#include "bldl_som_boot_lock.h"
#include "bldl_som_boot_select.h"
#include "bldl_som_state.h"
#include "bldl_common_protocol.h"

#ifdef USPD

#include "bldl_som_boot_lock.h"
#include "bldl_som_boot_select.h"
#include "bldl_som_state.h"
#include "bldl_uspd_io.h"
#include "bldl_common_protocol.h"

#endif

#ifdef ATB_3500

#include "bldl_som_boot_lock.h"
#include "bldl_som_boot_select.h"
#include "bldl_som_state.h"
#include "bldl_atb3500_io.h"
#include "bldl_common_protocol.h"

#endif

#endif // BLDL_H_

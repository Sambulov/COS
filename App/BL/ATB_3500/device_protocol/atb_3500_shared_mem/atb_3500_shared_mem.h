/*!
    \file atb_3500_shared_mem.h
    \author a.bodin
    
    \version 2024-06-05, V1.0.0, This file describe major protocol
 */

#ifndef ATB_3500_SHARED_MEM_H_
#define ATB_3500_SHARED_MEM_H_

#if defined(ATB_3500)

#define ATB_3500_SHARED_MEM_READ_SECTOR_SIZE    ((uint32_t)128)
#define ATB_3500_SHARED_MEM_WRITE_SECTOR_SIZE    ((uint32_t)64)

typedef struct {
    uint32_t write_sector[ATB_3500_SHARED_MEM_READ_SECTOR_SIZE];
} atb_3500_shared_mem_write_sector_t;

typedef struct {
    uint32_t read_sector[ATB_3500_SHARED_MEM_READ_SECTOR_SIZE];
} atb_3500_shared_mem_read_sector_t;

typedef struct {
    atb_3500_shared_mem_read_sector_t read_sector;
    atb_3500_shared_mem_write_sector_t write_sector;
} atb_3500_shared_mem_t;




#endif /* ATB_3500 */
#endif /* ATB_3500_SHARED_MEM_H_ */
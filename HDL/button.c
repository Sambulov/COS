#include "button.h"

// static void btn_process(sm_power_handle_t *handle) {
//   #define DEBOUNCE_DELAY 50
//   #define TIME_LONG_CLICK 5000

//     static uint8_t state_prev = 1;
//     static uint32_t time_start = 0;

//     // Проброс кнопки
//     handle->set_btn_pwron(!handle->get_x1_power_btn());

//     uint8_t state_current = handle->get_x1_power_btn();

//     if (state_prev == 1 && state_current == 0) // press
//     {
//         time_start = handle->millis();
//     }
//     else if (state_prev == 0 && state_current == 1) // release
//     {
//         uint32_t diff = handle->millis() - time_start;

//         if (diff >= DEBOUNCE_DELAY && diff < TIME_LONG_CLICK)
//         {
//             if (handle->soc_state == SOC_STATE_OFF)
//             {
//                 handle->soc_state = SOC_STATE_START;
//             }
//         }
//         else if (diff >= TIME_LONG_CLICK)
//         {
//             handle->soc_state = SOC_STATE_OFF;
//         }
//     }

//     state_prev = state_current;
// }

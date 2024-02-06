#include "sw_wdt.h"

void lib_wdt_soc_state_set(lib_wdt_soc_handle_t *handle, lib_wdt_soc_state_t state)
{
    handle->state = state;
}

lib_wdt_soc_state_t lib_wdt_soc_state_get(lib_wdt_soc_handle_t *handle)
{
    return handle->state;
}

void lib_wdt_soc_update(lib_wdt_soc_handle_t *handle)
{

    switch (handle->state)
    {
    case LIB_WDT_STATE_SOC_READY:
        // Ничего не делаем, ждём переключение из вне
        break;
    case LIB_WDT_STATE_SOC_ALIVE:
    {
        static uint32_t tick_current = 0;
        static uint32_t tick_prev = 0;

        if (handle->tick_get == NULL)
        {
            return;
        }

        tick_current = handle->tick_get();

        if ((tick_current - tick_prev) >= handle->tick_update)
        {
            tick_prev = tick_current;
            handle->cntr--;
            if (handle->cntr == 0)
            {
                handle->state = LIB_WDT_STATE_SOC_RESET;
            }
        }
        break;
    }

    case LIB_WDT_STATE_SOC_RESET:
    {
        static uint8_t is_started = 0;
        static uint32_t tick_start = 0;
        static uint32_t tick_current = 0;

        if (handle->tick_get == NULL)
        {
            return;
        }

        tick_current = handle->tick_get();

        if (is_started == 0)
        {
            is_started = 1;
            tick_start = tick_current;
            if (handle->soc_reset != NULL)
            {
                handle->soc_reset();
            }
        }

        if ((tick_current - tick_start) >= 500)
        {
            is_started = 0;
            handle->state = LIB_WDT_STATE_SOC_START;
        }
        break;
    }
    case LIB_WDT_STATE_SOC_START:
        if (handle->soc_start != NULL)
        {
            handle->soc_start();
        }

        handle->state = LIB_WDT_STATE_SOC_READY;
        break;

    default:
        break;
    }
}

void lib_wdt_soc_reload_set(lib_wdt_soc_handle_t *handle, uint32_t reload)
{
    handle->reload = reload;
}

void lib_wdt_soc_reload(lib_wdt_soc_handle_t *handle)
{
    handle->cntr = handle->reload;
}
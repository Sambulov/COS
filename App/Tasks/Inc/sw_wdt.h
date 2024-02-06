#ifndef SW_WDT_H_
#define SW_WDT_H_

#include "app.h"

/* @STATE WDT SOC */
typedef enum {
  LIB_WDT_STATE_SOC_READY, // WDT готов, но не запущен, чтобы запустить, необходимо вызвать lib_wdt_soc_state_set(&handle, LIB_WDT_STATE_SOC_ALIVE)
  LIB_WDT_STATE_SOC_ALIVE, // WDT запущен и счётчик декрементируется, необходимо вызывать команду lib_wdt_soc_reload, чтобы счётчик не достиг нуля, если счётчик достиг нуля, то переходим в состояние LIB_WDT_STATE_SOC_RESET
  LIB_WDT_STATE_SOC_RESET, // WDT достиг нуля, однократно вызывается функция soc_reset, чтобы сбросить процессор, после переходим в состояние LIB_WDT_STATE_SOC_START
  LIB_WDT_STATE_SOC_START  // WDT вызывает soc_start, чтобы процессор запустился, после этого переходим в состояние LIB_WDT_STATE_SOC_READY
} lib_wdt_soc_state_t;

typedef struct {
  lib_wdt_soc_state_t state; // состояние WDT

  uint32_t cntr;   // счётчик
  uint32_t reload; // переменная перезагрузки счётчика

  uint32_t tick_update;   // каждый tick_update счётчик будет декрементироваться
  uint32_t (*tick_get)(); // получить время

  void (*soc_reset)(); // сбросить питание процессора
  void (*soc_start)(); // запустить питание процессора
} lib_wdt_soc_handle_t;

/// @brief Установить состояние WDT
/// @param handle - указатель на объект wdt
/// @param state - ref @STATE WDT SOC
void lib_wdt_soc_state_set(lib_wdt_soc_handle_t *handle, lib_wdt_soc_state_t state);

/// @brief Получить состояние wdt
/// @param handle - указатель на объект wdt
/// @return ref @STATE WDT SOC
lib_wdt_soc_state_t lib_wdt_soc_state_get(lib_wdt_soc_handle_t *handle);

/// @brief Вызывать в основном цикле
/// @param handle - указатель на объект wdt
void lib_wdt_soc_update(lib_wdt_soc_handle_t *handle);

/// @brief Сброс счётчика wdt
/// @param handle - указатель на объект wdt
void lib_wdt_soc_reload(lib_wdt_soc_handle_t *handle);

/// @brief Установить значение перезагрузки, это значение будет установлено в cntr после вызова lib_wdt_soc_reload()
/// @param handle - указатель на объект wdt
/// @param reload - значение
void lib_wdt_soc_reload_set(lib_wdt_soc_handle_t *handle, uint32_t reload);

#endif // SW_WDT_H_
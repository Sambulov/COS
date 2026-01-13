# Выбор текущего проекта 
Создать файл `proj_select.txt`, указать в первой строке имя директории проекта, опционально второй строкой платформу, если проект поддерживает множество.

# Новый проект:
1) Создать дирекиторию нового проекта в `/Projects`
2) Добавить `CMakeLists.txt` по аналогии с демо проектом. Определить глобальные свойства:
```
#MCU - модель микроконтроллера 
#MCU_MANUFACTURER - производитель
#CORE - архитектура
#TOOLCHAIN_DIR - директория компилятора
#TOOLCHAIN_PREFIX - префикс для gcc, ld и т.д.

set_property(GLOBAL PROPERTY TOOLCHAIN_DIR "")
set_property(GLOBAL PROPERTY TOOLCHAIN_PREFIX "arm-none-eabi-")
set_property(GLOBAL PROPERTY CORE "ARM")

set_property(GLOBAL PROPERTY MCU "STM32F407VG")
set_property(GLOBAL PROPERTY MCU_MANUFACTURER "ST")
set_property(GLOBAL PROPERTY PROJECT_DEFINITIONS 
  "MBEDTLS_CONFIG_FILE=<mbedtls_config.h>"
  "LFS_CONFIG=lfs_config.h"
  "STM32F407xx"
  "STM32F407VG"
  "HARD_FLOAT"
)

#LIB_ENABLE_LWIP "ENABLE" для включения в проект библиотеки LwIP
#LIB_ENABLE_MBEDTLS "ENABLE" для включения в проект библиотеки MbedTLS
#LIB_ENABLE_LITTLE_FS "ENABLE" для включения в проект библиотеки LittleFS

set_property(GLOBAL PROPERTY LIB_ENABLE_LWIP "ENABLE")
set_property(GLOBAL PROPERTY LIB_ENABLE_MBEDTLS "ENABLE")
set_property(GLOBAL PROPERTY LIB_ENABLE_LITTLE_FS "ENABLE")

```
Данные свойства могут подменяться через условный оператор над свойством `BOARD`, если один проект расчитано запускать на разных платформах.
```
if(BOARD STREQUAL "DEMO_STM32F407")
...
elseif(BOARD STREQUAL "DEMO_STM32F429")
...
endif()
```

1) Создать файл графа инициализации `mig_<mcu>.c`. MIG файлы должны быть уникальны для платформы-проекта. Данный файл описывает модули используемые в проекте, их зависимости и конфигурации. Это полное описание системы от ядра контроллера и вектора его прерываний до высокоуровневых драйверов внешних связных устройств.
*можно адаптировать существующий из другого проекта.
2) Создать `mig_<mcu>.h`, тут экспортируются все дескрипторы модулей задействованных в роекте. По сути это требования бизнес-логики к платформе, какими ресурсами она должна обладать.
```
#ifndef MIG_H_
#define MIG_H_

extern hdl_time_counter_t mod_timer_ms; // само определение будет дано в mig_<mcu>.c
...

#endif // MIG_H_
```
3) Для удобства создать `app.h`, базовые зависимости бизнес-логики (по желанию). 
```
#ifndef APP_H_
#define APP_H_

#include "hdl_iface.h"
#include "mig_<mcu>.h"

void main();

#endif /* APP_H_ */
```
4) Создать файл точку входа проекта `app.c`. 
```
#include "app.h"

static uint8_t _app(coroutine_t *this, uint8_t cancel, void *context) {
  /* бизнес логика */
  return cancel;
}

void main() {
  DEBUG_INIT();
  static app_context_t app_context = {0};
  static coroutine_t app_cor;
  hdl_module_enable(&mod_app);
  coroutine_add(&app_cor, &_app, &app_context);
  while(cooperative_scheduler(HDL_FALSE) > 0);
}
```
`hdl_enable(&mod_app);` - пример, абстрактный модуль инициализация всего окружения. `cooperative_scheduler(HDL_FALSE)` - работа кооперативного планировщика.

1) Привязка дескрипторов модулей к их определениям в `mig_<mcu>.c`
```

...
const hdl_systick_counter_t mod_systick_counter = {
  .iface = &hdl_systick_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_systick_counter_config_t,
    .phy = (uint32_t)SysTick,
    .period = HDL_SYSTICK_COUNTER_RELOAD,
    #if (HDL_SYSTICK_PRESCALER == 8)
      .clock_src = 0
    #else
      .clock_src = SysTick_CTRL_CLKSOURCE_Msk
    #endif
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_time_counter_t mod_systick_timer = {
  .iface = &hdl_time_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_systick_counter, &mod_irq_systick),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_TIME_COUNTER_VAR_SIZE),
};
...

extern const hdl_time_counter_t mod_timer_ms                       __attribute__ ((alias ("mod_systick_timer")));
```
В данном примере инженер может подразумевать использование системного таймера для счета милисекунд, на другой платформе это может быть соершенно другой модуль реализующий интерфейс счетчика времени. Так бизнес логика абстрагируется от аппаратной платформы.

## Регистры системы
Регистр в системе можно зарегистрировать с помощью метода `register_add(address, reg_access)` где 
`address` - желаемый адрес регистрации, 
`reg_access` - метод доступа к регистру, определяется провайдером данных.
Возвращает 0 если регистрация не удалась.
`reg_access` вызывается всякий раз при обращении к регистру и ему будут переданы аргументы: адрес доступа, возвращаемое значение и режим доступа. 
Метод должен вернуть 0 в случае недопустимой операции, например, недопустимое значение записи или недопустимый режим доступа.
Для регистров определен набор режимов доступа:
 - REG_ACCESS_WRITE = 4 (запись значения)
 - REG_ACCESS_BIT_SET = 5 (запись, установка бит по маске)
 - REG_ACCESS_BIT_RESET = 6 (запись, сброс бит по маске)
 - REG_ACCESS_BIT_TOGGLE = 7 (запись, инверсия бит по маске)
 - REG_ACCESS_READ = 8 (чтение значения)

Для режимов чтения и записей допустимы комбинации, порядок операций при этом определяется методом доступа.<br>
Определение и описание регистров см. `app_reg_map.h`

### Пример регистрации:
```
uint32_t some_value = 1234;

/* Регистр только для чтения */
uint8_t reg_access(uint16_t address, uint32_t *value, reg_access_mode_t mode) {
  if(mode & REG_ACCESS_WRITE) return HDL_FALSE;
  if(value) *value = some_value;
  return HDL_TRUE;
}

/* где-то в инициализации. REG_SOME_VALUE внести в app_reg_map.h */
  register_add(REG_SOME_VALUE, &reg_access);
/* ... */

```
### Пример обращения:
```
  /* ... */
  /* Чтение */
  uint32_t val;
  if(registers_read(REG_SOME_VALUE, &val)) {
    /* val - имеет актуальное значение */
  }
  else {
    /* ошибка доступа */
  }
  /* ... */
```
### Доступ по CAN
Для карты регистров определен обработчик для обращения по шине CAN.
Используется кадр с расширенным адресом `REG_CAN_MESSAGE_ID = 0x12345678`<br>
#### Формат кадра:
|Режим доступа|Адрес|Данные|
|-|-|-|
|16b|16b|32b|
*В ответном кадре в качестве режима доступа - 0

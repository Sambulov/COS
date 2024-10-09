# Кроссмикроконтроллерная кооперативная ОС

## Подготовка к работе
1) Скачать и установить [тулчейн](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain)
2) Скачать и установить систему сборки [Ninja](https://github.com/ninja-build/ninja/releases)
3) Скачать и установить [CMake](https://cmake.org/download/)
4) Скачать и установить NuOpenOCD, ищем все [тут](https://www.nuvoton.com/products/microcontrollers/arm-cortex-m4-mcus/m463-can-fd-usb-hs-series/m463ygcae)
5) Скачать и установить OpenOCD (TODO: разобраться с Gigadevice)
6) При использовании J-Link скачать и установить [J-Link Software](https://www.segger.com/downloads/jlink/)
7) Возможно понадобится добавить в PATH директории тулчейна, Ninja, NuOpenOCD ...

## Сборка проекта
```powershell
cd <project dir>
cmake.exe -D CMAKE_BUILD_TYPE=Debug -D BOARD=<name> -S '.' -B './build' -G Ninja
cd .\build\
cmake --build . --clean-first
```
В параметре `-D BOARD=` указывается имя проекта.

## Пример. Прошивка микроконтроллера Nuvoton M463KG
### Локально через OpenOCD:
```powershell
$ nuopenocd.exe -s "./MCU/ARM/Nuvoton/NUM463KG/Res" -f "./MCU/ARM/Nuvoton/NUM463KG/Res/tool.cfg" -f "./MCU/ARM/Nuvoton/NUM463KG/Res/mcu.cfg" -c "init" -c "halt" -c "flash write_image erase ./build/bmc.hex" -c "reset run"
```
Где tool.cfg - конфиг NuLink, 
mcu.cfg - конфиг контроллера
Все можно найти в пакете SDK под конкретную серию микроконтроллера.

### Удаленно через GDB + OpenOCD:
```powershell
$ arm-none-eabi-gdb.exe
(gdb) target remote 10.20.30.40:3333
(gdb) monitor reset halt
(gdb) monitor flash erase_address 0x00000000 0x00040000
(gdb) monitor flash info 0
(gdb) monitor flash protect 0 0 7 off
(gdb) file ./build/bmc.elf
(gdb) load
(gdb) monitor reset run
```
Команды `(gdb) monitor flash info 0` и `(gdb) monitor flash protect 0 0 7 off` можно пропустить. Первая необходима чтобы помотреть состояние защиты флеш памяти от записи, вторая снять защиту.

Аналогично для других поддерживаемых микроконтроллеров. Скрипты можно найти в директории контроллера, в `./Res`.

## Установка и настройка среды VS Code
1) Скачать и установить [VS Code](https://code.visualstudio.com/download)
2) Установить расширения C/C++, Cortex-Debug (у меня заработало с Venus's Cortex-Debug)

Готово. 

### Сборка
Для каждого проекта предоставляется один или несколько пресетов:

### Прошивка
В меню `Terminal->Run Task` доступны задания по прошивке различными инструментами по проектам.

### Отладка
В меню `Run and Debug` доступны варианты отладки различными инструментами:

### Порядок действий 
Собираем, шьем, запускаем отладку F5. Не все варианты отладки настроены на автоматическую перепрошивку.

### ***
Чтобы запитать Target через J-Link, выполнить команду в J-Link Commander
`J-Link power on`
Или чтобы программатор принял настройку по-умолчанию.
`J-Link power on perm`

(TODO: проверить все на Linux)

### Портирование библиотеки на новый контроллер:
1) Скачать драйверы от производителя 
   - CMSIS(для контроллера не ядра ARM) положить в исходники в директорию `/Sys`, заголовки в `Sys/Inc`
   - SPL/HAL положить в директорию `/Sys/Drivers` исходники и заголовки в `Src`, `Inc` соответственно
2) Найти linker script, переименовать `linkerscript.ld`, положить в директорию `Res`
3) Найти Svd файл, переименовать `mcu.svd`, положить в директорию `Res`
4) Поправить j-link скрипты (`erase.jlink` и `flash.jlink`), в файле поменять имя устройства. 
```
...
device GD32F103VG
...
```
*можно скопировать у портов других контроллеров

5) Скопировать содержимое `/MCU/ARM/PortTemplate` в `/MCU/ARM/<manufacturer>/<mcu>/Port`
6) Добавить в .vscode/launch.json конфигурацию для нового контроллера
```
   "configurations": [
	    {
			"name": "Debug JLink GD32F103VG",
			"type": "cortex-debug",
			"request": "launch",
			"cwd": "${workspaceFolder}",
			"executable": "./build/bmc.elf",
			"windows": {
				"serverpath": "JLinkGDBServerCL.exe",
			},
			"linux": {
				"serverpath": "/usr/bin/JLinkGDBServerCLExe",
				"gdbPath": "/usr/bin/gdb-multiarch"
			},
			"runToEntryPoint": "main",
			//"showDevDebugOutput": "raw",
			"servertype": "jlink",
			"interface": "swd",
			"device": "GD32F103VG",
			"svdFile": "./MCU/ARM/GD32F103VG/Res/mcu.svd",
			"preLaunchTask": "J-Link Flash GD32F103VG",
			//"liveWatch": {
			//	"enabled": true,
			//	"samplesPerSecond": 4
			//},
		},
		...
	]
```
7) Добавить в `/.vcode/tasks.json` 2 задания для прошивки и очистки контроллера
```
    "tasks": [
		{
			"type": "shell",
			"label": "J-Link Flash GD32F103VG",
			"linux": {
				"command": "JLinkExe -CommandFile ./MCU/ARM/GD32F10x/Res/flash.jlink",
			},
			"windows": {
				"command": "JLink.exe -CommandFile ./MCU/ARM/GD32F10x/Res/flash.jlink",
			},
			"detail": "Flash",
			"group": "build"
		},
		{
			"label": "J-Link Erase GD32F103VG",
			"type": "shell",
			"linux": {
				"command": "JLinkExe -CommandFile ./MCU/ARM/GD32F10x/Res/erase.jlink",
			},
			"windows": {
				"command": "JLink.exe -CommandFile ./MCU/ARM/GD32F10x/Res/erase.jlink",
			},
			"detail": "Erase",
			"group": "build"
		},
        ...
    ]
```
8)  Описать все необходимые портируемые интерфейсы. Точка входа для ARM располагается в файле `port_core_x.c` согласно выбранному ядру микроконтроллера в проекте. Портирование следует начинать с файла `port_core.c` в котором описываются обработчики для прерываний и специфическая для данного микроконтроллера часть инициализации ядра.
9)  Создать тестовый проект для данного микроконтроллера.


### Новый проект:
1) Создать задание на сборку и прошивку контроллера в `CMakePresets.json`
```
   "configurePresets": [
	    ...
        {
            "name": "DebugGD32F103",
            "displayName": "Debug GD32F103",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug",
                "BOARD": "USPD"  <---- имя платформы/ревизии проекта
            }
        },
        {
            "name": "ReleaseGD32F103",
            "displayName": "Release GD32F103",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release",
                "BOARD": "USPD"
            }
        },
		...
	]
```

2) Добавить в `CMakeLists.txt` определение для платформы
```
  ...
  elseif(BOARD STREQUAL "ATB_RK3568J_SMC_R1") <---- имя платформы/ревизии проекта
    set(VENDOR "Gigadevice")
    set(MCU "GD32E23X") <---- контроллер платформы проекта
    set(CORE "ARM")
    set(MCPU cortex-m23) <---- ядро контроллера проекта
    set(PROJECT "ATB_RK3568J_SMC") <---- имя проекта
    # set(SPECIFIC_BUSINESS_LOGIC_INC_PATH ...)
    # set(SPECIFIC_BUSINESS_LOGIC_SRC_PATH ...)
    add_compile_definitions("__MCU_HAL_HDR__=<gd32e23x.h>") <---- импортируемая библиотека контроллера
	add_compile_definitions(...) <---- дополнительные глобальные определения необходимые для сборки, не обязательно
  ...
```
3) Создать файл графа инициализации в директории `mig_<mcu>.c`. MIG файлы должны быть уникальны для платформы-проекта. Данный файл описывает модули используемые в проекте, их зависимости и конфигурации. Это полное описание системы от ядра контроллера и вектора его прерываний до высокоуровневых драйверов внешних связных устройств.
*можно адаптировать существующий из другого проекта.
4) Создать `./Inc/mig.h`, тут экспортируются все дескрипторы модулей задействованных в роекте
```
#ifndef MIG_H_
#define MIG_H_

extern hdl_time_counter_t mod_timer_ms; // само определение в mig_<mcu>.c
...

#endif // MIG_H_
```
5) Создать `./Inc/app.h`
```
#ifndef APP_H_
#define APP_H_

#include "hdl.h"
#include "mig.h"

#include "CodeLib.h"

void main();

#endif /* APP_H_ */
```
5) Создать файл точки входа проекта `./app.c`
```
#include "app.h"

void main() {
  // включаем необходимые модули
  hdl_enable(&mod_timer_ms.module);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  // инициализация модулей завершена
  while (1) {
    cooperative_scheduler(false);
	// логика приложения
  }
}
```

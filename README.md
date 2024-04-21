# ATB Smarc BMC

Прошивка для BMC модуля ATB-RK3568J-SMC на базе микроконтроллеров: 
- Nuvoton NUM463KG
- Gigadevice GD32E230

## Подготовка к работе
1) Скачать и установить [тулчейн](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain)
2) Скачать и установить систему сборки [Ninja](https://github.com/ninja-build/ninja/releases)
3) Скачать и установить [CMake](https://cmake.org/download/)
4) Скачать и установить NuOpenOCD, ищем все [тут](https://www.nuvoton.com/products/microcontrollers/arm-cortex-m4-mcus/m463-can-fd-usb-hs-series/m463ygcae)
5) Скачать и установить OpenOCD (TODO: разобраться с Gigadevice)
6) При использовании J-Link скачать и установить [J-Link Software](https://www.segger.com/downloads/jlink/)
7) Возможно понадобится добавить в PATH директории тулчейна, Ninja, NuOpenOCD ...

## Сборка проекта проекта
```powershell
cd <project dir>
cmake.exe -D CMAKE_BUILD_TYPE=Debug -D BOARD=ATB-RK3568J-SMC-R3 -S '.' -B './build' -G Ninja
cd .\build\
cmake --build . --clean-first
```
В параметре `-D BOARD=` указывается на базе какого контроллера собирать проект: 
1) Для SMARC R0 на базе STM32L071 - `ATB-RK3568J-SMC-R0`
2) Для SMARC R1 на базе GD32E230 - `ATB-RK3568J-SMC-R1`
3) Для SMARC R3 на базе NUM463 - `ATB-RK3568J-SMC-R3`

## Прошивка микроконтроллера Nuvoton M463KG
### Локально через OpenOCD:
```powershell
$ nuopenocd.exe -s "./NUM463KG/Res" -f "./NUM463KG/Res/tool.cfg" -f "./NUM463KG/Res/mcu.cfg" -c "init" -c "halt" -c "flash write_image erase ./build/bmc.hex" -c "reset run"
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

## Прошивка микроконтроллера Gigadevice GD32E230 и ST STM32L071

Аналогично. Скрипты можно найти в директории контроллера, в `./Res`.

## Установка и настройка среды VS Code
1) Скачать и установить [VS Code](https://code.visualstudio.com/download)
2) Установить расширения C/C++, Cortex-Debug (у меня заработало с Venus's Cortex-Debug)

Готово. 

### Сборка
Для сборки доступно 6 пресетов:
1) `Debug NUM463KG`
2) `Release NUM463KG`
3) `Debug GD32E230`
4) `Release GD32E230`
5) `Debug STM32L071`
6) `Release STM32L071`

### Прошивка
В меню `Terminal->Run Task` доступно 8 заданий:
1) `OOCD Flash NUM463KG` - прошивка NUM463KG через OpenOCD
2) `J-Link Flash NUM463KG` - прошивка NUM463KG через J-Link 
3) `J-Link Flash GD32E230` - прошивка GD32E230 через J-Link
4) `J-Link Flash STM32L071KB` - прошивка STM32L071 через J-Link
5) `OOCD Flash NUM463KG` - очистка NUM463KG через OpenOCD
6) `J-Link Erase NUM463KG` - очистка NUM463KG через J-Link
7) `J-Link Erase GD32E230` - очистка GD32E230 через J-Link
8) `J-Link Erase STM32L071KB` - очистка STM32L071 через J-Link

### Отладка
В меню `Run and Debug` доступно 4 варианта:
1) `Debug OOCD NUM463KG`
2) `Debug J-Link NUM463KG`
3) `Debug J-Link GD32E230`
4) `Debug J-Link STM32L071`

### Порядок действий 
Собираем, шьем, запускаем отладку F5. Не все варианты отладки настроены на автоматическую перепрошивку.

### З.Ы.
Чтобы запитать Target через J-Link, выполнить команду в J-Link Commander
`J-Link power on perm`

(TODO: проверить все на LInux)

### Портирование библиотеки на новый контроллер:
1) Скачать драйверы от производителя 
   - CMSIS(для контроллера не ядра ARM) положить в исходники в директорию `/Sys`, заголовки в `Sys/Inc`
   - SPL/HAL положить в директорию `/Sys/Drivers` исходники и заголовки в `Src`, `Inc` соответственно
2) Найти linker script, переименовать `linkerscript.ld`, положить в директорию `Res`
3) Найти Svd файл, переименовать `mcu.svd`, положить в директорию `Res`
4) Поправить j-link скрипты (`erase.jlink` и `flash.jlink`), поменять имя устройства. 
```
...
device GD32F103VG
...
```
*можно скопировать у портов других контроллеров

5) Скопировать содержимое `/MCU/ARM/PortTemplate` в `/MCU/ARM/<your mcu>/Port`
6) Создать в проекте задание на сборку и прошивку контроллера в `CMakePresets.json`
```
   "configurePresets": [
        {
            "name": "DebugGD32F103",
            "displayName": "Debug GD32F103",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug",
                "BOARD": "USPD"  <---- имя платформы проекта
            }
        },
		...
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
	]
    "buildPresets": [
        {
            "name": "DebugGD32F103",
            "description": "",
            "displayName": "J-Link Debug GD32F103",
            "configurePreset": "DebugGD32F103"
        },
		...
	]
```

7) Добавить в `CMakeLists.txt` определение для платформы
```
  ...
  elseif(BOARD STREQUAL "USPD") <---- имя платформы проекта
    set(MCU "GD32F103VG")       <---- контроллер платформы проекта
    set(CORE "ARM")
    set(MCPU cortex-m3)         <---- ядро контроллера проекта
    add_compile_definitions("__MCU_HAL_HDR__=<gd32f10x.h>") <---- заголовок импортируемых драйверов контроллера
	add_compile_definitions("GD32F10X_XD")                  <---- дополнительные глобальные определения необходимые для сборки, не обязательно
  ...
```
8) Добавить в .vscode/launch.json конфигурацию для нового контроллера
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
9) Добавить в /.vcode/tasks.json 2 задания
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
10) Описать все необходимые методы драйверов HDL. Точка входа располагается в файле `port_core.c`, с него следует начинать портирование. Портирование драйвера ядра ARM сводится к правильному описанию вектора прерываний (можно посмотреть в startup-файле контроллера), написанию драйвера nvic и exti, а также произвести специфические настройки типа задержки обращения во флеш, задержки прерываний и тп.
11) Для разных контроллеров могут понадобиться разные опции периферии, тогда соответствующая структура описывается в заголовочных файлах port_xxx.h, которые должны располагаться в директории `Port/Inc`. Затем данный заголовочный файл включается в заголовочный файл HDL драйвера.
12) Создать файл графа инициализации в директории `MIG`. Использовать определение платформы проекта (см. п.7). MIG файлы должны быть уникальны для платформы-проекта.
```
#if defined ( <your board> ) 
...
#endif
```
13) Создать файл для тестов под портируемый контроллер `Test/test_<your_mcu>.c`. Примерное содержимое: 
```
#include "app.h"
#include "CodeLib.h"

#if defined ( <your_mcu> )

void test() {
  /* init module here */
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  while (1) {
    cooperative_scheduler(false);
	/* test module */
  }
}

#endif
```
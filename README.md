# Кроссмикроконтроллерная кооперативная ОС
Основная суть проекта - абстрагировать бизнес логику от микроконтроллерной платформы. А так же создать систему разработки приложений на базе микроконтроллеров.
## Подготовка к работе
1) Скачать и установить [тулчейн](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain)
2) Скачать и установить систему сборки [Ninja](https://github.com/ninja-build/ninja/releases)
3) Скачать и установить [CMake](https://cmake.org/download/)
4) Скачать и установить [OpenOCD](https://github.com/xpack-dev-tools/openocd-xpack/releases)
5) Для контроллеров Nuvoton Скачать и установить NuOpenOCD, ищем все [тут](https://www.nuvoton.com/products/microcontrollers/arm-cortex-m4-mcus/m463-can-fd-usb-hs-series/m463ygcae)
6) При использовании J-Link скачать и установить [J-Link Software](https://www.segger.com/downloads/jlink/)
7) В Windows понадобится добавить в PATH директории Ninja, OpenOCD ...

## Сборка проекта
```powershell
cd <COS dir>
cmake.exe -D CMAKE_BUILD_TYPE=Debug -D PROJECT=<name> -D BOARD=<name> -S '.' -B './build' -G Ninja
cd .\build\
cmake --build . --clean-first
```
В параметре `-D PROJECT=` название директории проекта, `-D BOARD=` указывается имя платформы проекта. 
Один проект может быть нацелен на множество платформ. Эти параметры можно указать в `.\Projects\proj_select.txt`.

## Пример. Прошивка микроконтроллера Nuvoton M463KG
### Локально через NuOpenOCD:
```powershell
$ nuopenocd.exe -s "./HDL/McuPort/ARM/Nuvoton/NUM463KG/Res" -f "./HDL/McuPort/ARM/Nuvoton/NUM463KG/Res/tool.cfg" -f "./HDL/McuPort/ARM/Nuvoton/NUM463KG/Res/mcu.cfg" -c "init" -c "halt" -c "flash write_image erase ./build/bmc.hex" -c "reset run"
```
Где `tool.cfg` - конфиг NuLink, 
`mcu.cfg` - конфиг контроллера.
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
2) Разрешить установить расширения C/C++, CMake, Cortex-Debug... (см. `./.vscode/extensions.json`)

Готово. 

### Сборка в среде
Для сборки проекта неоходимо определить имя проекта `PROJECT` и имя платформы `BOARD`. Надо создать файл `.\Projects\proj_select.txt`, первая строка `PROJECT`, вторая `BOARD`. `BOARD` - опциональный, используется внутри скрипта CMake внутри проекта, для уточнения под какую платформу собирать.

### Прошивка
В меню `Terminal->Run Task` доступны задания по прошивке различными инструментами. (TODO: протестировать)

### Отладка
В меню `Run and Debug` (Ctrl+Shift+D) доступны варианты отладки различными инструментами.

#### Порядок действий:
Собираем проект, выбираем вариант отладки, запускаем F5.

### ***
Чтобы запитать Target через J-Link, выполнить команду в J-Link Commander
`J-Link power on`
Или чтобы программатор принял настройку по-умолчанию.
`J-Link power on perm`

(TODO: проверить все на Linux)

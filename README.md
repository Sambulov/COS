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
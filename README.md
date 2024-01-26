# ATB Smarc BMC

Прошивка для BMC модуля ATB-RK3568J-SMC на базе микроконтроллера Nuvoton M463KG

## Подготовка к работе
1) Скачать и установить тулчейн https://developer.arm.com/downloads/-/gnu-rm
2) Скачать и установить систему сборки Ninja https://github.com/ninja-build/ninja/releases
3) Скачать и установить CMake https://cmake.org/download/
4) Скачать и установить NuOpenOCD 
5) Возможно понадобится добавить в PATH директории тулчейна, Ninja, NuOpenOCD ...

## Сборка проекта проекта
```powershell
cd <project dir>
cmake.exe -D CMAKE_BUILD_TYPE=Debug -S '.' -B './build' -G Ninja
cd .\build\
cmake --build . --clean-first
```

## Прошивка микроконтроллера
### Локально через OpenOCD:
```powershell
$ nuopenocd.exe -s "./NUM463KG/Res" -f "./NUM463KG/Res/tool.cfg" -f "./NUM463KG/Res/mcu.cfg" -c "init" -c "halt" -c "flash write_image erase ./build/bmc.hex" -c "reset run"
```
Где tool.cfg - конфиг NuLink, 
mcu.cfg - конфиг контроллера

Все можно найти в пакете SDK под конкретную серию микроконтроллера.

### Удаленно через GDB:
```powershell
$ arm-none-eabi-gdb.exe
(gdb) target remote 10.20.30.40:3333
(gdb) monitor reset halt
(gdb) monitor flash erase_address 0x00000000 0x00040000
(gdb) monitor flash info 0
(gdb) monitor flash protect 0 0 7 off
(gdb) file C:/Projects/ATB/bmc-smarc-nuvoton/build/bmc.elf
(gdb) load
(gdb) monitor reset run
```
Команды 
```
(gdb) monitor flash info 0
(gdb) monitor flash protect 0 0 7 off
```

можно пропустить. Первая необходима чтобы помотреть состояние защиты флеш памяти от записи, вторая снять защиту

## Установка и настройка среды VS Code
1) Скачать и установить VS Code https://code.visualstudio.com/download
2) Установить расширения C/C++, Cortex-Debug (у меня заработало с Venus's Cortex-Debug)
Готово. В меню Terminal->Run Task доступно 2 задания -сборка пророекта и -прошивка отладочной платы.
Запуск отладки F5.

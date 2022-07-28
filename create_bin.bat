arm-none-eabi-objcopy -O ihex Debug/F401_SmartGarden.elf Debug/F401_SmartGarden.hex
srec_cat Debug/F401_SmartGarden.hex -Intel -fill 0xFF 0x08040000 0x08060000 -o filled.hex -Intel -line-length=44
srec_cat filled.hex -Intel -crop 0x08040000 0x0805FFFC -STM32 0x0805FFFC -o signed_app.hex -Intel
arm-none-eabi-objcopy --input-target=ihex --output-target=binary signed_app.hex application.bin
rem del filled.hex
rem del signed_app.hex

rem tar.exe -a -cf application.zip application.bin





arm-none-eabi-objcopy --input-target=ihex --output-target=binary  Debug/F401_SmartGarden.hex  Debug/F401_SmartGarden.bin

pause
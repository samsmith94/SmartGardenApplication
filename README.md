```
+-- STM32CubeIDE workspace
|   +-- SmartGardenApplication
|   +-- SmartGardenBootloader
|   +-- production.bat
```
### production.bat:
```Cancel changes
rem Create application.hex and zipped application.bin

arm-none-eabi-objcopy -O ihex F401_SmartGarden/Debug/F401_SmartGarden.elf ./application.hex
srec_cat ./application.hex -Intel -fill 0xFF 0x08040000 0x08060000 -o filled.hex -Intel -line-length=44
srec_cat filled.hex -Intel -crop 0x08040000 0x0805FFFC -STM32 0x0805FFFC -o signed_app.hex -Intel
arm-none-eabi-objcopy --input-target=ihex --output-target=binary signed_app.hex application.bin
del filled.hex
del signed_app.hex
tar.exe -a -cf application.zip application.bin
del application.bin

rem Create bootloader.hex
arm-none-eabi-objcopy -O ihex F401_SmartGarden_Bootloader/Debug/F401_SmartGarden_Bootloader.elf ./bootloader.hex

rem Merge application.hex and bootloader.hex
mergehex -m bootloader.hex application.hex -o production.bin

del bootloader.hex
del application.hex

rem Flash production.bin
st-flash.exe --reset write production.bin 0x08000000

del production.bin

rem TODO: how to make bootloader not run when production is flashed?
rem So we should jump to application start flash addr. (0x08040000)
pause
```

This batch script
* creates .hex file from application's .elf
* appends CRC at the end
* creates .hex file from bootloader's .elf
* merges the 2 hex, and creates a .bin as a result
* flashes the resulting .bin and resets the STM32 target (which means, STM32 will run IAP bootloader first, that will start application)
* creates application.zip, so that can be uploaded to fota server (e.g. with [boto3 python library](https://aws.amazon.com/sdk-for-python/)) [TODO]

TODO: we don't want to execute bootloader first, we want to jump to app directly...

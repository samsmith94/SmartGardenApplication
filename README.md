```
+-- STM32CubeIDE workspace
|   +-- SmartGardenApplication
|   +-- SmartGardenBootloader
|   +-- production.bat
```
### production.bat:
```
mkdir "prod"

rem Create application.hex and zipped application.bin
arm-none-eabi-objcopy -O ihex F401_SmartGarden/Debug/F401_SmartGarden.elf prod/application.hex
srec_cat prod/application.hex -Intel -fill 0xFF 0x08040000 0x08060000 -o prod/filled.hex -Intel -line-length=44
srec_cat prod/filled.hex -Intel -crop 0x08040000 0x0805FFFC -STM32 0x0805FFFC -o prod/signed_app.hex -Intel
arm-none-eabi-objcopy --input-target=ihex --output-target=binary prod/signed_app.hex prod/application.bin

tar.exe -a -cf prod/application.zip prod/application.bin

rem Create bootloader.hex
arm-none-eabi-objcopy -O ihex F401_SmartGarden_Bootloader/Debug/F401_SmartGarden_Bootloader.elf prod/bootloader.hex

rem Merge application.hex and bootloader.hex
mergehex -m prod/bootloader.hex prod/signed_app.hex -o prod/production.bin


rem st-flash erase


rem Flash production.bin
st-flash.exe --reset write prod/production.bin 0x08000000


cd prod
rem del filled.hex
rem del signed_app.hex
rem del application.hex
rem del application.bin
rem del bootloader.hex
rem del production.bin
rem del application.zip

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

[st-flash](https://github.com/stlink-org/stlink/releases/tag/v1.7.0) needs to be downloaded.




## This is the application btw: https://github.com/samsmith94/SmartGardenBootloader


### create_OTA_bin.bat:
```
rem Create application.hex and zipped application.bin
arm-none-eabi-objcopy -O ihex F401_SmartGarden/Debug/F401_SmartGarden.elf prod/application.hex
srec_cat prod/application.hex -Intel -fill 0xFF 0x08040000 0x08060000 -o prod/filled.hex -Intel -line-length=44
srec_cat prod/filled.hex -Intel -crop 0x08040000 0x0805FFFC -STM32 0x0805FFFC -o prod/signed_app.hex -Intel
arm-none-eabi-objcopy --input-target=ihex --output-target=binary prod/signed_app.hex application.bin

pause
```

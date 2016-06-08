# SmartWatch
This project aims to create an open-source smartwatch with the following functionality:
- 1.3" OLED display (bright enough to work in outdoor conditions)
- BLE module (HM-10), pin-to-pin compatibility with well-known HC-06
- small vibration motor (SMS/Call notifications and so on)
- STM32F103C8T6 microcontroller.
- Big external SPI flash memory (M25P40), SW OTA update and a lot of images can be stored there.
- BMC150 accelerometer/compass (can recognize double tap on the screen).
- BMP180 barometer/thermometer (just because there is a plenty of space).
- 320mAh battery allowing more than 1 week standby time (with BLE active).
- PCB size: 37*25, complete assembled height should be about 10mm.
- 3D models for all parts are in my [KiCAD_libs](https://github.com/Sl-Alex/KiCAD_libs) repository, case will be available a later, so the whole project can be easily reproduced.

# Hardware
Both schematic and PCB were created with KiCAD. KiCAD project is located in the [PCB](./PCB) subfolder. Project uses components from my [KiCAD_libs](https://github.com/Sl-Alex/KiCAD_libs) repository.
Status: Finished. PCB is assembled, all HW features are tested and work as expected. Minor updates (component values and comments) will follow.

# Software
Project created in [EmBitz](http://www.emblocks.org/web/)
Status: Basic HW classes implementation (GPIO, SW/HW SPI and so on). SPL will be completely thrown away.

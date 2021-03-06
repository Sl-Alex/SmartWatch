# General description
This project aims to create an open-source smartwatch with the following functionality:
- 1.3" OLED display (bright enough to work in outdoor conditions)
- BLE module (HM-10), pin-to-pin compatibility with well-known HC-06
- small vibration motor (SMS/Call notifications and so on)
- STM32F103CBT6 microcontroller.
- Big external SPI flash memory (M25P40), SW OTA update and a lot of images can be stored there.
- BMC150 accelerometer/compass (can recognize double tap on the screen).
- BMP180 barometer/thermometer (just because there is a plenty of space).
- 320mAh battery allowing up to 3 weeks standby time (with BLE active).
- PCB size: 37*25, complete assembled height should be about 10mm.
- 3D models for all parts are in my [KiCAD_libs](https://github.com/Sl-Alex/KiCAD_libs) repository, case will be available a bit later, so the whole project can be easily reproduced.

# Hardware
Both schematic and PCB were created with KiCAD. KiCAD project is located in the PCB subfolder. Project uses components from my [KiCAD_libs](https://github.com/Sl-Alex/KiCAD_libs) repository.<br />
**Status**: Finished. PCB is assembled, all HW features are tested and work as expected. Standby time seems to be about 3 weeks (with BLE active).

# Software
Project is created in [EmBitz](https://www.embitz.org/)<br />
**Status**: Can be used on a daily basis with some basic functionality (automatic date/time synchronization, SMS reception). Feel free to submit patches, create or discuss issues [here](https://github.com/Sl-Alex/SmartWatch/issues).<br />

# Licensing
The whole project is available under GPL v3 license (see [LICENSE](LICENSE) file), except of files, which clearly mention other license type in the file header.

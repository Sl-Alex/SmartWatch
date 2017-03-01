# Prerequisites
In order to make it work you should have:

1. ST-Link or other SWD programmer/debugger
2. Android Studio IDE
3. EmBitz IDE
4. Soldering equipment (you should be able to solder LGA-14 package, the rest is much easier)
5. USB-UART adapter with 3.3V IO levels
6. Qt Creator and development environment

SWD programmer/debugger will be required for FW flashing.

Android Studio will be required for Android SW compilation.

EmBitz will be required for FW compilation/debugging.

USB-UART adapter will be required for initial HM-10 configuration.

# How to make it work

You should do the following steps:

1. Resources compilation
2. HM-10 configuration
3. Device assembling
4. External SPI memory flashing
5. FW flashing
6. Android application building

Future versions will be much simpler, steps 3, 5 and 6 will be removed.

## 1. Resources compilation

All resources are in the [root]/Resources/Flash folder.
Open [root]Tools/flash_packer/flash_packer.pro with Qt Creator. Run it.
It will take all resources and pack them in a single update.bin file in resources folder.

## 2. HM-10 configuration

The only strict requirement is a UART bitrate, which should be set to 115200.
See HM-10 documentation for details.

## 3. Device assembling

The hardest part of the assembling is the installation of the accelerometer, which comes in a LGA-14 case.
It is recommended to assemble everything in the following order:

1. Solder everything except of: BMP180, HM-10, buttons, display, battery.
2. Connect ST-Link to the debug pads on the bottom of the board.
3. Compile and flash the firmware (EmBitz project is in [root]/FW/ folder).
4. Run the FW in debug mode and ensure that BMC150 issues acknowledges on both I<sup>2</sup>C addresses.
5. Solder the rest.
6. Connect 5V power supply to the VIN pad on the bottom of the PCB
7. Check that battery charge works fine.

## 4. External SPI memory flashing

If you have an SPI flash programmer you can just flash update.bin to the external memory
(STM32 must be in reset state or without any FW).
If you don't have it do the following:

1. Open sm_hw_storage.cpp, find the line containing #include "update.inc", uncomment it.
2. Uncomment lines with "eraseBulk()" and "writeBuffer(update_bin, 0, update_bin_len)"
3. Convert update.bin to update.inc using xxd: "xxd -i update.bin update.inc"
4. Put update.inc in [root]/FW/inc folder
5. Build and run this FW.
6. Wait a bit while external flash is being flashed
7. Comment lines from step 1 and 2.

## 5. FW flashing

If external memory is already flashed and update-related lines are commented then just build and run the FW in "Release" mode.

## 6. Android application building

Open Android project in the [root]SW/SmCenter folder.
Open SW/SmCenter/app/src/main/java/ua/com/slalex/smcenter/services/SmWatchService.java,
find a line containing "setBtAddress" and change the address to the address of your HM-10 module.
Now everything is ready, just build and run the SW.

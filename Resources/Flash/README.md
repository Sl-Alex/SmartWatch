# Firmware resources
This folder contains all resources, which can be used by the firmware.
all resources should have proper naming, like NNN*.(bin|pbm|txt)
For example, 000_version.bin or 001_update.bin
3-digit number is mandatory and should match to the numbers defined in SmHwStorageIndices enumeration
(sm_hw_storage.h). Numbering starts from 000, and it is not allowed to skip some numbers.
When all resources are created, you can run flash_packer and it will produce update.bin file in this folder.
This file can be written directly to external flash using BLE connection or manually via any SPI flash adapter.
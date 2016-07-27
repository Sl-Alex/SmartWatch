# Tools for image production
## Flash Packer
This tool packs resource files in a single binary file, which can be directly downloaded into the external SPI flash.
Here is an example set of files:
- 000.txt (FW update version, ASCII text representing FW revision, for example "150")
- 001.bin (FW update itself)
- 002_FONT_SMALL.txt (all symbols for the FONT_SMALL in UCS-2LE charset)
- 002_FONT_SMALL.pbm (single image containing all symbols from FONT_SMALL in the same order like in *.txt file). PBM P4 format is used. Image has white background and black pixels. First column contains one black pixel at the font baseline. First row contain black pixels between symbols.
- 003.pbm (Image in PBM P4 format)
All necessary attributes will be calculated automatically.

#include "pbm_image.h"

#include <iostream>

bool PbmImage::loadFromFile(std::ifstream * fs)
{
    if (pData)
        delete[] pData;

    mWidth = 0;
    mHeight = 0;
    mSize = 0;

    char line[255];

    // Read first line with the format ID
    fs->getline(line, 255);

    // Should be "P4", ignore the rest
    line[2] = 0;

    // Only "P4" (portable bitmap) will be parsed
    const std::string valid_header = "P4";

    if (valid_header.compare(0,2,line,2) != 0)
    {
        std::cout << "Invalid header. Expecting \"P4\"" << std::endl;
        return false;
    }

    // Read line with size, skip comments
    do {
        fs->getline(line,255);
    } while (line[0] == '#');

    std::string ln = line;

    size_t pos = ln.find(' ');
    if (pos == std::string::npos)
    {
        std::cout << "Image size is expected, found the following: \"" << \
             ln << "\"" << std::endl;
        return false;
    }

    std::string ws = ln.substr(0,pos);
    std::string hs = ln.substr(pos + 1, ln.length()-1);

    try
    {
        mWidth = std::stoi(ws);
        mHeight = std::stoi(hs);
    }
    catch(...)
    {
        mWidth = 0;
        mHeight = 0;
        std::cout << "Cannot parse the following: \"" << ln << "\"" << std::endl;
        std::cout << "Seems to contain invalid width and height" << std::endl;
        return false;
    }

    // Reserve enough place for binary data
    uint32_t mSize = ((mWidth+7)/8) * mHeight;
    pData = new char[mSize];
    // Read data into the buffer
    fs->read(pData,mSize);

    pos = fs->tellg();
    fs->seekg(0, std::ios::end);
    size_t last = fs->tellg();
    if (last != pos)
    {
        mWidth = 0;
        mHeight = 0;
        mSize = 0;
        delete[] pData;
        std::cout << "last = " << last << ", pos = " << pos << std::endl;
    }

    return true;
}

uint8_t PbmImage::getPixel(uint32_t x, uint32_t y)
{
    return ((pData[y * ((mWidth + 7)/8) + (x / 8)] >> \
                      (7 - (x % 8))) & 0x01);
}

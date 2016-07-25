#include "sm_image.h"
#include "sm_hw_storage.h"

#define abs(a) ((a)>0?(a):(-(a)))

#define swap_int(a, b) { int t = a; a = b; b = t; }

bool SmImage::init(int imageIndex)
{
    // Get element info by index
    SmHwStorageElementInfo info;
    SmHwStorage::getInstance()->getElementInfo(imageIndex, &info);

    // Initialize image by offset
    return initOffset(info.offset);
}

bool SmImage::initOffset(uint32_t offset)
{
    // Read image header
    SmHwStorage::getInstance()->readData(offset, (uint8_t *)&header, sizeof(header));
    // Init image memory according to the header information
    SmTexture::init(header.width, header.height);
    // Read image data
    SmHwStorage::getInstance()->readData(offset + sizeof(header), (uint8_t *)getPData(), header.size);

    return true;
}

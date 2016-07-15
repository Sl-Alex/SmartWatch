#include "sm_image.h"
#include "sm_hw_storage.h"

#define abs(a) ((a)>0?(a):(-(a)))

#define swap_int(a, b) { int t = a; a = b; b = t; }

/// @TODO verify
bool SmImage::init(int imageIndex)
{

    ImageHeader header;

    if (SmHwStorage::getInstance()->getElementSize(imageIndex) == 0)
        return false;

    // Read image header
    SmHwStorage::getInstance()->readElement(imageIndex, 0, (uint8_t *)&header, sizeof(header));
    // Init image according to the header information
    SmTexture::init(header.width, header.height);
    // Read image data
    SmHwStorage::getInstance()->readElement(imageIndex, sizeof(header), (uint8_t *)getPData(), header.size);

    return true;
}

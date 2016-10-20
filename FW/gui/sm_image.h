#ifndef SM_IMAGE_H
#define SM_IMAGE_H

#include <cstring>
#include "sm_canvas.h"

/** @addtogroup EXTERNAL_FLASH
 * @{
 *
 * @par Image element
 *
 * Image element holds image in a native image format of the display (see @ref SmTexture for more information).
 * It has a header with image width, height and size and actual image data.
 * So, image layout in the @ref SmHwStorage is the following:
 *
 * Offset       | Parameter | Size  | Description
 * ------------ | --------- | ----- | ------
 * 0x0000       | width     | 32    | Image width in pixels
 * 0x0004       | height    | 32    | Image height in pixels
 * 0x0008       | size      | 32    | Image size in bytes
 * 0x000C       | data      | ...   | Image data
 *
 * @}
 *
 */

/// @brief Image class
class SmImage: public SmCanvas
{
public:

    /// @brief Init image by index (see @ref EXTERNAL_FLASH)
    virtual bool init(int imageIndex);

    /// @brief Init image by offset in the @ref EXTERNAL_FLASH
    virtual void initOffset(uint32_t offset);

    /// @brief Init only header, not the data;
    virtual void initHeaderOffset(uint32_t offset);

private:
    struct ImageHeader {
        int width;
        int height;
        int size;
    };

    ImageHeader header;
};

#endif // SM_IMAGE_H

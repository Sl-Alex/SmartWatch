#include "smartstorage.h"
#include <cstring>

extern const char image_data_font[1536];
extern const char menu[13][128];

// Number of elements in a storage, -1 means failure
int SmartStorage::nElements;
// Pointer to the array of headers
SmartStorage::StorageHeader * SmartStorage::pHeader = 0;
// CRC of all above
unsigned int SmartStorage::headerCrc;

int SmartStorage::init()
{
    // Cleanup
    nElements = -1;
    if (pHeader) delete[] pHeader;

    //readFlash(address = 0,size = sizeof(nElements),&nElements);
    /// @todo temp
    nElements = 16;

    if (nElements > 100) { nElements = -1; return nElements; }

    pHeader = new StorageHeader[nElements];

    //readFlash(address = 1,size = nElements * sizeof(StorageHeader), pHeader);
    /// @todo temp
    #if (BPP == 8)
    pHeader[0].address = 100;
    pHeader[0].size = 19*19 + 12;//*sizeof(int);
    pHeader[1].address = 200;
    pHeader[1].size = 5*5 + 12;
    pHeader[2].address = 300;
    pHeader[2].size = 6*8*255*sizeof(char) + 12;
    #elif (BPP == 1)
    pHeader[0].address = 100;
    pHeader[0].size = 19*3 + 12;//*sizeof(int);
    pHeader[1].address = 200;
    pHeader[1].size = 5*1 + 12;
    pHeader[2].address = 300;
    pHeader[2].size = 6*1*256*sizeof(char) + 12;

    pHeader[3].address = 400;
    pHeader[3].size = 32*4*sizeof(char) + 12;
    pHeader[4].address = 400;
    pHeader[4].size = 32*4*sizeof(char) + 12;
    pHeader[5].address = 400;
    pHeader[5].size = 32*4*sizeof(char) + 12;
    pHeader[6].address = 400;
    pHeader[6].size = 32*4*sizeof(char) + 12;
    pHeader[7].address = 400;
    pHeader[7].size = 32*4*sizeof(char) + 12;
    pHeader[8].address = 400;
    pHeader[8].size = 32*4*sizeof(char) + 12;
    pHeader[9].address = 400;
    pHeader[9].size = 32*4*sizeof(char) + 12;
    pHeader[10].address = 400;
    pHeader[10].size = 32*4*sizeof(char) + 12;
    pHeader[11].address = 400;
    pHeader[11].size = 32*4*sizeof(char) + 12;
    pHeader[12].address = 400;
    pHeader[12].size = 32*4*sizeof(char) + 12;
    pHeader[13].address = 400;
    pHeader[13].size = 32*4*sizeof(char) + 12;
    pHeader[14].address = 400;
    pHeader[14].size = 32*4*sizeof(char) + 12;
    pHeader[15].address = 400;
    pHeader[15].size = 32*4*sizeof(char) + 12;
    #endif
    if (!checkCrc())
    {
        nElements = -1;
        return nElements;
    }

    return nElements;
}

bool SmartStorage::checkCrc(void)
{
    return true;
}

int SmartStorage::getSize(int idx)
{
    if (!isValid()) return -1;
    if (idx >= nElements) return -1;

    return pHeader[idx].size;
}

int SmartStorage::loadData(int idx, int offset, int cnt, char * pData)
{
    int i;
    if (!pData) return -1;
    if (!isValid()) return -1;
    if (idx >= nElements) return -1;
    if ((offset + cnt) > pHeader[idx].size) return -1;

    //readFlash(pHeaders[idx]->address, pHeaders[idx]->size, pData);
    /// @todo implement
    char data = 0;
    if (offset == 0){
        if (idx == 0)
        {
            ((int *)pData)[0] = 19;
            ((int *)pData)[1] = 19;
            #if (BPP == 8)
            ((int *)pData)[2] = 19*19;
            #else
            ((int *)pData)[2] = 19*3;
            #endif
        }
        else if (idx == 1)
        {
            ((int *)pData)[0] = 5;
            ((int *)pData)[1] = 5;
            #if (BPP == 8)
            ((int *)pData)[2] = 5*5;
            #else
            ((int *)pData)[2] = 5*1;
            #endif
        }
        else if (idx == 2)
        {
            ((int *)pData)[0] = 6UL*256UL;
            ((int *)pData)[1] = 8UL;
            #if (BPP == 8)
            ((int *)pData)[2] = 6*256*8UL;
            #else
            ((int *)pData)[2] = 6*256*1UL;
            #endif
        }
        else if ((idx >= 3) && (idx <= 16))
        {
            ((int *)pData)[0] = 32;
            ((int *)pData)[1] = 32;
            #if (BPP == 8)
            ((int *)pData)[2] = 32*32;
            #else
            ((int *)pData)[2] = 32*4;
            #endif
        }
    }
    if (offset == 0) return 3*sizeof(int);
    if (idx == 0)
        for (int i = 0; i < cnt; i++)
        {
            pData[i] = data;
            #if (BPP == 8)
            data+=64;
            #else
            data = 1 - data;
            #endif
        }
    else if (idx == 1)
        for (int i = 0; i < cnt; i++)
        {
            pData[i] = 255;
            #if (BPP == 8)
            data = 255 - data;
            #endif
        }
    else if (idx == 2)
    {
        #if (BPP == 8)
        pData[0] = 255;
        pData[1] = 255;
        pData[2] = 255;
        pData[3] = 255;
        pData[4] = 255;
        pData[5] = 255;
        pData[255*6*7+0] = 255;
        pData[255*6*7+1] = 128;
        pData[255*6*7+2] = 128;
        pData[255*6*7+3] = 128;
        pData[255*6*7+4] = 128;
        pData[255*6*7+5] = 255;

        pData[6+0] = 255;
        pData[6+1] = 128;
        pData[6+2] = 255;
        pData[6+3] = 255;
        pData[6+4] = 128;
        pData[6+5] = 255;
        pData[255*6*7+6+0] = 255;
        pData[255*6*7+6+1] = 255;
        pData[255*6*7+6+2] = 128;
        pData[255*6*7+6+3] = 128;
        pData[255*6*7+6+4] = 255;
        pData[255*6*7+6+5] = 255;
        #else
        for (i =0;i< 256*6; i++)
        {

            *pData = image_data_font[i];
            pData++;
        }
        /*pData[0]=0x80;
        pData[1]=0x70;
        pData[2]=0x0F;
        pData[3]=0x01;
        pData[4]=0xFF;
        pData[5]=0x81;

        pData[6+0]=0x7E;
        pData[6+1]=0x81;
        pData[6+2]=0xBD;
        pData[6+3]=0xBD;
        pData[6+4]=0x81;
        pData[6+5]=0x7E;*/
        #endif
    }
    else if ((idx >= 3) && (idx <= 16))
    {
        #if (BPP == 1)
        for (i =0;i< 32*4; i++)
        {

            *pData = menu[idx - 3][i];
            pData++;
        }
        #endif
    }
    return pHeader[idx].size;
}


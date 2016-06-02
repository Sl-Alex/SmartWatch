#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>

using namespace std;

const int MAX_FILES = 100;

struct FileInfo{
    string name;
    enum FileType{FT_PBM, FT_BIN} type;
    uint32_t size;
};

static const unsigned char crc8table[256] = {
    0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97,
    0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E,
    0x43, 0x72, 0x21, 0x10, 0x87, 0xB6, 0xE5, 0xD4,
    0xFA, 0xCB, 0x98, 0xA9, 0x3E, 0x0F, 0x5C, 0x6D,
    0x86, 0xB7, 0xE4, 0xD5, 0x42, 0x73, 0x20, 0x11,
    0x3F, 0x0E, 0x5D, 0x6C, 0xFB, 0xCA, 0x99, 0xA8,
    0xC5, 0xF4, 0xA7, 0x96, 0x01, 0x30, 0x63, 0x52,
    0x7C, 0x4D, 0x1E, 0x2F, 0xB8, 0x89, 0xDA, 0xEB,
    0x3D, 0x0C, 0x5F, 0x6E, 0xF9, 0xC8, 0x9B, 0xAA,
    0x84, 0xB5, 0xE6, 0xD7, 0x40, 0x71, 0x22, 0x13,
    0x7E, 0x4F, 0x1C, 0x2D, 0xBA, 0x8B, 0xD8, 0xE9,
    0xC7, 0xF6, 0xA5, 0x94, 0x03, 0x32, 0x61, 0x50,
    0xBB, 0x8A, 0xD9, 0xE8, 0x7F, 0x4E, 0x1D, 0x2C,
    0x02, 0x33, 0x60, 0x51, 0xC6, 0xF7, 0xA4, 0x95,
    0xF8, 0xC9, 0x9A, 0xAB, 0x3C, 0x0D, 0x5E, 0x6F,
    0x41, 0x70, 0x23, 0x12, 0x85, 0xB4, 0xE7, 0xD6,
    0x7A, 0x4B, 0x18, 0x29, 0xBE, 0x8F, 0xDC, 0xED,
    0xC3, 0xF2, 0xA1, 0x90, 0x07, 0x36, 0x65, 0x54,
    0x39, 0x08, 0x5B, 0x6A, 0xFD, 0xCC, 0x9F, 0xAE,
    0x80, 0xB1, 0xE2, 0xD3, 0x44, 0x75, 0x26, 0x17,
    0xFC, 0xCD, 0x9E, 0xAF, 0x38, 0x09, 0x5A, 0x6B,
    0x45, 0x74, 0x27, 0x16, 0x81, 0xB0, 0xE3, 0xD2,
    0xBF, 0x8E, 0xDD, 0xEC, 0x7B, 0x4A, 0x19, 0x28,
    0x06, 0x37, 0x64, 0x55, 0xC2, 0xF3, 0xA0, 0x91,
    0x47, 0x76, 0x25, 0x14, 0x83, 0xB2, 0xE1, 0xD0,
    0xFE, 0xCF, 0x9C, 0xAD, 0x3A, 0x0B, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57, 0xC0, 0xF1, 0xA2, 0x93,
    0xBD, 0x8C, 0xDF, 0xEE, 0x79, 0x48, 0x1B, 0x2A,
    0xC1, 0xF0, 0xA3, 0x92, 0x05, 0x34, 0x67, 0x56,
    0x78, 0x49, 0x1A, 0x2B, 0xBC, 0x8D, 0xDE, 0xEF,
    0x82, 0xB3, 0xE0, 0xD1, 0x46, 0x77, 0x24, 0x15,
    0x3B, 0x0A, 0x59, 0x68, 0xFF, 0xCE, 0x9D, 0xAC
};

unsigned char calcCrc(unsigned char * pData, unsigned int count, unsigned char crc = 0xFF)
{
    while (count--)
        crc = crc8table[crc ^ *pData++];

    return crc;
}

bool pbm_file_get_size(ifstream * file, unsigned int * width, unsigned int * height)
{
    char line[255];

    // Read first line with the format ID
    file->getline(line, 255);
    line[2] = 0;

    // Only "P4" (portable bitmap) will be parsed
    const string valid_header = "P4";
    if (valid_header.compare(0,2,line,2) != 0)
    {
        cout << "Invalid header. Expecting \"P4\"" << endl;
        return false;
    }

    // Read line with size, skip comments
    do {
        file->getline(line,255);
    } while (line[0] == '#');

    string ln = line;

    size_t pos = ln.find(' ');
    if (pos == std::string::npos)
    {
        cout << "Image size is expected, found the following: \"" << \
             ln << "\"" << endl;
        return false;
    }

    string ws = ln.substr(0,pos);
    string hs = ln.substr(pos + 1, ln.length()-1);

    try
    {
        *width = std::stoi(ws);
        *height = std::stoi(hs);
    }
    catch(...)
    {
        cout << "Cannot parse the following: \"" << ln << "\"" << endl;
        cout << "Seems to contain invalid width and height" << endl;
        return false;
    }

    return true;
}

void pbm_file_convert_data(char * in, char * out)
{

}

int main()
{
    vector<FileInfo> filelist;

    streampos size;
    char * memblock;

    filelist.clear();

    while (true)
    {
        FileInfo info;

        string base_name = "";

        if (filelist.size() < 10)
            base_name.append("0");
        base_name.append(to_string(filelist.size()));

        // Search for "*.pbm" files
        string pbm_name = base_name;
        pbm_name.append(".pbm");

        // Search for "*.bin" files
        string bin_name = base_name;
        bin_name.append(".bin");

        ifstream pbm_file(pbm_name, ios::in|ios::binary);
        if (pbm_file.is_open())
        {
            unsigned int width, height;
            if (!pbm_file_get_size(&pbm_file, &width, &height))
            {
                cout << pbm_name << " is invalid" << endl;
                return 0;
            }

            // Calculate the size of the packed file
#if (BPP == 1)
#ifdef PACK_VERT
            unsigned int size = width*((height + 7)/8);
#endif
#endif
#if (BPP == 8)
            unsigned int size = width*height;
#endif
            pbm_file.close();
            info.size = size;
            info.name = pbm_name;
            info.type = FileInfo::FT_PBM;
            filelist.push_back(info);
            continue;
        }

        ifstream bin_file(bin_name, ios::in|ios::binary|ios::ate);
        if (bin_file.is_open())
        {
            info.size = bin_file.tellg();
            info.name = bin_name;
            info.type = FileInfo::FT_BIN;
            bin_file.close();
            filelist.push_back(info);
            continue;
        }

        // File not found
        break;
    }

    // Check if there are some files to pack
    if (filelist.size() == 0)
    {
        cout << "Nothing to pack" << endl;
        return 0;
    }

    cout << filelist.size() << " file(s) found. Packing..." << endl;

    ofstream outfile("out.bin", ios::out|ios::binary|ios::trunc);
    //         CRC CNT  HEADERS(OFF+SIZE)
    char header[1 + 1 + 2*sizeof(int)*filelist.size()];
    memset(header,0,sizeof(header));
    header[1] = filelist.size();
    unsigned int offset  = sizeof(header);
    for (auto it = filelist.begin(); it != filelist.end(); it++)
    {
        if (it->type == FileInfo::FT_PBM)
        {
            *((unsigned int *)&(header[2 + 2*sizeof(int)*(it - filelist.begin())])) = it->size + 3*sizeof(int);
            *((unsigned int *)&(header[2 + 2*sizeof(int)*(it - filelist.begin()) + sizeof(int)])) = offset;
            offset += it->size + 3*sizeof(int);
        }
        if (it->type == FileInfo::FT_BIN)
        {
            *((unsigned int *)&(header[2 + 2*sizeof(int)*(it - filelist.begin())])) = it->size;
            *((unsigned int *)&(header[2 + 2*sizeof(int)*(it - filelist.begin()) + sizeof(int)])) = offset;
            offset += it->size;
        }
    }
    header[0] = calcCrc((unsigned char *)&header[1],sizeof(header) - 1);
    outfile.write(header,sizeof(header));

    for (auto it = filelist.begin(); it != filelist.end(); it++)
    {
        cout << "#" << it - filelist.begin() << " :" << it->name << " ";
        if (it->type == FileInfo::FT_PBM)
        {
            cout << "(portable bitmap)" << endl;
            unsigned int width, height;

            ifstream file(it->name, ios::in|ios::binary);
            if (!file.is_open())
            {
                cout << "Can't open file" << endl;
                return 0;
            }
            if (!pbm_file_get_size(&file,&width,&height))
            {
                file.close();
                cout << "Can't read dimensions" << endl;
                return 0;
            }
            cout << "    width = " << width << "; height = " << height \
                 << "; packed size (bytes) = " << it->size << endl;
            char original_data[((width+7)/8) * height];
            char converted_data[it->size];
            memset(converted_data,0,it->size);
            file.read(original_data,sizeof(original_data));
            file.close();
            char bitmap[width][height];
            // Unpack bitmap
            for (unsigned int x = 0; x < width; x++)
            {
                for (unsigned int y = 0; y < height; y++)
                {
                    bitmap[x][y] = (((original_data[y * ((width + 7)/8) + (x / 8)] >> \
                                     (7 - (x % 8))) & 0x01) == 0)? 0 : 255;
                }
            }
            // Pack bitmap
            for (unsigned int x = 0; x < width; x++)
            {
                for (unsigned int y = 0; y < height; y++)
                {
#if (BPP == 1)
#ifdef PACK_VERT
                    if (bitmap[x][y])
                        converted_data[width*(y/8) + x] |= 0x01 << y % 8;
#endif
#endif
                }
            }

            pbm_file_convert_data(original_data, converted_data);
            int hdr[3];
            hdr[0] = width;
            hdr[1] = height;
            hdr[2] = sizeof(converted_data);
            outfile.write((char *)hdr,sizeof(hdr));
            outfile.write(converted_data,sizeof(converted_data));
        }
        if (it->type == FileInfo::FT_BIN)
        {
            ifstream file(it->name, ios::in|ios::binary);
            if (!file.is_open())
            {
                cout << "Can't open file" << endl;
                return 0;
            }
            char data[it->size];
            file.read(data,sizeof(data));
            file.close();
            outfile.write(data,it->size);
            cout << "(binary file)" << endl;
        }
    }
/*    ifstream file ("example.bin", ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
      size = file.tellg();
      memblock = new char [size];
      file.seekg (0, ios::beg);
      file.read (memblock, size);
      file.close();

      cout << "the entire file content is in memory";

      delete[] memblock;
    }
    else cout << "Unable to open file";*/
    return 0;
}


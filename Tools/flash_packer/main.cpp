#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

#define PATH_RESOURCES      "../../Resources/Flash"
#define PATH_STRINGS_TPL    "../../FW/inc/sm_strings.tpl"
#define PATH_STRINGS_H      "../../FW/inc/sm_strings.h"

// Include dirent.h (simple directory navigation)
#if defined(WIN32)
    #include "dirent_win.h"
#elif defined(__linux__)
    #include <dirent.h>
#else
    // Didn't test on other operating systems
    // If you have dirent.h in your OS, please include it
    #error "Unknown OS"
#endif

// All functions declarations
#include "global.h"

/// @brief Maximum number of flash elements
const int MAX_ELEMENTS = 100;

/// @brief Path to the resources folder
std::string resourcePath = "";

/// @brief Get file extension
std::string fileExt(std::string fileName)
{
    std::string result = "";
    size_t extpos = fileName.find_last_of('.');

    if (extpos == std::string::npos)
        return result;

    result = fileName.substr(extpos + 1,fileName.length());
    return result;
}

/// @brief Get full file name (including path)
std::string fullName(std::string fileName)
{
    std::string ret = resourcePath;
    ret.append("/");
    ret.append(fileName);
    return ret;
}

/// @brief Search for the file starting from the specified number (e.g. "000.bin" or "001_BLABLABLA.txt")
void fileSearch(int number, const std::vector<std::string> * fileList, std::vector<std::string> * result)
{
    result->clear();

    std::string numStr;
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(3) << number;
    numStr = oss.str();

    for (auto it = fileList->begin(); it != fileList->end(); ++it)
    {
        std::string fileName;
        fileName = *it;
        if (fileName.compare(0,numStr.length(),numStr) != 0)
            continue;

        // File is found
        result->push_back(fileName);
    }
}

int main()
{
    // List of files in the resources folder
    std::vector<std::string> fileList;
    // List of parsed fonts
    std::vector<FontTable *> fontList;
    // List of flash elements
    std::vector<FlashElement> flashElements;

    // Store resources folder path
    resourcePath = PATH_RESOURCES;

    // Get file list in the resources folder
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (resourcePath.c_str())) == NULL)
    {
        // Can not open directory
        std::cerr << "Can't open directory. Nothing to process" << std::endl;
        return EXIT_FAILURE;
    }

    // Add files with names starting from 3 digits to the list
    // Ignore the rest
    while ((ent = readdir (dir)) != NULL) {
        std::string fileName = ent->d_name;
        std::string number = fileName.substr(0,3);
        if (number.length() != 3)
            continue;
        if (! std::isdigit(number[0]))
            continue;
        if (! std::isdigit(number[1]))
            continue;
        if (! std::isdigit(number[2]))
            continue;

        fileList.push_back(fileName);
    }
    closedir (dir);

    // Parse fileList
    int number = 0;
    while (true)
    {
        // Create a list of files matching to "NNN*" template
        std::vector<std::string> matchList;
        fileSearch(number, &fileList, &matchList);

        // If only one file is found
        if (matchList.size() == 1)
        {
            // Open this file
            std::string resourceName = fullName(matchList[0]);
            std::ifstream inFile(resourceName, std::ios::in | std::ios::binary);

            // Check for the supported extension
            if (fileExt(matchList[0]).compare("txt") == 0)
            {
                std::cout << matchList[0] << ": text"  << std::endl;
                FlashElement element;
                if (parseTxt(inFile, &element) == false)
                {
                    std::cout << "Parsing failed" << std::endl;
                    std::cout << "Stopping." << std::endl;
                    inFile.close();
                    break;
                }
                flashElements.push_back(element);
            }
            else if (fileExt(matchList[0]).compare("bin") == 0)
            {
                std::cout << matchList[0] << ": binary" << std::endl;
                FlashElement element;
                if (parseBin(inFile, &element) == false)
                {
                    std::cout << "Parsing failed" << std::endl;
                    std::cout << "Stopping." << std::endl;
                    inFile.close();
                    break;
                }
                flashElements.push_back(element);
            }
            else if (fileExt(matchList[0]).compare("pbm") == 0)
            {
                std::cout << matchList[0] << ": image" << std::endl;
                FlashElement element;
                if (parsePbm(inFile, &element) == false)
                {
                    std::cout << "Parsing failed" << std::endl;
                    std::cout << "Stopping." << std::endl;
                    inFile.close();
                    break;
                }
                flashElements.push_back(element);
            }
            else
            {
                // Unknown extension
                std::cout << matchList[0] << ": unsupported file type" << std::endl;
                std::cout << "Stopping." << std::endl;
                break;
            }
            // File is parsed (or skipped), close file
            inFile.close();
        }
        // If two files are found (it can be font)
        else if (matchList.size() == 2)
        {
            // Determine which file contains symbols in UCS-2LE without BOM
            // and which file contains symbols images
            uint32_t txtNum;
            uint32_t pbmNum;
            if ((matchList[0].find(".txt",matchList[0].length() - 4) != std::string::npos) &&
                (matchList[1].find(".pbm",matchList[0].length() - 4) != std::string::npos))
            {
                txtNum = 0;
                pbmNum = 1;
            }
            else if ((matchList[0].find(".pbm",matchList[0].length() - 4) != std::string::npos) &&
                     (matchList[1].find(".txt",matchList[0].length() - 4) != std::string::npos))
            {
                txtNum = 1;
                pbmNum = 0;
            }
            else
            {
                std::cout << "Wrong font files" << std::endl;
                std::cout << "Stopping." << std::endl;
                break;
            }

            std::cout << matchList[0] << " and " << matchList[1] << ": font" << std::endl;

            // Open font files
            std::ifstream inFileTxt(fullName(matchList[txtNum]), std::ios::in | std::ios::binary);
            std::ifstream inFilePbm(fullName(matchList[pbmNum]), std::ios::in | std::ios::binary);

            // Parse font
            FlashElement element;
            FontTable * table = new FontTable();
            if (parseFont(inFileTxt, inFilePbm, &element, table) == false)
            {
                std::cout << "Parsing failed" << std::endl;
                std::cout << "Stopping." << std::endl;
                delete table;
                inFileTxt.close();
                inFilePbm.close();
                break;
            }
            // Store font element index
            table->index = number;
            // Store font table
            fontList.push_back(table);
            // Store flash element
            flashElements.push_back(element);

            // Close both font files
            inFilePbm.close();
            inFileTxt.close();
        }
        // Nothing found - assume that it was the end
        else if (matchList.size() == 0)
        {
            std::cout << "Done." << std::endl;
            break;
        }
        else
        {
            std::cout << "Wrong number of files (" << matchList.size() <<") is found for number " << std::setfill('0') << std::setw(3) << number << std::endl;
            std::cout << "Stopping." << std::endl;
            break;
        }

        number++;
        if (number >= MAX_ELEMENTS)
        {
            std::cout << "Maximum of " << MAX_ELEMENTS <<" resource files is allowed." << std::endl;
            std::cout << "Stopping." << std::endl;
            break;
        }
    }

    // Here we will write output update file for the external flash memory
    std::ofstream updateFile(fullName("update.bin"), std::ios::out | std::ios::binary | std::ios::trunc);

    // Here is a flash structure, size is in bits, offset in bytes
    // Offset       | Parameter | Size  | Description
    // ------------ | --------- | ----- | ------
    // 0x0000       | nElements | 32    | Number of elements stored
    // 0x0001       | offset1   | 32    | Offset of the first element. Must be equal to 1 + nElements*8 + 4.
    // 0x0005       | size1     | 32    | Size of the first element
    // 0x0009       | offset2   | 32    | Offset of the second element
    // 0x000D       | size2     | 32    | Size of the second element
    // ...          | ...       | ...   | ... Other parameters ...
    // offset1 - 4  | crc32     | 32    | Checksum of the header
    // offset1      | data1     | size1 | Actual data for the first element
    // offset2      | data2     | size2 | Actual data for the second element
    // ...          | ...       | ...   | ... Other data elements ...

    // Size of the header in uint32_t
    //               nElements   offsets and sizes    CRC32
    uint32_t headerSize = 1 + flashElements.size()*2 + 1;

    // Init CRC
    uint32_t crc32 = 0xFFFFFFFF;

    // write nElements
    uint32_t nElements = flashElements.size();
    updateFile.write((char *)&nElements,sizeof(uint32_t));
    crc32 = calcCRC32(crc32, (char *)&nElements, sizeof(nElements));

    // write elements offsets and sizes
    uint32_t offset = headerSize * sizeof(uint32_t);
    for (auto it = flashElements.begin(); it != flashElements.end(); ++it)
    {
        FlashElement element = *it;
        // Write element offset
        updateFile.write((char *)&offset,sizeof(uint32_t));
        crc32 = calcCRC32(crc32, (char *)&offset, sizeof(offset));
        // Write element size
        updateFile.write((char *)&element.size,sizeof(uint32_t));
        crc32 = calcCRC32(crc32, (char *)&element.size, sizeof(element.size));
        // Increment offset
        offset += element.size;
    }

    // Write CRC32
    updateFile.write((char *)&crc32,sizeof(crc32));

    // Write elements
    for (auto it = flashElements.begin(); it != flashElements.end(); ++it)
    {
        FlashElement element = *it;
        updateFile.write(element.data,element.size);
        if (element.size) delete[] element.data;
    }

    // Close update file
    updateFile.close();

    // Parse sm_strings template file and make sm_strings.h
    std::ifstream inFile(PATH_STRINGS_TPL, std::ios::in | std::ios::binary);
    std::ofstream outFile(PATH_STRINGS_H, std::ios::out | std::ios::binary | std::ios::trunc);

    sm_strings_process(inFile,outFile,fontList);

    // Free resources
    for (auto it = fontList.begin(); it != fontList.end(); ++it)
    {
        FontTable * element = *it;
        delete[] element->symbols;
        delete element;
    }

    inFile.close();
    outFile.close();
}


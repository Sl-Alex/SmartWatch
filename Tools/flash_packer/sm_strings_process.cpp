#include "global.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <iomanip>

// Split string to up to 5 elements. If string has more than 5 elements then last element
// will contain the rest of the string
void string_split(std::string from, std::vector<std::string> &to, std::string delimiters = u8" \n\r")
{
    to.clear();

    size_t posBegin = 0;
    size_t posEnd = 0;
    std::string word;

    // Replace \t with space
    std::replace( from.begin(), from.end(), '\t', ' ');
    from.erase(from.find_last_not_of(" \n\r\t")+1);

    while (true)
    {
        posBegin = from.find_first_not_of(delimiters,posEnd);
        if (posBegin == std::string::npos) return;

        posEnd = from.find_first_of(delimiters,posBegin);
        if (posEnd == std::string::npos)
        {
            word = from.substr(posBegin, from.length() - posBegin);
        }
        else
        {
            word = from.substr(posBegin, posEnd - posBegin);
        }

        to.push_back(word);
        if (to.size() == 4)
        {
            posBegin = from.find_first_not_of(delimiters,posEnd);

            if (posBegin == std::string::npos) return;

            word = from.substr(posBegin, from.length() - posBegin);
            to.push_back(word);
            break;
        }
    }
}

// Process sm_strings template file
void sm_strings_process(std::ifstream &inFile, std::ofstream &outFile, std::vector<FontTable *> fontList)
{
    std::string line;
    struct FontParams
    {
        std::string name;
        uint32_t index;
    };
    std::vector<FontParams> fontParams;

    // Reset text file to the beginning
    inFile.seekg(0, std::ios::beg);

    // Searching for all FONT_IDX_xxx definitions
    while(std::getline(inFile,line))
    {
        std::vector<std::string> wordsList;
        string_split(line, wordsList);
        outFile << line << std::endl;
        // Check only word lists with size >=3
        if (wordsList.size() >= 3)
        {
            if ((wordsList[0] == "#define") &&
                (wordsList[1].compare(0, 9, u8"FONT_IDX_") == 0) &&
                (wordsList[1].length() != 9))
            {
                FontParams params;
                params.name = wordsList[1];
                params.index = std::stoi(wordsList[2]);
                fontParams.push_back(params);
            }
            if (wordsList.size() >= 5)
            {
                if (wordsList[1] == "PACK")
                {
                    // Get string name
                    std::string strName = wordsList[2];
                    // Find appropriate font from the list
                    for (auto it = fontParams.begin(); it != fontParams.end(); ++it)
                    {
                        FontParams params = *it;
                        if (params.name == wordsList[3])
                        {
                            std::string strData = wordsList[4];

                            std::u16string utf16 = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(strData.data());
                            //std::cout << "UTF16 conversion produced " << utf16.size() << " code units:\n";
                            //for (char16_t c : utf16)
                                //std::cout << std::hex << std::showbase << c << '\n';

                            outFile << "#define " << strName << " (uint16_t *)\"";
                            // Find appropriate font table
                            FontTable * table = 0;
                            for (auto fontIt = fontList.begin(); fontIt != fontList.end(); ++fontIt)
                            {
                                table = *fontIt;
                                if (table->index == params.index)
                                {
                                    // Found existing index
                                    break;
                                }
                            }
                            if (table == 0)
                                break;

                            // For each symbol
                            uint32_t sz = 0;
                            for (char16_t c : utf16)
                            {
                                for (uint32_t i = 0; i < table->count; ++i)
                                {
                                    if (table->symbols[i] == c)
                                    {
                                        outFile << "\\x" << std::hex << std::setfill('0') << std::setw(2) << (i & 0xFF);
                                        outFile << "\\x" << std::hex << std::setfill('0') << std::setw(2) << (i >> 8);
                                        sz++;
                                        break;
                                    }
                                }
                            }
                            outFile << "\"" << std::endl;
                            outFile << "#define " << strName << "_SZ " << std::dec << sz << std::endl;
                            outFile << std::endl;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Reset text file to the beginning
    inFile.seekg(0, std::ios::beg);

    // Copy all lines to the output file with the on-the-fly strings patching

}

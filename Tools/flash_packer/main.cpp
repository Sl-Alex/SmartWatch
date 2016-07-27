#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

// Include dirent.h
#if defined(WIN32)
#include "dirent.h"
#elif defined(LINUX)
#include <dirent.h>
#else
#error "Unknown OS"
#endif

using namespace std;

const int MAX_FILES = 100;

struct FileInfo{
    string name;
    enum FileType{FT_PBM, FT_BIN} type;
    uint32_t size;
};

string fileExt(string fileName)
{
    string result = "";
    size_t extpos = fileName.find_last_of('.');

    if (extpos == std::string::npos)
        return result;

    result = fileName.substr(extpos + 1,fileName.length());
    return result;
}

/// @brief Search
void fileSearch(int number, const vector<string> * fileList, vector<string> * result)
{
    result->clear();

    string numStr;
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(3) << number;
    numStr = oss.str();

    for (auto it = fileList->begin(); it != fileList->end(); ++it)
    {
        string fileName;
        fileName = *it;
        if (fileName.compare(0,numStr.length(),numStr) != 0)
            continue;

        // File is found
        result->push_back(fileName);
    }
}

int main()
{
    // List of files
    vector<string> fileList;

    // Get file list in the directory
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (".")) == NULL)
    {
        // Can not open directory
        std::cerr << "Can't open directory. Nothing to process" << std::endl;
        return EXIT_FAILURE;
    }

    // Add only files with names starting from 3 digits */
    while ((ent = readdir (dir)) != NULL) {
        string fileName = ent->d_name;
        string number = fileName.substr(0,3);
        if (number.length() != 3)
            continue;
        if (! isdigit(number[0]))
            continue;
        if (! isdigit(number[1]))
            continue;
        if (! isdigit(number[2]))
            continue;

        fileList.push_back(fileName);
    }

    // Parse fileList
    int number = 0;
    while (true)
    {
        vector<string> matchList;

        // Get a list of files matching to "NNN*" template
        fileSearch(number, &fileList, &matchList);

        if (matchList.size() == 1)
        {
            if (fileExt(matchList[0]).compare("txt") == 0)
            {
                /// @todo Implement
                std::cout << matchList[0] << " - text element"  << std::endl;
            }
            else if (fileExt(matchList[0]).compare("bin") == 0)
            {
                /// @todo Implement
                std::cout << matchList[0] << " - binary element" << std::endl;
            }
            else if (fileExt(matchList[0]).compare("pbm") == 0)
            {
                /// @todo Implement
                std::cout << matchList[0] << " - image element" << std::endl;
            }
            else
            {
                /// @todo Implement
                std::cout << matchList[0] << " - unknown element" << std::endl;
            }
        }
        else if (matchList.size() == 2)
        {
            /// @todo Implement
            std::cout << matchList[0] << " and " << matchList[1] << " can contain font" << std::endl;
        }
        else if (matchList.size() == 0)
        {
            std::cout << "Stopping." << std::endl;
            break;
        }
        else
        {
            std::cout << "Wrong number of files (" << matchList.size() <<") is found for number " << std::setfill('0') << std::setw(3) << number << std::endl;
            std::cout << "Stopping." << std::endl;
            break;
        }


        number++;
    }

    // Here we will write output file
    ofstream outfile("out.bin", ios::out|ios::binary|ios::trunc);
    char header[10];
    memset(header,0,sizeof(header));
    header[0] = 'A';
    header[1] = 'B';
    header[2] = 'C';
    header[3] = 'D';
    outfile.write(header,sizeof(header));
    outfile.close();

    closedir (dir);
}


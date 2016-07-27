#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>

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

        //std::cout << fileName << std::endl;
    }

    for (auto it = fileList.begin(); it != fileList.end(); ++it)
    {
        std::cout << *it << std::endl;
    }

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


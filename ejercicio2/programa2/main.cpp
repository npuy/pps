#include <iostream>
#include <filesystem>
#include <fstream>

#include "LOCCounter.h"

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <root_directory>\n";
        return 1;
    }

    std::string rootDir = argv[1];

    if (!fs::exists(rootDir) || !fs::is_directory(rootDir))
    {
        std::cerr << "Error: Directory '" << rootDir << "' does not exist.\n";
        return 1;
    }

    LOCCounter counter;
    if (!counter.getSourceFiles(rootDir))
    {
        std::cerr << "Error: Could not retrieve source files from directory.\n";
        return 1;
    }

    counter.processFiles();
    counter.printReport();
}

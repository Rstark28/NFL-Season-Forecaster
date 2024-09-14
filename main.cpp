#include <iostream>
#include <string>
#include "NFLSim.h"

int main(int argc, char *argv[])
{
    // Check if the correct number of arguments is provided
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    // Get the file name from command-line arguments
    std::string filename = argv[1];

    // Pass the file name to NFLSim
    NFLSim newSim(filename);

    return 0;
}

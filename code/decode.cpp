#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <bits/stdc++.h>

using namespace std;

typedef basic_string<uint8_t> ustring;

int main(int argc, char *argv[])
{
    // Input validation
    if (argc != 2)
    {
        if (argc > 2)
            cout << "Too many arguments for decoder" << endl;
        else
            cout << "You need to specify the filepath!" << endl;
        return 1;
    }

    // Read the filepath from arguments and open file
    string filePath(argv[1]);
    ifstream inputFile(filePath);

    // Exit if file did not open
    if (!inputFile.is_open())
    {
        cerr << "Failed to open inputFile: " << filePath << endl;
        return 1;
    }

    // Initialize dictionary with all 256 ACII values with <number, string> pair
    map<uint16_t, ustring> dict;
    for (int i = 0; i < 256; i++)
    {
        uint8_t c = static_cast<uint8_t>(i);
        ustring s(1, c);
        dict.insert(make_pair(i, s));
    }

    // Reading the very first byte of the input file
    char initBuf[2];
    inputFile.read(initBuf, 2);

    // Read most- and least significant bytes - little endian format
    uint8_t initLo = static_cast<uint8_t>(initBuf[0]);
    uint8_t initHi = static_cast<uint8_t>(initBuf[1]);

    // Reconstructing the 2-byte number from 2 individual bytes
    uint16_t initNum = (initHi << 8) | initLo;

    // Initializing the string output and last sequence variables
    ustring output;
    ustring lastSeq = dict.at(initNum);
    output += lastSeq;
    size_t inputSize = 0;

    cout << "Starting to decompress content from inputfile: " << filePath << endl;

    // Read until reached end-of-file
    while (true)
    {
        // Read two bytes from stream
        char buf[2];
        inputFile.read(buf, 2);
        // Checking if 2 bytes were read from stream
        if (inputFile.gcount() < 2)
        {
            break;
        }
        // Read most- and least significant bytes - little endian format
        uint8_t lo = static_cast<uint8_t>(buf[0]);
        uint8_t hi = static_cast<uint8_t>(buf[1]);

        // Reconstructing the 2-byte number from 2 individual bytes
        uint16_t num = (hi << 8) | lo;

        // Current sequence of chars
        ustring currentSeq = dict.find(num) != dict.end()
                                 ? dict.at(num)             // If num is inside dict, retrieve chars
                                 : lastSeq + lastSeq.at(0); // if num is not inside dict, use last sequence

        output += currentSeq;                                            // Append retieved chars to the output
        dict.insert(make_pair(dict.size(), lastSeq + currentSeq.at(0))); // Update dict with the new sequence of chars

        lastSeq = currentSeq;
        inputSize++;
    }
    // Close file 
    inputFile.close();
    
    // Print out usefull information
    cout << "Read " << inputSize * 2 << " bytes from the input file" << endl;
    cout << "Dictionary size is " << dict.size() << endl;

    // Construct name for file with decompressed data
    filesystem::path p(filePath);
    string filename = p.stem().string();
    filename = filename.substr(0, filename.find_first_of("_"));
    string outputFilePath = "./examples/" + filename + "_decompressed";

    // Make file with decompressed data
    ofstream decompressedFile(outputFilePath);
    for (uint8_t c : output)
    {
        decompressedFile << c;
    }
    decompressedFile.close();
    return 0;
}
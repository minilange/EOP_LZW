#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <bits/stdc++.h>

using namespace std;
#define MAX_DICT_SIZE 65536
typedef basic_string<uint8_t> ustring;

int main(int argc, char *argv[])
{

    // Input validation
    if (argc != 2)
    {
        if (argc > 2)
            cout << "Too many arguments for encoder" << endl;
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

    // Initialize dictionary with all 256 ACII values with <string, number> pair
    map<ustring, uint16_t> dict;
    for (int i = 0; i < 256; i++)
    {
        uint8_t c = static_cast<uint8_t>(i);
        ustring s(1, c);
        dict.insert(make_pair(s, i));
    }

    // Initializes the output and current sequence variables
    size_t inputSize = 0;
    vector<uint16_t> output;
    ustring currentSeq;

    cout << "Starting to compress contents of inputFile: " << filePath << endl;

    // Reading characters one by one until all is read
    typedef istreambuf_iterator<char> ibuf_iterator;
    for (ibuf_iterator c(inputFile), end; c != end; ++c)
    {
        // Convert char to a string
        ustring seq(1, *c);
        ustring charsToAdd = currentSeq + seq;

        // Check if the current sequence plus newly read character exists in dict
        if (dict.find(charsToAdd) != dict.end())
        {
            currentSeq = charsToAdd;
        }
        else
        {
            // Update output and dict with current sequence
            output.push_back(dict.at(currentSeq));
            // IMPORTANT! - Otherwise compressor will run wild!
            if (dict.size() < MAX_DICT_SIZE)
                dict.insert(make_pair(charsToAdd, dict.size()));
            currentSeq = seq;
        }

        inputSize++;
    }
    // Close file
    inputFile.close();
    
    // Ensure all characters has been written to output
    if (currentSeq.size() > 0)
    {
        output.push_back(dict.at(currentSeq));
    }
    
    // Print out usefull information
    cout << "Original message is " << inputSize << " bytes long" << endl;
    cout << "Encoded message is " << output.size() * 2 << " bytes long" << endl;
    cout << "Dictionary size is " << dict.size() << endl;

    // Construct name for file with compressed data
    filesystem::path p(filePath);
    string filename = p.stem().string();
    string outputfilePath = "./examples/" + filename + "_compressed";

    // Make file with compressed data
    ofstream compressedFile(outputfilePath, ios::binary);
    for (uint16_t code : output)
    {
        compressedFile.write(reinterpret_cast<const char *>(&code), sizeof(code));
    }
    compressedFile.close();

    return 0;
}
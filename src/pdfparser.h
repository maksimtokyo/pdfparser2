#ifndef PDFPARSER_H
#define PDFPARSER_H


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iterator>
#include <utility>
#include <stack>

#include "zlib.h"

class PdfParser{
    std::string filename{""};
    std::vector<std::pair<long , long int>> refs;


    // methods
    void setfilename(std::string filename);
    void createrefs(std::ifstream& file, long long pos);
    void slines(std::ifstream& file, std::string& buf, int count);
    std::string readnumber(std::string::iterator it, std::string::iterator end);
    void decompress(std::vector<uint8_t>& data);
    void transformation(std::vector<uint8_t>& decompressed_data, size_t sizearr);
    void parsfile();
public:
    PdfParser();
    //methods
    void decompress_refs();
    void readfiles(std::vector<std::string> filenames);


};

#endif

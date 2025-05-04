#include "pdfparser.h"
#include "custombuffer/custombuffer.hpp"
#include "customfunctions/customfunction.h"

PdfParser::PdfParser() {}

void PdfParser::setfilename(std::string filename)
{
    this->filename  = filename;
}

void PdfParser::readfiles(std::vector<std::string> filenames)
{
    for (const auto filename : filenames)
    {
        setfilename(filename);
        parsfile();
    }
}

void PdfParser::slines(std::ifstream& file, std::string& buf, int count)
{
    for (int i = 0; i < count; ++i)
        std::getline(file, buf);
}


std::string PdfParser::readnumber(std::string::iterator it, std::string::iterator end)
{
    std::string str_buf = "";
    while (it != end && (*it >= '0' && *it <= '9')){
        str_buf += *it;
        ++it;
    }
    return str_buf;
}


void PdfParser::createrefs(std::ifstream& file, long long ref)
{
    file.seekg(ref, std::ios::beg);
    std::string buf = ""; std::streampos prevpos = 0;
    slines(file, buf, 1);

    while(buf[0] != 't')
    {
        slines(file, buf, 1);
        if (CustomFunction::stripTrailing(buf, [](char c){return !(c >= 97 && c <= 122);}) == 'n')
        {
            prevpos = file.tellg();
            file.seekg(std::stoi(readnumber(buf.begin(), buf.end())), std::ios::beg);
            slines(file, buf, 2);
            if (buf.find("FlateDecode") != std::string::npos)
            {
                long int it = buf.find("Length ");

                if (it != std::string::npos && buf.find("Image") == std::string::npos)
                {
                    it+=7;
                    long long refp = file.tellg();
                    std::pair<long long, long int> ref{refp, std::stoll(readnumber(buf.begin() + it, buf.end()))};
                    refs.push_back(ref);
                }
            }
            file.seekg(prevpos, std::ios::beg);
        }
    }

    slines(file, buf, 1);
    long long ptr = buf.find("Prev ");
    if (ptr != std::string::npos)
    {
        std::string::iterator pt = buf.begin() + ptr + 5;
        createrefs(file, std::stoi(readnumber(pt, buf.end())));
    }
}

void PdfParser::decompress(std::vector<uint8_t>& data)
{
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    if (inflateInit(&stream) != Z_OK)
        return;

    size_t CHUNK_SIZE = data.size()*4;
    std::vector<unsigned char> decompressed_data(data.size());
    stream.next_in = const_cast<unsigned char*>(data.data());
    stream.avail_in = data.size();
    int ret;

    do{
        decompressed_data.resize(CHUNK_SIZE + stream.total_out);
        stream.next_out = decompressed_data.data() + stream.total_out;
        stream.avail_out = CHUNK_SIZE;
        ret = inflate(&stream, Z_NO_FLUSH);
        if (ret != Z_OK && ret != Z_STREAM_END)
            return;
    } while (ret != Z_STREAM_END);
    transformation(decompressed_data, stream.total_out);
    inflateEnd(&stream);
}

void PdfParser::decompress_refs()
{
    for (int i = refs.size() - 1; i >= 0; --i)
    {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        file.seekg(refs[i].first + 7, std::ios::beg);
        std::vector<uint8_t> data(refs[i].second);
        file.read(reinterpret_cast<char*>(data.data()), data.size());
        decompress(data);
    }
}

void PdfParser::transformation(std::vector<uint8_t>& decompressed_data, size_t sizearr)
{
    std::vector<uint8_t>::iterator iter = decompressed_data.begin();
    std::vector<uint8_t>::iterator end = decompressed_data.begin() + sizearr - 1;
    std::stack<double> stack;
    std::string num = "";

    while (iter != end)
    {
        if (*iter >= '0' && *iter <= '9'){
            while (iter != end && ((*iter >= '0' && *iter <= '9') || *iter == '.'))
            {
                num += *iter++;
            }
            stack.push(std::stold(num));
            num = "";
        }else if (*iter == 'c' && *(iter + 1) == 'm' && *(iter - 1) == ' '){
            iter+=2;
        }else if (*iter == 'm'  && *(iter-1) == ' '&& stack.size() >= 2)
        {
        }else if(*iter == 'c' && *(iter-1) == ' ' && stack.size() >= 6)
        {
        }else if(*iter == 'l' && *(iter-1) == ' ' && stack.size() >= 2)
        {
        }else if (*iter == 'q' && *(iter - 1) == ' ')
        {
        }
        if (iter != end) ++iter;
    }
}

void PdfParser::parsfile()
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return;

    std::streampos search_pos = file.tellg();
    std::vector<uint8_t> buffer(READ_CHUNK_SIZE);
    CustomVector<uint8_t> customBuffer;

    do{
        search_pos -= READ_CHUNK_SIZE;
        file.seekg(search_pos, std::ios::beg);
        file.read(reinterpret_cast<char*>(buffer.data()), READ_CHUNK_SIZE);
        customBuffer += buffer;
        customBuffer.important_iterator = CustomFunction::customsearch_beg(customBuffer.getbegin(), customBuffer.getend(), "startxref");
    }while (search_pos > 0 && customBuffer.important_iterator == customBuffer.getend());
    createrefs(file, customBuffer.getstartxreftable());
}

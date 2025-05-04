#include <iostream>
#include <chrono>
#include "src/pdfparser.h"


int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    PdfParser m_pars;
    m_pars.readfiles({
        "test1.pdf",
        // "test2.pdf",
        // "test3.pdf",
        // "test4.pdf",
    });
    m_pars.decompress_refs();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds\n";

}

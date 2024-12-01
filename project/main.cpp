#include "page.hpp"

#include <sstream>
#include <chrono>
#include <thread>

const std::string URL = "https://www.labirint.ru/books/";

SCBeautifulSoup scbs;
Parser parser_;

const std::vector<std::string> HEADERS = {
    "X-Requested-With: XMLHttpRequest",
    "Referer: https://www.labirint.ru/books/0/"
};

int main()
{
    //const int id = 10017284;  // 1017284;  //877234
    Request *req = new RequestCurl;
    LabyrinthPage labirintpage( req, scbs, parser_, URL, HEADERS );
    //std::vector<int> ids = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 10017284, 1017284, 877234, 8772340 };
    //std::stringstream sos;

    int id = 200000;
    try{
    while( true )
    {
        std::cout << "------------------------\t" << id << std::endl;
        const auto start = std::chrono::high_resolution_clock::now();
        if( labirintpage(id++) )
        {
            std::stringstream sos;
            sos << labirintpage;
            std::cout << sos.str();
        }
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << "\t\ttime: " << elapsed << "\n";
    }
    }catch( std::exception& ex )
    {
        std::cout << ex.what() << std::endl;
    }
    //lp(id);
    //std::stringstream sos;
    //sos << lp;
    //std::cout << sos.str();
    delete req;
    return 0;
}



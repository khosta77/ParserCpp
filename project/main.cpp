#include "page.hpp"

#include <sstream>
#include <chrono>
#include <thread>

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdint>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <deque>
#include <functional>
#include <exception>
#include <future>
#include <iomanip>
#include <ctime> 

const std::string URL = "https://www.labirint.ru/books/";

const std::vector<std::string> HEADERS = {
    "X-Requested-With: XMLHttpRequest",
    "Referer: https://www.labirint.ru/books/0/"
};

class ParserPool
{
private:

    size_t count_active_;
    std::atomic<size_t> page_i_;
    std::atomic<size_t> page_I_;
    size_t count;
    const size_t balance;

    std::vector<std::thread> threads_;
    std::vector<double> timeOneIter_;

    std::mutex mutex_;

    const std::string table = "data/table.csv";
    const std::string logger = "data/logger.log";

    std::string start_time;

    std::string generateTimeString()
    {
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);

        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << localTime->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << localTime->tm_min << ":"
            << std::setw(2) << std::setfill('0') << localTime->tm_sec;

        return oss.str();
    }


public:
    explicit ParserPool( const size_t& N, const size_t& start = 0, const size_t& end = 1'200'00) :
        count_active_(N), page_i_(start), page_I_(end), balance( ( ( start != 0 ) ? start : 0 ) ), count(0)
    {
        std::cout << std::format( "Запущено {} потоков\nДиапазон значений от {} до {}\n", N, start, end );
        start_time = generateTimeString();  // std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        // Это ужасно написано, да я знаю. Очень сильно тороплюсь....
        std::ofstream out( table.c_str(), ( std::ios_base::trunc ) );
        if( !out.is_open() )
        {
            std::cerr << "Все очень плохо\n";
            throw;
        }
        out << "id;typeObject;groupOfType;underGroup;genres;bookName;imgUrl;age;authors;publisher;datePublisher;series;bookGenres;allPrice;myPrice;sale;isbn;pages;pageType;weight;da;db;dc;box;covers;decoration;illustrations;rate;rateSize;annotation\n";
        out.close();
        std::ofstream logout( logger.c_str(), ( std::ios_base::trunc ) );
        if( !logout.is_open() )
        {
            std::cerr << "Все очень плохо2\n";
            throw;
        }
        logout.close();

        std::unique_lock<std::mutex> lock(mutex_);
        for( size_t i = 0; i < N; ++i )
        {
            threads_.emplace_back(&ParserPool::ThreadFunction, this);
        }
    }

    ~ParserPool()
    {
        for( std::thread& t : threads_ )
        {
            t.join();
        }
    }

    int run()
    {
        ThreadFunction();
        return 0;
    }

private:
    float round(float var)
    {
        float value = (int)(var * 100 + .5);
        return (float)value / 100;
    }
    
    void saveToFile( const std::string& content, const std::string& fn )
    {
        std::lock_guard<std::mutex> guard( mutex_, std::adopt_lock );
        std::ofstream out( fn.c_str(), ( std::ios_base::app ) );
        if( !out.is_open() )
            throw std::runtime_error( ( "<error>File '" + fn + "' not open!!!</error>" ) );
        
        if( fn == table )
            ++count;
        
        out << content;
        out.close();
    }

    void ThreadFunction()
    {
        SCBeautifulSoup scbs;
        Parser parser_;
        Request *req = new RequestCurl;
        LabyrinthPage labirintpage( req, scbs, parser_, URL, HEADERS );
        bool status = false;
        for( size_t i = page_i_++, I = page_I_; i < I; i = page_i_++ )
        {
            const auto start = std::chrono::high_resolution_clock::now();
            try
            {
                status = labirintpage( i );
            }
            catch( const std::exception& ex )
            {
                saveToFile( std::format("===>{}\t{}\n", i, ex.what() ), logger );
                continue;
            }
            catch(...)
            {
                saveToFile( std::format("--->{}\tno std::throw, what was ???\n", i ), logger );
                continue;
            };
            const auto end = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double, std::milli> d = end - start;

            if( status )
            {
                std::stringstream sos;
                sos << labirintpage;
                saveToFile( sos.str(), table );
            }

            std::cout << std::format(
                    "  {:>5} % [{:>7}:{:>7}] Save: {:>5}/{:>6} time: {:>12};\t{}\t{}\n",
                    round( ( ( ( i - balance ) * 1.0f ) / ( ( I - balance ) * 1.0f ) * 100.0f ) ),
                    i, I, ( ( status ) ? "*YES*" : "*NO*" ), count, d, start_time, generateTimeString()
                );
        }

        delete req;
    }
};

int main()
{
    ParserPool pp( std::thread::hardware_concurrency() );
    return pp.run();
}



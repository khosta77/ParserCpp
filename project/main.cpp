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

const std::string URL = "https://www.labirint.ru/books/";

const std::vector<std::string> HEADERS = {
    "X-Requested-With: XMLHttpRequest",
    "Referer: https://www.labirint.ru/books/0/"
};

class ParserPool
{
private:
    enum class State
    {
        WORKING,
        STOPPING,
        STOPPED
    };

    State state_;
    size_t count_active_;
    std::atomic<size_t> page_i_;
    std::atomic<size_t> page_I_;
    size_t count;
    const size_t balance;

    std::vector<std::thread> threads_;
    std::vector<double> timeOneIter_;

    std::mutex mutex_;
    std::condition_variable cond_var_;
    std::condition_variable cond_update_;

    const std::string table = "table.csv";

public:
    explicit ParserPool( const size_t& N, const size_t& start, const size_t& end ) : state_(State::STOPPED),
        count_active_(N), page_i_(start), page_I_(end), balance( ( ( start != 0 ) ? start : 0 ) ), count(0)
    {
        
        std::ofstream out( table, ( std::ios_base::trunc ) );
        if( !out.is_open() )
        {
            std::cerr << "Все очень плохо\n";
            throw;
        }
        out << "id;typeObject;groupOfType;underGroup;genres;bookName;imgUrl;age;authors;publisher;datePublisher;series;bookGenres;allPrice;myPrice;sale;isbn;pages;pageType;weight;da;db;dc;box;covers;decoration;illustrations;rate;rateSize;annotation\n";
        out.close();

        std::unique_lock<std::mutex> lock(mutex_);
        for( size_t i = 0; i < N; ++i )
        {
            threads_.emplace_back(&ParserPool::ThreadFunction, this);
        }
    }
    // ...no copy, no move...

    ~ParserPool()
    {
        for( std::thread& t : threads_ )
        {
            t.join();
        }
    }

    int progressbar()
    {
        state_ = State::WORKING;
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
        //std::unique_lock<std::mutex> lock(mutex_);

        SCBeautifulSoup scbs;
        Parser parser_;
        Request *req = new RequestCurl;
        LabyrinthPage labirintpage( req, scbs, parser_, URL, HEADERS );
        bool status = false;
        std::cout << std::this_thread::get_id() << std::endl;
        //cond_update_.wait( lock, [&]() { return ( state_ != State::WORKING ); } );

        for( size_t i = page_i_++, I = page_I_; i < I; i = page_i_++ )
        {
            const auto start = std::chrono::high_resolution_clock::now();
            try
            {
                status = labirintpage( i );
            }
            catch(...)
            {
                std::cerr << "\t>!!!" << std::endl;
            };
            const auto end = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double, std::milli> d = end - start;

            float p = round( ( ( ( i - balance ) * 1.0f ) / ( ( I - balance ) * 1.0f ) * 100.0f ) );
            std::string stts = ( ( status ) ? "YES" : "NO" );

            if( status )
            {
                std::stringstream sos;
                sos << labirintpage;
                saveToFile( sos.str(), table );
                //std::cout << sos.str();
            }
            std::cout << std::format( "  {} %\t{}/{}\tSave: {}/{}\ttime: {} ms\n", p, i, I, stts, count, d );
        }

        delete req;
    }
};

int main()
{
    ParserPool pp( 40, 200'000, 300'000 );
    return pp.progressbar();
}



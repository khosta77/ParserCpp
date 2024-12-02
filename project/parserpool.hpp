#ifndef PROJECT_PARSERPOOL_H_
#define PROJECT_PARSERPOOL_H_

#include "page.hpp"

#include <chrono>
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

    std::string generateTimeString();

public:
    explicit ParserPool( const size_t& N, const size_t& start = 0, const size_t& end = 1'200'00);
    ParserPool() = delete;
    ParserPool( const ParserPool& ) = delete;
    ParserPool( ParserPool&& ) = delete;
    ParserPool& operator=( const ParserPool& ) = delete;
    ParserPool& operator=( ParserPool&& ) = delete;
    ~ParserPool();

    int run();

private:
    float round( float var );
    
    void saveToFile( const std::string& content, const std::string& fn );

    void ThreadFunction();
};


#endif  // PROJECT_PARSERPOOL_H_

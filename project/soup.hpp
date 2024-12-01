#ifndef PROJECT_STEPAN_CPP_BEAUTIFULSOAP_HPP_
#define PROJECT_STEPAN_CPP_BEAUTIFULSOAP_HPP_

#include <string>
#include <vector>
#include <iostream>

class SCBeautifulSoup
{
private:
    std::vector<std::string> getKeyWords( const std::vector<std::pair<std::string, std::string>>& pairs );

    std::pair<std::string, size_t> getBlockTitle( const std::string& content, const size_t& start );

    bool isKeysNpos( const std::string& head, const std::vector<std::string>& keys );

    size_t findEndObject( const std::string& content, const size_t& start,
                          const std::string& tL, const std::string& tR );

public:
    SCBeautifulSoup() = default;
    SCBeautifulSoup(int) = delete;
    SCBeautifulSoup( const SCBeautifulSoup& ) = default;
    SCBeautifulSoup( SCBeautifulSoup&& ) = delete;
    ~SCBeautifulSoup() = default;

    std::pair<std::string, size_t> parseHref( const std::string& content, const std::string& object,
            const std::vector<std::pair<std::string, std::string>> pairs, const size_t& startPosition = 0 );

    std::pair<std::string, size_t> parseHead( const std::string& content, const std::string& object,
        const std::vector<std::pair<std::string, std::string>> pairs, const size_t& startPosition = 0 );

};

#endif   // PROJECT_STEPAN_CPP_BEAUTIFULSOAP_HPP_

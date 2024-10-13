#include <iostream>
#include <chrono>
#include <string>
#include <regex>

#include <curl/curl.h>

#define DEBUG 0

const std::string URL = "https://www.labirint.ru/books/877234/";

class Request
{
public:
    virtual std::string GET( const std::string& ) = 0;
};

class RequestCurl : public Request
{
private:
    CURL *curl;
    std::string content;

    // Функция обратного вызова для записи данных, полученных от curl
    static size_t WriteCallback( void* contents, size_t size, size_t nmemb, std::string* output )
    {
        size_t total_size = size * nmemb;
        output->append( static_cast<char*>(contents), total_size );
        return total_size;
    }

public:
    RequestCurl() : content("")
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
    }

    ~RequestCurl()
    {
        curl_global_cleanup();
    }
    
    std::string GET( const std::string& url ) override
    {
        if(curl)
        {
            content.clear();
            curl_easy_setopt( curl, CURLOPT_URL, url.c_str() );
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);

            if( CURLcode res = curl_easy_perform(curl); res != CURLE_OK )
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return content;
        }
        throw;
        return content;
    }
};


class SCBeautifulSoup
{
private:
    std::vector<std::string> getKeyWords( const std::vector<std::pair<std::string, std::string>>& pairs )
    {
        std::vector<std::string> keys( pairs.size() );
        size_t i = 0;
        for( const std::pair<std::string, std::string>& pair : pairs )
        {
            keys[i++] = ( pair.first + "=\"" + pair.second + "\"" );
        }
        return keys;
    }

    std::pair<std::string, size_t> getBlockTitle( const std::string& content, const size_t& start )
    {
        std::string returnMessage = "";
        size_t i = start;
        for( ; i < content.size(); )
        {
            returnMessage += content[i];  // TODO: переделать
            if( content[i++] == '>' )
                break;
        }
        return std::pair<std::string, size_t>( returnMessage, i );
    }

    bool isKeysNpos( const std::string& head, const std::vector<std::string>& keys )
    {
        for( const std::string& key : keys )
        {
            if( head.find( key ) == std::string::npos )
                return true;
        }
        return false;
    }

    size_t findEndObject( const std::string& content, const size_t& start,
                          const std::string& tL, const std::string& tR )
    {
        size_t i = start;

        size_t count_object_open = 1;
        size_t count_object_end = 0;

        size_t posNextObject = 0;
        size_t posEndCurrent = 0;

        for( size_t I = content.size(); i < I; )
        {
            posNextObject = content.find( tL, i );
            posEndCurrent = content.find( tR, i );

            if( posEndCurrent == std::string::npos )
                return std::string::npos;  // Ломанный файл, вернуть ноль

            if( posNextObject == std::string::npos )
                return posEndCurrent;

            if( posNextObject > posEndCurrent )
            {
#if DEBUG
                std::cout << posNextObject << '>' << posEndCurrent << std::endl;
#endif
                i = ( posEndCurrent + 1 );
                ++count_object_end;
            }
            else
            {
#if DEBUG
                std::cout << posNextObject << '<' << posEndCurrent << std::endl;
#endif
                i = ( posNextObject + 1 );
                ++count_object_open;
            }

            if( count_object_open == count_object_end )
                return posEndCurrent;
        }
    
        return std::string::npos;
    }

public:
    SCBeautifulSoup() = default;
    SCBeautifulSoup(int) = delete;
    SCBeautifulSoup( const SCBeautifulSoup& ) = delete;
    SCBeautifulSoup( SCBeautifulSoup&& ) = delete;
    ~SCBeautifulSoup() = default;

    std::string parseHref
    (
        const std::string& content,
        const std::string& object,
        const std::vector<std::pair<std::string, std::string>> pairs
    )
    {
        // Триггеры начала и конца блока
        const std::string tL = ( "<" + object );
        const std::string tR = ( "</" + object + ">" );

        // Ключевые слова
        const std::vector<std::string> keys = getKeyWords( pairs );
        std::pair<std::string, size_t> block{ "", 0 };

        while( ( block.second = content.find( tL, block.second ) ) != std::string::npos )
        {
            block = getBlockTitle( content, block.second );

            if( !isKeysNpos( block.first, keys ) )
            {
#if DEBUG
                std::cout << block.first << std::endl;
#endif
                size_t posEndCurrent = findEndObject( content, block.second, tL, tR );
                if( posEndCurrent == std::string::npos )
                    return std::string("");
                return content.substr( block.second, ( posEndCurrent - block.second ) );
            }
        }
        return std::string("");
    }
};

RequestCurl request;
SCBeautifulSoup scbs;
/*
План парсинга такой, у нас есть большой скрин всей страницы размер N, извлекаем из него маленький блок
размера M, где M << N и проходимся по нему мелким поиском. В случае когда мы не находим ничего empty = 1.


*/
int main()
{
    std::string content = request.GET(URL);
    //std::cout << parseHref( content, "div", { { "id", "fullannotation" } } ) << std::endl;
    //std::cout << parseHref( content, "span", { { "class", "buying-pricenew-val-number" } } ) << std::endl;
    //std::cout << parseHref( content, "div", { { "id", "product-image" } } ) << std::endl;
    std::cout << scbs.parseHref( content, "div", { { "id", "product" } } ) << std::endl;
//    std::cout << parseHref( content, "", { { "", "" } } ) << std::endl;

    return 0;
}



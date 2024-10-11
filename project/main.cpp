#include <iostream>
#include <chrono>
#include <string>
#include <regex>

#include <curl/curl.h>

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

std::string parseHref(
        const std::string& content,
        const std::string& object,
        const std::vector<std::pair<std::string, std::string>> pairs
    )
{
    const std::string triggerLeft = ( "<" + object );
    const std::string triggerRight = ( "</" + object + ">" );
    const std::vector<std::string> keys = getKeyWords( pairs );  // ( _code + "=\"" + object + "\"" );

    std::string message = "";
#if 1
    size_t count = 0;
    size_t i = 0;
    size_t j = 0;

    size_t posNextDiv = 0;
    size_t posEndCurrent = 0;

    while( ( i = content.find( triggerLeft, i ) ) != std::string::npos )
    {
        auto block = getBlockTitle( content, i );
        i = block.second;

        if( isKeysNpos( block.first, keys ) )
            continue;

        //std::cout << block.first << std::endl;
        j = i;

        for( size_t J  = content.size(); j < J; j = posEndCurrent )
        {
            posNextDiv = content.find( triggerLeft, j );
            posEndCurrent = content.find( triggerRight, j );

            if( posEndCurrent == std::string::npos )
                throw;  // Ломанный файл

            if( ( posNextDiv == std::string::npos ) or ( posNextDiv > posEndCurrent ) )
                break;
        }

        message = content.substr( i, ( posEndCurrent - i ) );
        break;
    }
#endif
    return message;
}

RequestCurl request;

int main()
{
    std::string content = request.GET(URL);
    //std::cout << parseHref( content, "div", { { "id", "fullannotation" } } ) << std::endl;
    //std::cout << parseHref( content, "span", { { "class", "buying-pricenew-val-number" } } ) << std::endl;
    std::cout << parseHref( content, "div", { { "id", "product-image" } } ) << std::endl;
//    std::cout << parseHref( content, "", { { "", "" } } ) << std::endl;
    return 0;
}



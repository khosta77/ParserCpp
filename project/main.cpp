#include <iostream>
#include <chrono>
#include <string>
#include <regex>

#include <curl/curl.h>

//const std::string URL = "https://ru.investing.com/indices/us-spx-500";
const std::string URL = "https://www.labirint.ru/books/877234/";
//const std::string URL = "https://ru.investing.com/search";

class Request
{
private:
    CURL *curl;
    std::string content;

    // Функция обратного вызова для записи данных, полученных от curl
    static size_t WriteCallback( void* contents, size_t size, size_t nmemb, std::string* output )
    {
        size_t total_size = size * nmemb;
        output->append(static_cast<char*>(contents), total_size);
        return total_size;
    }

public:
    Request() : content("")
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
    }

    ~Request()
    {
        curl_global_cleanup();
    }
    
    Request( const Request& rhs ) = delete;
    Request( Request&& rhs ) = delete;
    Request& operator=( const Request& rhs ) = delete;
    Request& operator=( Request&& rhs ) = delete;

    std::string GET( const std::string& url )
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


//<a class="js-inner-all-results-quote-item row" href="/equities/mts_rts">
//  <span class="flag first"><i class="ceFlags middle Russian_Federation"></i></span>
//  <span class="second">MTSS</span>
//  <span class="third">Мобильные ТелеСистемы ПАО</span>
//  <span class="fourth">Aкция - Москва equities</span>
//</a>

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

std::string parseHref( const std::string& content, const std::string& _class = "div",
        const std::string& _code = "id", const std::string _value = "fullannotation" )
{
    const std::string triggerLeft = ( "<" + _class );
    const std::string triggerRight = ( "</" + _class + ">" );
    const std::string key = ( _code + "=\"" + _value + "\"" );
    std::
    std::string message = "";
    size_t count = 0;
    size_t i = 0, j = 0;
    while( ( i = content.find( triggerLeft, i ) ) != std::string::npos )
    {
        auto block = getBlockTitle( content, i );
        i = block.second;

        if( block.first.find( key ) == std::string::npos )
            continue;

        std::cout << block.first << std::endl;
    }
/*
    for( size_t i = 0; i < content.size(); ++i )
    {
        if( content[i] =='<' )
        {
            auto block = getBlockTitle( content, i );
            std::cout << block.first << std::endl;
            i = block.second;

            if( block.first.find( triggerLeft ) != std::string::npos )
            {
                if( message.empty() )
                    continue;
                
                message += block.first;
                --count;
                if( count == 0 )
                    break;
            }

            if( block.first.find( triggerLeft ) == std::string::npos )
            {
                if( !message.empty() )
                    message += block.first;
                continue;
            }
            
            if( block.first.find( key ) == std::string::npos )
            {
                if( !message.empty() )
                    message += block.first;
                continue;
            }

            if( !message.empty() )
                ++count;

            message += block.first;
        }

        if( !message.empty() )
            message += content[i];
    }
*/
    return message;
#if 0
    std::regex regex( R"(<div[^>]*id=["']fullannotation["'][^>]*>(.*?)</div>)" );
    std::smatch match;

    if( std::regex_search(content, match, regex) && match.size() > 1 )
	{
        std::string href = match[0].str();
        return href;
    }
    throw;
#endif
}

float parsePrice( const std::string& content )
{
    std::regex regex( R"(<div[^>]*data-test=["']instrument-price-last["'][^>]*>(.*?)</div>)" );
    std::smatch match;

    if( std::regex_search(content, match, regex) && match.size() > 1 )
	{
        std::string priceStr = match[1].str();
        priceStr.erase( std::remove( priceStr.begin(), priceStr.end(), '.' ), priceStr.end() );
        std::replace( priceStr.begin(), priceStr.end(), ',', '.' );
        return std::stof(priceStr);
    }
    throw;
}

Request request;

int main()
{
    std::string content = request.GET(URL);
    std::cout << parseHref(content) << std::endl;
    //std::string href = parseHref(content);
    //float price = parsePrice(content);
	//std::cout << href << std::endl;
	return 0;
}



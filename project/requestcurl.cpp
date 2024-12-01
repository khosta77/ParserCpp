#include "requestcurl.hpp"

RequestCurl::~RequestCurl()
{
    content.clear();
    curl_global_cleanup();
}

// Функция обратного вызова для записи данных, полученных от curl
static size_t WriteCallback( void* contents, size_t size, size_t nmemb, std::string* output )
{
    size_t total_size = ( size * nmemb );
    output->append( static_cast<char*>(contents), total_size );
    return total_size;
}

std::string RequestCurl::GET( const std::string& url, const std::vector<std::string>& headersVector ) 
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl)
    {
        content.clear();
        curl_easy_setopt( curl, CURLOPT_URL, url.c_str() );

        if( !headersVector.empty() )
        {
            struct curl_slist* headers = nullptr;
            for( const std::string& header : headersVector )
                headers = curl_slist_append( headers, header.c_str() );
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }

        curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, WriteCallback );
        curl_easy_setopt( curl, CURLOPT_WRITEDATA, &content );

        if( CURLcode res = curl_easy_perform(curl); res != CURLE_OK )
        {    // not good!!!
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            throw std::out_of_range("Выход за пределы curl!!!");    
        }

        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return content;
    }

    curl_global_cleanup();
    content.clear();
    std::cerr << "curl_easy_perform() failed: somebroblem" << std::endl;
    throw std::out_of_range("Выход за пределы curl!!!");
    return content;
}



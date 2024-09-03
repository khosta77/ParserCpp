#include <iostream>
#include <curl/curl.h>
//#include <gsoup.h>

const std::string url = "https://ru.investing.com/indices/mcx";

// Функция обратного вызова для записи данных, полученных от curl
size_t WriteCallback( void* contents, size_t size, size_t nmemb, std::string* output )
{
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}


int main()
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl)
    {
        curl_easy_setopt( curl, CURLOPT_URL, url.c_str() );

        // Установка функции обратного вызова для записи данных
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if( res != CURLE_OK )
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        else
        {
            std::cout << readBuffer << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    

    return 0;
}



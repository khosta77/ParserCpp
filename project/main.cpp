#include <iostream>
#include <curl/curl.h>
//#include <gsoup.h>
//#include <gsoap/soapH.h> // Замени на актуальный заголовочный файл gSOAP
#include <gsoap/soaphttp.h>
#include <gsoap/sa.h>

const std::string url = "https://ru.investing.com/indices/mcx";

// Функция обратного вызова для записи данных, полученных от curl
size_t WriteCallback( void* contents, size_t size, size_t nmemb, std::string* output )
{
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

std::string get_div_content(const std::string& html, const std::string& div_id) {
    size_t start = html.find("<div id=" + div_id + ">");
    if (start == std::string::npos) return "";

    start += 19 + div_id.length(); // Длина "<div id=""
    size_t end = html.find("</div>", start);

    if (end == std::string::npos) return "";

    return html.substr(start, end - start);
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
            //std::cout << readBuffer << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

#if 1
    std::string div_content = get_div_content(readBuffer, "myDiv");

    std::cout << "Content of div: " << div_content << std::endl;

#endif

    return 0;
}



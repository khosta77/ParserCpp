#include <iostream>
#include <chrono>
#include <string>
#include <regex>

#include <curl/curl.h>

const std::string url = "https://ru.investing.com/indices/mcx";

// Функция обратного вызова для записи данных, полученных от curl
size_t WriteCallback( void* contents, size_t size, size_t nmemb, std::string* output )
{
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

float parsePrice(const std::string& content)
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

int main()
{
	auto startAll = std::chrono::steady_clock::now();

    CURL *curl;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
	auto startCurl = std::chrono::steady_clock::now();

    if(curl)
    {
        curl_easy_setopt( curl, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        if( CURLcode res = curl_easy_perform(curl); res != CURLE_OK )
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    }
    auto endCurl = std::chrono::steady_clock::now();

	auto startParse = std::chrono::steady_clock::now();
    float price = parsePrice(readBuffer);
    auto endAll = std::chrono::steady_clock::now();
    std::chrono::duration<double> diffAll = endAll - startAll;
    std::chrono::duration<double> diffCurl = endCurl - startCurl;
    std::chrono::duration<double> diffParse = endAll - startParse;
    
    std::cout << "Time taken: " << diffCurl.count() << " seconds" << std::endl;
    std::cout << "Time taken: " << diffParse.count() << " seconds" << std::endl;
    std::cout << "Time taken: " << diffAll.count() << " seconds" << std::endl;
	std::cout << price << std::endl;
    curl_easy_cleanup(curl);
    curl_global_cleanup();
	return 0;
}



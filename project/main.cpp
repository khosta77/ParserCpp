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

#if 0
namespace beast = boost::beast;          // изолируем экспозицию boost::beast
namespace http = beast::http;            // изолируем экспозицию boost::beast::http
namespace net = boost::asio;              // изолируем экспозицию boost::asio
using tcp = net::ip::tcp;                // изолируем экспозицию boost::asio::ip::tcp

// Функция для выполнения GET запроса
std::string http_get(const std::string& url) {
    // Разделение URL на компоненты
    auto const pos = url.find("://");
    if (pos == std::string::npos) {
        throw std::runtime_error("Invalid URL: " + url);
    }

    // Получаем протокол и сдвигаем указатель после "://"
    std::string protocol = url.substr(0, pos);
    std::string remaining_url = url.substr(pos + 3);

    // Разделяем оставшуюся часть на имя хоста и путь
    auto const path_start = remaining_url.find('/');
    std::string host = (path_start == std::string::npos) ? remaining_url : remaining_url.substr(0, path_start);
    std::string path = (path_start == std::string::npos) ? "/" : remaining_url.substr(path_start);

    // Создаем контекст io и сокет
    net::io_context io_context;

    // Разрешаем имя хоста
    tcp::resolver resolver(io_context);
    auto const results = resolver.resolve(host, protocol == "https" ? "443" : "80");

    // Создаем и заполняем HTTP-запрос
    beast::tcp_stream stream(io_context);
    net::connect(stream.socket(), results.begin(), results.end());

    // Если используем https, оборачиваем в secure stream
    if (protocol == "https") {
        stream = beast::ssl_stream<beast::tcp_stream>(std::move(stream), boost::asio::ssl::context(boost::asio::ssl::context::sslv23));
        boost::asio::ssl::stream_base::handshake_type handshake_type = boost::asio::ssl::stream_base::client;
        static_cast<beast::ssl_stream<beast::tcp_stream>&>(stream).handshake(handshake_type);
    }

    // Составляем запрос
    http::request<http::string_body> req(http::verb::get, path, 11);
    req.set(http::field::host, host);
    req.set(http::field::user_agent, "Boost.Beast/1.81.0"); // Укажите вашу версию Boost.Beast

    // Отправляем запрос
    http::write(stream, req);

    // Получаем ответ
    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);

    // Закрываем соединение
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    // Если ошибка, выводим её
    if (ec && ec != beast::errc::not_connected) {
        throw beast::system_error{ec};
    }

    // Возвращаем ответ
    return res.body();
}
#endif


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
    return 0;
}



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
    virtual std::string GET( const std::string&, const std::vector<std::string>& ) = 0;
};

class RequestCurl : public Request
{
private:
    CURL *curl;
    std::string content;

    // Функция обратного вызова для записи данных, полученных от curl
    static size_t WriteCallback( void* contents, size_t size, size_t nmemb, std::string* output )
    {
        size_t total_size = size * nmemb;  // TODO: можно переделать
        output->append( static_cast<char*>(contents), total_size );
        return total_size;
    }

public:
    RequestCurl() : content("") {}

    ~RequestCurl()
    {
        curl_global_cleanup();
    }
    
    std::string GET( const std::string& url, const std::vector<std::string>& headersVector = {} ) override
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
                {
                    //std::cout << header << std::endl;
                    headers = curl_slist_append( headers, header.c_str() );
                }
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            }

            curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, WriteCallback );
            curl_easy_setopt( curl, CURLOPT_WRITEDATA, &content );

            if( CURLcode res = curl_easy_perform(curl); res != CURLE_OK )
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

            curl_easy_cleanup(curl);
            curl_global_cleanup();
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

    std::pair<std::string, size_t> parseHref
    (
        const std::string& content,
        const std::string& object,
        const std::vector<std::pair<std::string, std::string>> pairs,
        const size_t& startPosition = 0
    )
    {
        // Триггеры начала и конца блока
        const std::string tL = ( "<" + object );
        const std::string tR = ( "</" + object + ">" );

        // Ключевые слова
        const std::vector<std::string> keys = getKeyWords( pairs );
        std::pair<std::string, size_t> block{ "", startPosition };

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
                    return { std::string(""), -1 };
                return { content.substr( block.second, ( posEndCurrent - block.second ) ), posEndCurrent };
            }
        }
        return { std::string(""), -1 };
    }
};

RequestCurl request;
SCBeautifulSoup scbs;
/*
План парсинга такой, у нас есть большой скрин всей страницы размер N, извлекаем из него маленький блок
размера M, где M << N и проходимся по нему мелким поиском. В случае когда мы не находим ничего empty = 1.


*/
class Parser
{
public:
    static std::string unpack( const std::string& content )
    {
        // TODO: Переделать на stringstream
        std::string text;
        bool isMark = false;
        for( auto symbol : content )
        {
            if( symbol == '<' )
            {
                isMark = true;
                continue;
            }

            if( symbol == '>' )
            {
                isMark = false;
                continue;
            }

            if( isMark )
                continue;   

            text += symbol;
        }
        return text;
    }
};


struct LabyrinthPage
{
    //// Получаем сразу
    int id;                   // Id книги он же в ссылке справа
    
    //// Первое, что извлекаем
    std::string typeObject;   // Объект: книга, игрушка и т. д.
    std::string groupOfType;  // Типы объекта: Нехудож литер, игрушка и т д
    std::string underGroup;   // Подгруппа типа: Информ технологии и т д
    std::string genres;       // Жанр детально внутренний

    //// Обработка описания
    std::string bookName;       // Название книги
    
    std::string authors;  // Авторы
    std::string publisher; // Издатель
    std::string datePublisher;  // Дата издания
    std::string series;
    std::string bookGenres;  // Жанр книг

    float allPrice;     // цена для всех
    float myPrice;      // моя цена
    float sale;         // скидка
    
    std::string isbn;  // ISBN: 978-5-9693-0549-6
    std::string pages;
    std::string pageType;  // Оффсет, бумага и т. д.

    // ajax/design
    std::string box;    // Тип упаковки
    std::string covers;   // Тип обложки: 7Б - твердая (плотная бумага или картон)
    std::string decoration;   // Оформление: Тиснение объемное
    std::string illustrations;  // Иллюстрации: Черно-белые + цветные

    std::string weight;  // Масса
    int da;  // Размеры
    int db;
    int dc;
    
    std::string rate;
    std::string rateSize;
    std::string annotation;

    std::string imgUrl;

    // RtB Five reasons to buy
    std::string RtB1;
    std::string RtB2;
    std::string RtB3;
    std::string RtB4;
    std::string RtB5;


    LabyrinthPage( [[maybe_unused]] const int& idIn,
                   [[maybe_unused]] const std::string& product,
                   [[maybe_unused]] const std::string& popup ) : id(idIn)
    {
        toExtractObject( product );
    }

    ~LabyrinthPage() 
    {
        typeObject.clear();
        groupOfType.clear();
        underGroup.clear();
        genres.clear();

    }
private:
    void toExtractObject( const std::string& product )
    {
        std::pair<std::string, size_t> label;
        label = scbs.parseHref( product, "a", { { "itemprop", "item" } } );
        typeObject = Parser::unpack(label.first);
        //std::cout << typeObject << ';' << groupOfType << ';' << underGroup << ';' << genres << std::endl;
        if( label.first.empty() )
        {
            groupOfType = "";
            underGroup = "";
            genres = "";
            return;
        }

        label = scbs.parseHref( product, "a", { { "itemprop", "item" } }, label.second );
        groupOfType = Parser::unpack(label.first);
        //std::cout << typeObject << ';' << groupOfType << ';' << underGroup << ';' << genres << std::endl;
        if( label.first.empty() )
        {
            underGroup = "";
            genres = "";
            return;
        }

        label = scbs.parseHref( product, "a", { { "itemprop", "item" } }, label.second  );
        underGroup = Parser::unpack(label.first);
        //std::cout << typeObject << ';' << groupOfType << ';' << underGroup << ';' << genres << std::endl;
        if( label.first.empty() )
        {
            genres = "";
            return;
        }

        label = scbs.parseHref( product, "a", { { "itemprop", "item" } }, label.second  );
        genres = Parser::unpack(label.first);
        //std::cout << typeObject << ';' << groupOfType << ';' << underGroup << ';' << genres << std::endl;
    }
};

const std::vector<std::string> HEADERS = {
    "X-Requested-With: XMLHttpRequest",
    "Referer: https://www.labirint.ru/books/0/"
};

int main()
{
    const int id = 877234;
    const std::string page = request.GET(URL);
    const std::string project = scbs.parseHref( page, "div", { { "id", "product" } } ).first;
    const std::string popup = request.GET( "https://www.labirint.ru/ajax/design/877234/", HEADERS );
    LabyrinthPage( id, project, popup );
    //std::cout << parseHref( content, "div", { { "id", "fullannotation" } } ) << std::endl;
    //std::cout << parseHref( content, "span", { { "class", "buying-pricenew-val-number" } } ) << std::endl;
    //std::cout << parseHref( content, "div", { { "id", "product-image" } } ) << std::endl;
//    std::cout << scbs.parseHref( content, "div", { { "id", "product" } } ) << std::endl;
//    std::cout << parseHref( content, "", { { "", "" } } ) << std::endl;
    //std::cout << request.GET("https://www.labirint.ru/ajax/design/781041/") << std::endl;
    //std::cout << scbs.parseHref( content, "div", { { "class", "popup" } } );
    //std::cout << request.GET( "https://www.labirint.ru/ajax/design/877234/", HEADERS );
    return 0;
}



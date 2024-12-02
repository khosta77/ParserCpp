#ifndef PROJECT_LABIRINT_PAGE_HPP_
#define PROJECT_LABIRINT_PAGE_HPP_

#include "requestcurl.hpp"
#include "parser.hpp"
#include "soup.hpp"

/*
План парсинга такой, у нас есть большой скрин всей страницы размер N, извлекаем из него маленький блок
размера M, где M << N и проходимся по нему мелким поиском. В случае когда мы не находим ничего empty = 1.
*/
class LabyrinthPage
{
private:
    Request* _request_;
    SCBeautifulSoup _scbs_;
    Parser _parser_;
    const std::string _totatUrl;
    const std::vector<std::string> _headers;

    //// Получаем сразу
    int id                     = -1;     // + Id книги он же в ссылке справа
    
    //// Первое, что извлекаем
    std::string typeObject     = "";     // + Объект: книга, игрушка и т. д.
    std::string groupOfType    = "";     // + Типы объекта: Нехудож литер, игрушка и т д
    std::string underGroup     = "";     // + Подгруппа типа: Информ технологии и т д
    std::string genres         = "";     // + Жанр детально внутренний

    //// Обработка описания
    std::string bookName       = "";     // + Название книги
    std::string imgUrl         = "";     // +

    //// Description
    std::string age            = "";     // +
    // Блять да какого.... У этих пидоров еще редактор пишется, я когда отлаживал мне книги
    // без редакторов попадались, в проде выяснил, что он есть, пиздос. Это надо сильно вверх
    // поднтся, короче в жопу редактора  // комментарий сделан в 02:22 ночи 
    std::string authors        = "";     // + Авторы
    std::string publisher      = "";     // + Издатель
    int datePublisher          = -1;     // + Дата издания
    std::string series         = "";     // +
    std::string bookGenres     = "";     // + Жанр книг
    float allPrice             = -1.0f;  // + цена для всех
    float myPrice              = -1.0f;  // + моя цена
    float sale                 = -1.0f;  // + скидка
    std::string isbn           = "";     // + ISBN: 978-5-9693-0549-6
    std::string pages          = "";     // +
    std::string pageType       = "";     // + Оффсет, бумага и т. д.
    std::string weight         = "";     // + Масса
    int da                     = -1;     // + Размеры
    int db                     = -1;     // +
    int dc                     = -1;     // +

    //// ajax/design
    std::string box            = "";     // + Тип упаковки
    std::string covers         = "";     // + Тип обложки: 7Б - твердая (плотная бумага или картон)
    std::string decoration     = "";     // + Оформление: Тиснение объемное
    std::string illustrations  = "";     // + Иллюстрации: Черно-белые + цветные

    //// rate
    float rate                 = -1.0f;  // +
    int rateSize               = -1;     // +
    std::string annotation     = "";     // +

public:
    LabyrinthPage( Request* req, SCBeautifulSoup &scbc_, Parser& parser_,
            const std::string& url, const std::vector<std::string> headers ) : _request_(req),
        _scbs_(scbc_), _parser_(parser_), _totatUrl(url), _headers(headers) {}

    ~LabyrinthPage();

    friend std::ostream& operator<<( std::ostream&, const LabyrinthPage& );

    bool operator()( const int& );

    void clear();

private:
    bool toExtractAnnotation( const std::string& );
    void toExtractObject( const std::string& );
    void toExtractBookName( const std::string& );
    void toExtractImgUrl();
    void toExtractDescription( const std::string& );
    void toExtractBookDescription( const std::string& );
    void _toExtractBox( const std::string&, const size_t& );
    void _toExtractCovers( const std::string&, const size_t& );
    void _toExtractDecoration( const std::string&, const size_t& );
    void _toExtractIllustrations( const std::string&, const size_t& );
    void toExtractRate( const std::string& );
};

std::ostream& operator<<( std::ostream&, const LabyrinthPage& );

#endif  // PROJECT_LABIRINT_PAGE_HPP_

#include "page.hpp"

LabyrinthPage::~LabyrinthPage()
{
    clear();
}

std::ostream& operator<<( std::ostream& os, const LabyrinthPage& lp )
{
    const char del = ';';
    os << lp.id << del << lp.typeObject << del << lp.groupOfType << del << lp.underGroup << del
       << lp.genres << del << lp.bookName << del << lp.imgUrl << del
       << lp.age << del << lp.authors << del << lp.publisher << del << lp.datePublisher << del
       << lp.series << del << lp.bookGenres << del << lp.allPrice << del << lp.myPrice << del
       << lp.sale << del << lp.isbn << del << lp.pages << del << lp.pageType << del << lp.weight << del
       << lp.da << del << lp.db << del << lp.dc << del << lp.box << del << lp.covers << del
       << lp.decoration << del << lp.illustrations << del << lp.rate << del << lp.rateSize << del
       << lp.annotation << '\n';
    return os;
}

bool LabyrinthPage::operator()( const int& i )
{
    clear();

    // Очень медленная функция, она является узким местом
    std::string page = _request_->GET(
            ( _totatUrl + std::to_string(i) + "/" ),
            std::vector<std::string>()
        );

    if( page.find("Страница, которую вы ищете, затерялась в Лабиринте :(") != std::string::npos)
    {
        page.clear();
        return false;
    }

    std::pair<std::string, size_t> buffer = _scbs_.parseHref( page, "div", { { "id", "product" } } );
    if( buffer.second == std::string::npos )
    {
        page.clear();
        buffer.first.clear();
        return false;
    }
    std::string product = std::move(buffer.first);
    
    std::string popup = _request_->GET(
            ( "https://www.labirint.ru/ajax/design/" + std::to_string(i)  + "/" ),
            _headers
        );

    if( !toExtractAnnotation( product ) )
    {
        page.clear();
        product.clear();
        return false;
    }
    id = i;
    toExtractObject( product );
    toExtractBookName( product );
    toExtractImgUrl();
    toExtractDescription( product );
    if( popup.find("Страница, которую вы ищете, затерялась в Лабиринте :(") == std::string::npos )
        toExtractBookDescription( popup );
    toExtractRate( product );

    page.clear();
    product.clear();
    popup.clear();
    return true;
}

void LabyrinthPage::clear()
{
    typeObject.clear();
    groupOfType.clear();
    underGroup.clear();
    genres.clear();
    bookName.clear();
    imgUrl.clear();
    age.clear();
    authors.clear();
    publisher.clear();
    series.clear();
    bookGenres.clear();
    isbn.clear();
    pages.clear();
    pageType.clear();
    weight.clear();
    box.clear();
    covers.clear();
    decoration.clear();
    illustrations.clear();
    annotation.clear();

    id = -1;
    allPrice = -1.0;
    datePublisher = -1;
    myPrice = -1.0;
    sale = -1.0;
    da = -1;
    db = -1;
    dc = -1;
    rate = -1.0;
    rateSize = -1.0;
}


void LabyrinthPage::toExtractObject( const std::string& product )
{
    std::pair<std::string, size_t> label;
    label = _scbs_.parseHref( product, "a", { { "itemprop", "item" } } );
    typeObject = _parser_.unpack(label.first);
    if( label.first.empty() )
        return;

    label = _scbs_.parseHref( product, "a", { { "itemprop", "item" } }, label.second );
    groupOfType = _parser_.unpack(label.first);
    if( label.first.empty() )
        return;

    label = _scbs_.parseHref( product, "a", { { "itemprop", "item" } }, label.second  );
    underGroup = _parser_.unpack(label.first);
    if( label.first.empty() )
        return;

    label = _scbs_.parseHref( product, "a", { { "itemprop", "item" } }, label.second  );
    genres = _parser_.unpack(label.first);
}

void LabyrinthPage::toExtractBookName( const std::string& product )
{
    std::string _bookName = _scbs_.parseHref( product, "div", { { "id", "product-title" } } ).first;
    if( _bookName.empty() )
        return;

    _bookName = _scbs_.parseHref( _bookName, "h1", {} ).first;
    if( _bookName.empty() )
        return;

    size_t i = _bookName.find(':');
    if( i != std::string::npos )
    {
        bookName = _bookName.substr( ( i + 2 ) );
        return;
    }
    bookName = _bookName;
}

void LabyrinthPage::toExtractImgUrl()
{
    imgUrl = "https://static10.labirint.ru/books/" + std::to_string( id ) + "/cover.jpg";
}

void LabyrinthPage::toExtractDescription( const std::string& product )
{
    std::string _description  = _scbs_.parseHref( product, "div", { { "class",
                                                                      "product-description" } } ).first;
    if( _description.empty() )
        return;
    std::pair<std::string, size_t> _b;
    size_t start = 0, end = 0;
    age = _scbs_.parseHref( _description, "div", { { "id", "age_dopusk" } } ).first;
    authors = _scbs_.parseHref( _description, "a", { { "data-event-label", "author" } } ).first;
    _b = _scbs_.parseHref( _description, "a", { { "data-event-label", "publisher" } } );
    
    // Вроде здесь, а вроде и нет возникает ошибка с конвертацией строки. См логи...
    if( _b.second != std::string::npos )
    {
        publisher = _b.first;
        datePublisher = std::stoi( _description.substr( ( _b.second + 6 ), 4 ) );
    }

    series = _scbs_.parseHref( _description, "a", { { "data-event-label", "series" } } ).first;
    bookGenres = _scbs_.parseHref( _description, "a", { { "data-event-label", "genre" } } ).first;

    _b = _scbs_.parseHref( _description, "span", { { "class", "buying-priceold-val-number" } } );
    allPrice = ( ( _b.second != std::string::npos ) ? std::stof(std::move(_b.first)) : 0.0 );
    _b = _scbs_.parseHref( _description, "span", { { "class", "buying-pricenew-val-number" } } );
    myPrice = ( ( _b.second != std::string::npos ) ? std::stof(std::move(_b.first)) : 0.0 );
    sale = ( ( allPrice != 0.0 ) ? ( myPrice / allPrice * 100.0 ) : 0.0 );

    _b = _scbs_.parseHref( _description, "div", { { "class", "isbn" } } );
    if( _b.second != std::string::npos )
    {
        start = 6;
        end = _b.first.find("&");
        isbn = ( ( end == std::string::npos ) ? _b.first.substr( start ) : _b.first.substr( start,
                                                                                ( end - start ) ) );
    }

    _b = _scbs_.parseHref( _description, "div", { { "class", "pages2" } } );
    if( _b.second != std::string::npos )
    {
        std::string _pages = std::move(_b.first);
        const std::string pages_name = "Страниц: ";
        size_t start_x = ( _pages.find( pages_name ) + pages_name.size() );
        start = _pages.find('(');
        if( start != std::string::npos )
        {
            end = ( _pages.find(')') - start );
            pageType = _pages.substr( start + 1, end - 1 );
        }
        else
        {
            start = ( _pages.find(' ', start_x ) + 1 );
        }
        pages = _pages.substr( start_x, ( start - start_x - 1 ) );
    }

    _b = _scbs_.parseHref( _description, "div", { { "class", "weight" } } );
    if( _b.second != std::string::npos )
    {
        start = 12;
        end = _b.first.find(" г");
        weight = _b.first.substr( start, ( end - start ) );
    }
    std::string _d = _scbs_.parseHref( _description, "div", { { "class", "dimensions" } } ).first;
    start = (_d.find(' ') + 1);
    end = _d.find( ' ', start );
    std::vector<std::string> dABC = _parser_.split( _d.substr( start, end - start ), 'x' );
    if( dABC.size() == 3 )
    {
        da = std::stoi(dABC[0]);
        db = std::stoi(dABC[1]);
        dc = std::stoi(dABC[2]);
    }
}

void LabyrinthPage::_toExtractBox( const std::string& title, const size_t& start )
{
    const size_t end = title.find("</div>");
    box = title.substr( start - 1, ( end - start + 1) );
}

void LabyrinthPage::_toExtractCovers( const std::string& title, const size_t& start  )
{
    const size_t end = title.find("</div>");
    covers = title.substr( start - 1, ( end - start + 1 ) );
}

void LabyrinthPage::_toExtractDecoration( const std::string& title, const size_t& start  )
{
    const size_t end = title.find("</div>");
    decoration = title.substr( start - 1, ( end - start + 1 ) );
}

void LabyrinthPage::_toExtractIllustrations( const std::string& title, const size_t& start  )
{
    const size_t end = title.find("</div>");
    illustrations = title.substr( start, ( end - start ) );
}

void LabyrinthPage::toExtractBookDescription( const std::string& popup )
{
    const std::vector<std::string> roll = {
        "Тип обложки: ",
        "Оформление: ",
        "Иллюстрации: ",
        "Тип упаковки: "
    };
    std::vector<std::string> frames = _parser_.split( popup, "<div>" );
    for( const std::string& frame : frames )
    {
        if( size_t s = frame.find( roll[0] ); s != std::string::npos )
            _toExtractCovers( frame, ( s + roll[0].size() ) );
        else if( size_t s = frame.find( roll[1] ); s != std::string::npos )
            _toExtractDecoration( frame, ( s + roll[0].size() ) );
        else if( size_t s = frame.find( roll[2] ); s != std::string::npos )
            _toExtractIllustrations( frame, ( s + roll[0].size() ) );
        else if( size_t s = frame.find( roll[3] ); s != std::string::npos )
            _toExtractBox( frame, ( s + roll[0].size() ) );
    }
}

void LabyrinthPage::toExtractRate( const std::string& product )
{
    std::pair<std::string, size_t> _b;

    _b = _scbs_.parseHref( product, "div", { { "id", "rate" } } );
    rate = ( ( _b.second != std::string::npos ) ? std::stof(std::move(_b.first)) : 0.0 );

    _b = _scbs_.parseHref( product, "div", { { "id", "product-rating-marks-label" } } );
    if( _b.second != std::string::npos )
    {
        size_t start = ( _b.first.find(":") + 1 );
        size_t end = _b.first.find(")");
        rateSize = std::stoi( _b.first.substr( start, end - start ) );
    }
    else
        rateSize = 0.0;
}

static std::vector<std::string> stopWords = {
    "<br/>",
    "<noindex>",
    "</noindex>",
    "<em>",
    "</em>",
    "\n"
};

bool LabyrinthPage::toExtractAnnotation( const std::string& product )
{
    std::pair<std::string, size_t> buffer = _scbs_.parseHref( product, "div", { { "id",
                                                                                  "fullannotation" } } );
    if( buffer.second == std::string::npos )
    {
        buffer = _scbs_.parseHref( product, "div", { { "id", "product-about" } } );
    }

    if( buffer.first.empty() )
    {
        //std::cerr << std::format("\tBook {}, haven't annotation\n", id );
        return false;
    }
    
    annotation = std::move(buffer.first);
    size_t start = ( annotation.find("<p>") + 3 );
    size_t end = annotation.find("<div class=\"product-about-bubble\">");
    if( end == std::string::npos )
        end = annotation.find("</p>");
    annotation = annotation.substr( start, ( end - start ) );
    for( const std::string& stopWord : stopWords )
        _parser_.fastClear( annotation, stopWord );
    return true;
}



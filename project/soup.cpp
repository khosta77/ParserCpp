#include "soup.hpp"

std::vector<std::string> SCBeautifulSoup::getKeyWords(
        const std::vector<std::pair<std::string, std::string>>& pairs )
{
    std::vector<std::string> keys( pairs.size() );
    size_t i = 0;
    for( const std::pair<std::string, std::string>& pair : pairs )
    {
        keys[i++] = ( pair.first + "=\"" + pair.second + "\"" );
    }
    return keys;
}

std::pair<std::string, size_t> SCBeautifulSoup::getBlockTitle( const std::string& content,
        const size_t& start )
{
    std::string returnMessage = "";
    size_t i = start;
    for( ; i < content.size(); )
    {
        if( content[i++] == '>' )
        {
            returnMessage = content.substr( start, i - start );
            break;
        }
    }
    return std::pair<std::string, size_t>( returnMessage, i );
}

bool SCBeautifulSoup::isKeysNpos( const std::string& head, const std::vector<std::string>& keys )
{
    for( const std::string& key : keys )
    {
        if( head.find( key ) == std::string::npos )
            return true;
    }
    return false;
}

size_t SCBeautifulSoup::findEndObject( const std::string& content, const size_t& start,
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
            i = ( posEndCurrent + 1 );
            ++count_object_end;
        }
        else
        {
            i = ( posNextObject + 1 );
            ++count_object_open;
        }

        if( count_object_open == count_object_end )
            return posEndCurrent;
    }

    return std::string::npos;
}

std::pair<std::string, size_t> SCBeautifulSoup::parseHref( const std::string& content,
        const std::string& object, const std::vector<std::pair<std::string, std::string>> pairs,
        const size_t& startPosition )
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
        if( !isKeysNpos( block.first, keys ) and !keys.empty() )
        {
            size_t posEndCurrent = findEndObject( content, block.second, tL, tR );
            if( posEndCurrent == std::string::npos )
                return { std::string(""), -1 };
            return { content.substr( block.second, ( posEndCurrent - block.second ) ), posEndCurrent };
        }

        if( keys.empty() )
        {
            size_t posEndCurrent = findEndObject( content, block.second, tL, tR );
            if( posEndCurrent == std::string::npos )
                return { std::string(""), -1 };
            return { content.substr( block.second, ( posEndCurrent - block.second ) ), posEndCurrent };
        }
    }
    return { std::string(""), -1 };
}

std::pair<std::string, size_t> SCBeautifulSoup::parseHead( const std::string& content,
        const std::string& object, const std::vector<std::pair<std::string, std::string>> pairs,
        const size_t& startPosition )
{
    if( pairs.empty() )
        return { std::string(""), -1 };

    // Триггеры начала и конца блока
    const std::string tL = ( "<" + object );
    const std::string tR = ( "</" + object + ">" );

    // Ключевые слова
    const std::vector<std::string> keys = getKeyWords( pairs );
    std::pair<std::string, size_t> block{ "", startPosition };

    while( ( block.second = content.find( tL, block.second ) ) != std::string::npos )
    {
        block = getBlockTitle( content, block.second );

        if( !isKeysNpos( block.first, keys ) and !keys.empty() )
            return { block.first, ( block.second + block.first.size() ) };
    }
    return { std::string(""), -1 };
}



#include "parser.hpp"

std::string Parser::unpack( const std::string& content )
{
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

std::vector<std::string> Parser::split( const std::string& content, const char& del )
{
    std::vector<std::string> arrayString;
    size_t i = 0, j = 0;
    for( const size_t I = content.size(); i < I; ++i )
    {
        if (content[i] == del)
        {
            arrayString.push_back( content.substr( j, i - j ) );
            j = ( i + 1 );
        }
    }
    arrayString.push_back( content.substr( j, i - j ) );
    return arrayString;
}

std::vector<std::string> Parser::split(const std::string& content, const std::string& del)
{
    std::vector<std::string> arrayString;
    size_t i = 0, j = content.find(del);
    for( const size_t I = content.size(); i < I and j != std::string::npos; j = content.find( del, ( i + 1 ) ) )
    {
        arrayString.push_back( content.substr( i, ( j - i ) ) );
        i = j;
    }
    arrayString.push_back( content.substr( ( i + del.size() ), ( content.size() - i ) ) );
    return arrayString;
}

void Parser::fastClear( std::string& content, const std::string& del )
{
    for( size_t i = content.find(del); i != std::string::npos; i = content.find(del) )
    {
        for( size_t j = i, J = ( i + del.size() ); j < J; ++j )
            content[j] = ' ';
    }
}



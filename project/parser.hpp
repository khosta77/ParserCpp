#ifndef PROJECT_PARSER_HPP_
#define PROJECT_PARSER_HPP_

#include <string>
#include <vector>

class Parser
{
public:
    Parser() = default;
    Parser(int) = delete;
    Parser( const Parser& ) = default;
    Parser( Parser&& ) = delete;
    ~Parser() = default;

    std::string unpack( const std::string& content );

    std::vector<std::string> split( const std::string& content, const char& del );
    std::vector<std::string> split(const std::string& content, const std::string& del);

    void fastClear( std::string& content, const std::string& del );
};


#endif  // PROJECT_PARSER_HPP_

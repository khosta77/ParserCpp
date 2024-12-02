#ifndef PROJECT_REQUEST_HPP_
#define PROJECT_REQUEST_HPP_

#include <iostream>
#include <string>
#include <vector>

class Request
{
public:
    virtual ~Request() = default;
    virtual std::string GET( const std::string&, const std::vector<std::string>& ) = 0;
};

#endif  // PROJECT_REQUEST_HPP_

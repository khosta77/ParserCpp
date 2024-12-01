#ifndef PROJECT_REQUESTCURL_HPP_
#define PROJECT_REQUESTCURL_HPP_

#include "request.hpp"

#include <curl/curl.h>

class RequestCurl : public Request
{
private:
    std::string content;
    CURL *curl;

public:
    RequestCurl() : content("") {}
    RequestCurl(int) = delete;
    RequestCurl( const RequestCurl& rhs ) : content(rhs.content), curl(rhs.curl) {}
    RequestCurl(RequestCurl&&) = delete;
    RequestCurl& operator=( const RequestCurl& ) = delete;
    RequestCurl& operator=( RequestCurl&& ) = delete;
    ~RequestCurl();

    std::string GET( const std::string& url, const std::vector<std::string>& headersVector ) override;
};

#endif  // PROJECT_REQUESTCURL_HPP_

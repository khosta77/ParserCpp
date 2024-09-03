#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <vector>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <sstream>

// Key word
#define HTML     "</html>"
#define DOCUMENT "</document>"


#define TEST 1

char http[]={"http://"};
char host[]={"localhost"};
char parm[]={"/api/method.php"};


std::string getProtocol(const std::string &url) {
    std::string protocol = "";
    for (size_t i = 0; i < url.size(); ++i) {
        if (url[i] != '/' || url[i+1] != '/') {
            protocol += url[i];
        } else {
            protocol += "//";
            break;
        }
    }
    return protocol;
}

std::string getHost(std::string url) {  // переделать
    std::string host = "";
    url.replace(0, getProtocol(url).size(), "");

    for (size_t i = 0; i < url.size(); ++i) {
        if (url[i] != '/') {
            host += url[i];
        } else {
            break;
        }
    }
    return host;
}


std::string GET(const std::string &url, const bool &metadat = true) {
    std::string http = getProtocol(url);
    std::string host = getHost(url);

    char buf[( 16384  )];

    //std::string header = "";
    const char* s1 = R"foo(
GET /wiki/HTTPS HTTP/1.1
Host: ru.wikipedia.org
)foo";
    std::string header(s1);
#if 0
    header += "GET ";
    header += url;
    header += (std::string)" HTTPS/1.1" + "\r\n";
    header += (std::string)"Host: " + host + "\r\n";
    header += (std::string)"User-Agent: Mozilla/5.0 (Windows NT 6.1) Gecko/20853 Firefox 20.0" + "\r\n";  // Это мета данные браузер. Надо бы их рандомные придумать
    header += (std::string)"Accept: */*" + "\r\n";
    header += (std::string)"Cookie: GeoIP=RU:MOW:Moscow:55.75:37.62:v4; WMF-Last-Access-Global=20-Aug-2024; NetworkProbeLimit=0.001; WMF-Last-Access=20-Aug-2024\r\n";
    //header += (std::string)"Accept-Language: ru,en-us;q=0.7,en;q=0.3" + "\r\n";
    //header += (std::string)"Accept-Charset: windows-1251,utf-8;q=0.7,*;q=0.7" + "\r\n";
    header += (std::string)"Connection: keep-alive " + "\r\n";
    //header += (std::string)"Upgrade-Insecure-Requests: 1 " + "\r\n";
    // Со строками ниже работает не корректно
    //header += (std::string)"Content-Type: multipart/form-data; boundary=b44fbd93540" + "\r\n";
    //header += (std::string)"Content-Length: 345" + "\r\n";
    header += "\r\n";
#endif
    std::cout << header << "------------" << std::endl;
    std::cout << http << ' ' << host << std::endl;
    int sock;
    struct sockaddr_in addr;
    struct hostent* raw_host;
    raw_host = gethostbyname(host.c_str());  // ???
    if (raw_host == NULL) {
        std::cout << "ERROR, no such host";
        exit(0);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);

    bcopy((char*)raw_host->h_addr, (char*)&addr.sin_addr, raw_host->h_length);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr) ) < 0) {
        std::cerr << "connect error" << std::endl;
        exit(2);
    }

    char *message = new char[header.size()];
    for (int i = 0; i < header.size(); i++) {
        message[i] = header[i];
    }

    send(sock, message, header.size(), 0);

    std::string otvet = "";
    int rcved = recv(sock, buf, sizeof(buf), 0);
    otvet.insert(otvet.end(), &buf[0], &buf[rcved]);
#if 0
    while ((rcved = recv(sock, buf, sizeof(buf), 0)) > 0)
    {
        otvet.insert(otvet.end(), &buf[0], &buf[rcved]);
        if (otvet.substr((size_t)(0.99 * otvet.size())).find("\r\n0\r\n\r\n") != std::string::npos)  // очко
            break;
    }
#endif
    return otvet;
}



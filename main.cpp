//#include "test.h"
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

char http[]={"http://"};
char host[]={"localhost"};
char parm[]={"/api/method.php"};


std::string getProtocol( std::string url )
{
    std::string protocol = "";

    int i = 0;

    for(i = 0; i < url.size(); i++)
    {
        if( url[i] != '/' || url[i+1] != '/'  )
        {
            protocol += url[i];
        }
        else
        {
            protocol += "//";
            break;
        }
    }

    return protocol;
}

std::string getHost( std::string url )
{
    std::string host = "";

    url.replace(0, getProtocol(url).size(), "");

    int i = 0;
    for(i = 0; i < url.size(); i++)
    {

        if( url[i] != '/' )
        {
            host += url[i];
        }
        else
        {
            break;
        }

    }

    return host;
}

std::string getAction( std::string url )
{
    std::string parm = "";

    url.replace(0, getProtocol(url).size()+getHost(url).size(), "");

    int i = 0;
    for(i = 0; i < url.size(); i++)
    {

        if( url[i] != '?' && url[i] != '#' )
        {
            parm += url[i];
        }
        else
        {
            break;
        }

    }

    return parm;
}

std::string getParams( std::vector< std::pair< std::string, std::string> > requestData )
{
    std::string parm = "";

    std::vector< std::pair< std::string, std::string> >::iterator itr = requestData.begin();

    for( ; itr != requestData.end(); ++itr )
    {
        if( parm.size() < 1 )
        {
            parm += "";
        }
        else
        {
            parm += "&";
        }
        parm += itr->first + "=" + itr->second;
    }

    return parm;
}


std::string GET( std::string url, std::vector< std::pair< std::string, std::string> > requestData )
{
    std::string http = getProtocol(url);
    std::string host = getHost(url);
    std::string script = getAction(url);
    std::string parm = getParams( requestData );

    char buf[1024];

    std::string header = "";

    header += "GET ";
    header += http + host + script;
    header += (std::string)" HTTP/1.1" + "\r\n";
    header += (std::string)"Host: " + http + host + "/" + "\r\n";
    header += (std::string)"User-Agent: Mozilla/5.0" + "\r\n";
    //header += (std::string)"Accept: text/html" + "\r\n";
    header += (std::string)"Accept-Language: ru,en-us;q=0.7,en;q=0.3" + "\r\n";
    header += (std::string)"Accept-Charset: windows-1251,utf-8;q=0.7,*;q=0.7" + "\r\n";
    header += (std::string)"Connection: keep-alive " + "\r\n";
    header += "\r\n";



    int sock;
    struct sockaddr_in addr;
    struct hostent* raw_host;
    raw_host = gethostbyname( host.c_str() );
    if (raw_host == NULL)
    {
        std::cout<<"ERROR, no such host";
        exit(0);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);

    bcopy( (char*)raw_host->h_addr, (char*)&addr.sin_addr, raw_host->h_length );

    if( connect( sock, (struct sockaddr *)&addr, sizeof(addr) ) < 0)
    {
        std::cerr<<"connect error"<<std::endl;
        exit(2);
    }


    char * message = new char[ header.size() ];
    for(int i = 0; i < header.size(); i++)
    {
        message[i] = header[i];
    }

    send(sock, message, header.size(), 0);
    recv(sock, buf, sizeof(buf), 0);

    std::string answer = "";

    for(int j = 0; j < 1024; j++)
    {
            answer += buf[j];
    }

    return answer;

}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
    std::vector< std::pair< std::string, std::string> > requestData;
    std::cout << GET("https://ru.wikipedia.org", requestData) << std::endl;
    //system("ls > test.txt");
    //auto msg = GET("https://ru.wikipedia.org/wiki/HTTPS");
    //std::cout << msg<< std::endl;
#if 0
    std::string d1, d2, d3, d4;
    d1 = GET("https://iss.moex.com/iss/engines/stock/markets/shares/boards/TQBR/securities/MTSS.csv?iss.meta=off&iss.only=marketdata&marketdata.columns=SECID,OPEN,LOW,HIGH,LAST,HIGH,TIME,SYSTIME,ISSUECAPITALIZATION_UPDATETIME");
    //d2 = GET("https://iss.moex.com/iss/engines/stock/markets/shares/boards/TQBR/securities/MTSS.html?iss.meta=off&iss.only=marketdata&marketdata.columns=SECID,OPEN,LOW,HIGH,LAST,HIGH,TIME,SYSTIME,ISSUECAPITALIZATION_UPDATETIME");
    //d3 = GET("https://iss.moex.com/iss/engines/stock/markets/shares/boards/TQBR/securities/MTSS.json?iss.meta=off&iss.only=marketdata&marketdata.columns=SECID,OPEN,LOW,HIGH,LAST,HIGH,TIME,SYSTIME,ISSUECAPITALIZATION_UPDATETIME");
    //d4 = GET("https://iss.moex.com/iss/engines/stock/markets/shares/boards/TQBR/securities/MTSS.xml?iss.meta=off&iss.only=marketdata&marketdata.columns=SECID,OPEN,LOW,HIGH,LAST,HIGH,TIME,SYSTIME,ISSUECAPITALIZATION_UPDATETIME");

    std::cout << d1 << std::endl;
    //std::cout << d2 << std::endl;
    //std::cout << d3 << std::endl;
    //std::cout << d4 << std::endl;
#endif
    return 0;
}


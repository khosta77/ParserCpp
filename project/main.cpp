#include "parserpool.hpp"

int main()
{
    ParserPool pp( std::thread::hardware_concurrency() );
    return pp.run();
}



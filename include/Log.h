#define LOG_H
#include <iostream>

namespace plog
{
#ifdef ENABLE_LOG
    void log(std::string s)
    {
        std::cout << "LOG:\t" << s << std::endl;
    };
#else
    void log(std::string s)
    {
        (void)s;
    };
#endif
};

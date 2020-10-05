#ifndef StringUtility_h
#define StringUtility_h

#include <string>

class StringUtility
{
public:
    static std::string &ltrim(std::string &str, const std::string &chars = "\t\n\v\f\r ");
    static std::string &rtrim(std::string &str, const std::string &chars = "\t\n\v\f\r ");
    static std::string &trim(std::string &str, const std::string &chars = "\t\n\v\f\r ");
};

#endif

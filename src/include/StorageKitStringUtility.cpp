#include "StorageKitStringUtility.h"

using namespace std;

std::string &StringUtility::ltrim(std::string &str, const std::string &chars)
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

std::string &StringUtility::rtrim(std::string &str, const std::string &chars)
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

std::string &StringUtility::trim(std::string &str, const std::string &chars)
{
    return ltrim(rtrim(str, chars), chars);
}

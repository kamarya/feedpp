#include <cstring>
#include <ctime>
#include <locale>
#include <regex>
#include <iostream>
#include <array>

#include "date.h"

namespace feedpp {

bool date::validate(const std::string& date, const char* str_regex)
{
    std::regex regx(str_regex);
    return std::regex_match(date, regx);
}

}

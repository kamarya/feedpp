/* Copyright (C) 2016 Behrooz Aliabadi
 * Licensed under the MIT/X Consortium License.
 * See file LICENSE for more information.
 */

#include <cstring>
#include <ctime>
#include <locale>
#include <iostream>
#include <array>
#include <algorithm>
#include "log.h"
#include "date.h"

#ifdef USE_BOOST_REGEX
#include <boost/regex.hpp>
#define REGEX_NS boost
#else
#include <regex>
#define REGEX_NS std
#endif

namespace feedpp {

    bool date::validate(const std::string& date, const char* str_regex)
    {
        REGEX_NS::regex regx(str_regex);
        return REGEX_NS::regex_match(date, regx);
    }

    std::string date::format(const std::string& date)
    {
        std::string   ret;
        struct tm     tm_date;
        char*         p;

        memset(&tm_date, 0x00, sizeof (struct tm));

        if (date::validate(date, REGEX_RFC822))
        {
            p = strptime(date.c_str(), "%a, %d %b %Y %H:%M:%S", &tm_date);
            if (p == nullptr) LOG_ERROR("strptime() returned with error for REGEX_RFC822.");
        }
        else if (date::validate(date, REGEX_W3CDTF))
        {
            p = w3cdtf_to_tm(date, &tm_date);
            if (p == nullptr) LOG_ERROR("strptime() returned with error for REGEX_W3CDTF.");
        }
        else if (date::validate(date, REGEX_ISO8601))
        {
            //LOG_DEBUG("date : %s", date.c_str());
            p = strptime(date.c_str(), "%Y-%m-%dT%H:%M:%SZ", &tm_date);

            if (p == nullptr)
            {
                p = strptime(date.c_str(), "%t%Y-%m-%dT%H:%M%t", &tm_date);
            }

            if (p == nullptr)
            {
                p = strptime(date.c_str(), "%t%Y-%m-%d", &tm_date);
            }

            if (p == nullptr)
            {
                // warning : skip the time-zone
                p = strptime(date.c_str(), "%FT%T", &tm_date);
            }

            if (p == nullptr) LOG_ERROR("REGEX_ISO8601 : strptime() returned with error for (%s).", date.c_str());
        }
        else
        {
            LOG_ERROR("date::format() failed to proccess the date.");
            return ret;
        }

        ret = std::string(std::asctime(&tm_date));

        return ret;
    }

    char* date::w3cdtf_to_tm(const std::string& date, struct tm *tm_arg)
    {
        struct tm stm;

        memset(&stm,    0x00, sizeof (struct tm));

        stm.tm_mday = 1;

        char * ptr = strptime(date.c_str(), "%Y", &stm);

        if (ptr != NULL)
        {
            ptr = strptime(ptr, "-%m", &stm);
        }
        else
        {
            return ptr;
        }

        if (ptr != NULL)
        {
            ptr = strptime(ptr, "-%d", &stm);
        }
        if (ptr != NULL)
        {
            ptr = strptime(ptr, "T%H", &stm);
        }
        if (ptr != NULL)
        {
            ptr = strptime(ptr, ":%M", &stm);
        }
        if (ptr != NULL)
        {
            ptr = strptime(ptr, ":%S", &stm);
        }

        int offs = 0;
        if (ptr != NULL)
        {
            if (ptr[0] == '+' || ptr[0] == '-')
            {
                unsigned int hour, min;
                if (sscanf(ptr + 1,"%02u:%02u", &hour, &min) == 2)
                {
                    offs = 60 * 60 * hour + 60 * min;
                    if (ptr[0] == '+') offs = -offs;
                    stm.tm_gmtoff = offs;
                }
            }
            else if (ptr[0] == 'Z')
            {
                stm.tm_gmtoff = 0;
            }
        }

        time_t t = mktime(&stm);
        time_t x = time(NULL);
        t += localtime(&x)->tm_gmtoff + offs;
        std::memcpy(tm_arg, std::localtime(&t), sizeof(struct tm));
        return ptr;
    }
}

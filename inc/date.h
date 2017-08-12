/* Copyright (C) 2016 Behrooz Aliabadi
 * Licensed under the MIT/X Consortium License.
 * See file LICENSE for more information.
 */

#ifndef _DATE_H
#define _DATE_H

#define REGEX_RFC822        "(((Mon)|(Tue)|(Wed)|(Thu)|(Fri)|(Sat)|(Sun)), *)?\\d\\d? +"\
                            "((Jan)|(Feb)|(Mar)|(Apr)|(May)|(Jun)|(Jul)|(Aug)|(Sep)|(Oct)|(Nov)|(Dec))"\
                            " +\\d\\d(\\d\\d)? +\\d\\d:\\d\\d(:\\d\\d)? +(([+\\-]?\\d\\d\\d\\d)"\
                            "|(UT)|(GMT)|(EST)|(EDT)|(CST)|(CDT)|(MST)|(MDT)|(PST)|(PDT)|\\w)"

#define REGEX_RFC822_RED    "(((Mon)|(Tue)|(Wed)|(Thu)|(Fri)|(Sat)|(Sun)), *)?\\d\\d? +"\
                            "((Jan)|(Feb)|(Mar)|(Apr)|(May)|(Jun)|(Jul)|(Aug)|(Sep)|(Oct)|(Nov)|(Dec))"\
                            " +\\d\\d(\\d\\d)? +\\d\\d:\\d\\d(:\\d\\d)?"

#define REGEX_RFC1123       "(((Mon)|(Tue)|(Wed)|(Thu)|(Fri)|(Sat)|(Sun)), *)?\\d\\d? +"\
                            "((Jan)|(Feb)|(Mar)|(Apr)|(May)|(Jun)|(Jul)|(Aug)|(Sep)|(Oct)|(Nov)|(Dec))"\
                            " +\\d\\d(\\d\\d)? +\\d\\d:\\d\\d(:\\d\\d)? +(([+\\-]?\\d\\d\\d\\d)"\
                            "|(UT)|(GMT)|(EST)|(EDT)|(CST)|(CDT)|(MST)|(MDT)|(PST)|(PDT)|\\w)"

#define REGEX_W3CDTF        "(\\d{4})-(\\d{2})-(\\d{2})T(\\d{2}):(\\d{2})(:(\\d{2}))?(?:([-+])"\
                            "(\\d{2}):?(\\d{2})|(Z))?"

#define REGEX_ISO8601       "^(?:[1-9]\\d{3}-(?:(?:0[1-9]|1[0-2])-(?:0[1-9]|1\\d|2[0-8])|"\
                            "(?:0[13-9]|1[0-2])-(?:29|30)|(?:0[13578]|1[02])-31)|(?:[1-9]"\
                            "\\d(?:0[48]|[2468][048]|[13579][26])|(?:[2468][048]|[13579][26])00)-02-29)"\
                            "T(?:[01]\\d|2[0-3]):[0-5]\\d:[0-5]\\d(?:Z|[+-][01]\\d:[0-5]\\d)$"

namespace feedpp
{
class date
{
  public:
    static bool validate(const std::string& date, const char* str_regex);
    static std::string format(const std::string& date);
    static char* w3cdtf_to_tm(const std::string& date, struct tm *tm_arg);
};
}

#endif

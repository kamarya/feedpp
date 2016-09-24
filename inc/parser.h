#ifndef _PARSER_H
#define _PARSER_H

#include <string>

namespace feedpp {

class parser {

    public:
        parser(unsigned int timeout = 30, const char * user_agent = "FeedPP/0.5.0",
                const char * proxy = 0, const char * proxy_auth = 0,
                curl_proxytype proxy_type = CURLPROXY_HTTP);

        ~parser();

        feed parse_url(const std::string& url,
                CURL *ehandle = nullptr,
                time_t lastmodified = 0,
                const std::string& etag = "",
                const std::string& cookie_cache = "");

        feed parse_buffer(const char * buffer, size_t size, const char * url = NULL);

        feed parse_file(const std::string& filename);

        time_t get_last_modified() {
            return lm;
        }
        const std::string& get_etag() {
            return et;
        }

        static void global_init();
        static void global_cleanup();
    private:

        feed parse_xmlnode(xmlNode * node);
        unsigned int to;
        const char * ua;
        const char * prx;
        const char * prxauth;
        curl_proxytype prxtype;
        xmlDocPtr doc;
        time_t lm;
        std::string et;
};

}

#endif

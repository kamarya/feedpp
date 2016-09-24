/* Copyright (C) 2008-2012 Andreas Krennmair <ak@newsbeuter.org>
 * Modified work Copyright (C) 2016 Behrooz Aliabadi
 * Licensed under the MIT/X Consortium License. See file LICENSE
 * for more information.
 */

#include <libxml/tree.h>
#include <cstring>
#include <ctime>
#include <locale>
#include <iostream>
#include <array>

#include "rsspp_internal.h"

namespace feedpp {

std::string rss_parser::get_content(xmlNode * node) {
	std::string retval;
	if (node) {
		xmlChar * content = xmlNodeGetContent(node);
		if (content) {
			retval = (const char *)content;
			xmlFree(content);
		}
	}
	return retval;
}

void rss_parser::cleanup_namespaces(xmlNodePtr node) {
	node->ns = NULL;
	for (auto ptr = node->children; ptr != NULL; ptr = ptr->next) {
		cleanup_namespaces(ptr);
	}
}

std::string rss_parser::get_xml_content(xmlNode * node) {
	xmlBufferPtr buf = xmlBufferCreate();
	std::string result;

	cleanup_namespaces(node);

	if (node->children) {
		for (xmlNodePtr ptr = node->children; ptr != NULL; ptr = ptr->next) {
			if (xmlNodeDump(buf, doc, ptr, 0, 0) >= 0) {
				result.append((const char *)xmlBufferContent(buf));
				xmlBufferEmpty(buf);
			} else {
				result.append(get_content(ptr));
			}
		}
	} else {
		result = get_content(node); // fallback
	}
	xmlBufferFree(buf);

	return result;
}

std::string rss_parser::get_prop(xmlNode * node, const char * prop, const char * ns) {
	std::string retval;
	if (node) {
		xmlChar * value;
		if (ns)
			value = xmlGetProp(node, (xmlChar *)prop);
		else
			value = xmlGetNsProp(node, (xmlChar *)prop, (xmlChar *)ns);
		if (value) {
			retval = (const char*)value;
			xmlFree(value);
		}
	}
	return retval;
}

std::string rss_parser::w3cdtf_to_rfc822(const std::string& w3cdtf) {
	return __w3cdtf_to_rfc822(w3cdtf);
}

std::string rss_parser::__w3cdtf_to_rfc822(const std::string& w3cdtf) {
	struct tm stm;
	memset(&stm, 0, sizeof (stm));
	stm.tm_mday = 1;

	char * ptr = strptime(w3cdtf.c_str(), "%Y", &stm);

	if (ptr != NULL) {
		ptr = strptime(ptr, "-%m", &stm);
	} else {
		return "";
	}

	if (ptr != NULL) {
		ptr = strptime(ptr, "-%d", &stm);
	}
	if (ptr != NULL) {
		ptr = strptime(ptr, "T%H", &stm);
	}
	if (ptr != NULL) {
		ptr = strptime(ptr, ":%M", &stm);
	}
	if (ptr != NULL) {
		ptr = strptime(ptr, ":%S", &stm);
	}

	int offs = 0;
	if (ptr != NULL) {
		if (ptr[0] == '+' || ptr[0] == '-') {
			unsigned int hour, min;
			if (sscanf(ptr+1,"%02u:%02u", &hour, &min)==2) {
				offs = 60*60*hour + 60*min;
				if (ptr[0] == '+')
					offs = -offs;
				stm.tm_gmtoff = offs;
			}
		} else if (ptr[0] == 'Z') {
			stm.tm_gmtoff = 0;
		}
	}

	time_t t = mktime(&stm);
	time_t x = time(NULL);
	t += localtime(&x)->tm_gmtoff + offs;
	char datebuf[256];
	strftime (datebuf, sizeof (datebuf), "%a, %d %b %Y %H:%M:%S %z", gmtime(&t));
	return datebuf;
}

std::string rss_parser::to_rfc1123(const struct tm& date)
{
  std::array<std::string, 7> DAY_NAMES =
    {{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" }};
  std::array<std::string, 12> MONTH_NAMES =
    {{ "Jan", "Feb", "Mar", "Apr", "May", "Jun",
      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" }};
      const int RFC1123_TIME_LEN = 29;
      char buf[RFC1123_TIME_LEN + 1];

      std::strftime(buf, RFC1123_TIME_LEN + 1, "%a, %d %b %Y %H:%M:%S GMT", &date);

      return std::string(buf);
}

std::string rss_parser::validate_date(const std::string& date) {
  struct tm result;
  char *ret;
  char buff[255];
  std::array<std::string, 21> formats = {{
      "%Y",
      "%Y-%m",
      "%y-%m",
      "%Y-%m-%d",
      "%y-%m-%d",
      "%Y%m%d",
      "%y%m%d",
      "%Y-%m-%d %T",
      "%y-%m-%d %T",
      "%Y%m%d%H%M%S",
      "%y%m%d%H%M%S",
      "%Y-%m-%dT%T",
      "%y-%m-%dT%T",
      "%Y-%m-%dT%TZ",
      "%y-%m-%dT%TZ",
      "%Y-%m-%d %TZ",
      "%y-%m-%d %TZ",
      "%Y%m%dT%TZ",
      "%y%m%dT%TZ",
      "%Y%m%d %TZ",
      "%y%m%d %TZ" }};

  std::memset(&result, 0, sizeof(result));

  for(const auto& f : formats) {
      ret = strptime(date.data(), f.data(), &result);

      if (ret && *ret == '\0') {
        if (strftime(buff, sizeof(buff), f.data(), &result)) {
          if (std::strcmp(buff, date.data()) == 0) return date;
        }
      }
  }

  return std::string();
}


bool rss_parser::node_is(xmlNode * node, const char * name, const char * ns_uri) {
	if (!node || !name || !node->name)
		return false;

	if (strcmp((const char *)node->name, name)==0) {
		if (!ns_uri && !node->ns)
			return true;
		if (ns_uri && node->ns && node->ns->href && strcmp((const char *)node->ns->href, ns_uri)==0)
			return true;
	}
	return false;
}

}

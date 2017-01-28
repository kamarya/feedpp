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

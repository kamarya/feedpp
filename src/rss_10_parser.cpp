/* Copyright (C) 2008-2012 Andreas Krennmair <ak@newsbeuter.org>
 * Modified work Copyright (C) 2016 Behrooz Aliabadi
 * Licensed under the MIT/X Consortium License. See file LICENSE
 * for more information.
 */

#include "rsspp_internal.h"
#include "exception.h"
#include "date.h"
#include <cstring>

#define RSS_1_0_NS "http://purl.org/rss/1.0/"

namespace feedpp {

void rss_10_parser::parse_feed(feed& f, xmlNode * rootNode)
{
	if (!rootNode)
		throw exception(std::string("XML root node is NULL"));

	for (xmlNode * node = rootNode->children; node != NULL; node = node->next)
	{
		if (node_is(node, "channel", RSS_1_0_NS))
		{
			for (xmlNode * cnode = node->children; cnode != NULL; cnode = cnode->next)
			{
				if (node_is(cnode, "title", RSS_1_0_NS))
				{
					f.title = get_content(cnode);
					f.title_type = "text";
				}
				else if (node_is(cnode, "link", RSS_1_0_NS))
				{
					f.link = get_content(cnode);
				}
				else if (node_is(cnode, "description", RSS_1_0_NS))
				{
					f.description = get_content(cnode);
				}
				else if (node_is(cnode, "date", DC_URI))
				{
					f.pubDate = date::format(get_content(cnode));
				}
				else if (node_is(cnode, "creator", DC_URI))
				{
					f.dc_creator = get_content(cnode);
				}
			}
		}
		else if (node_is(node, "item", RSS_1_0_NS))
		{
			item it;
			it.guid = get_prop(node, "about", RDF_URI);
			for (xmlNode * itnode = node->children; itnode != NULL; itnode = itnode->next)
			{
				if (node_is(itnode, "title", RSS_1_0_NS))
				{
					it.title = get_content(itnode);
					it.title_type = "text";
				}
				else if (node_is(itnode, "link", RSS_1_0_NS))
				{
					it.link = get_content(itnode);
				}
				else if (node_is(itnode, "description", RSS_1_0_NS))
				{
					it.description = get_content(itnode);

                    if (xmlChildElementCount(itnode) == 0)
                    {
                        it.description_type = (it.description.find_first_of("<") == std::string::npos)?"text":"html";
                    }
                    else
                    {
                        it.description_type = "html";
                    }
				}
				else if (node_is(itnode, "date", DC_URI))
				{
					it.pubDate = date::format(get_content(itnode));
				}
				else if (node_is(itnode, "encoded", CONTENT_URI))
				{
					it.content_encoded = get_content(itnode);
				}
				else if (node_is(itnode, "summary", ITUNES_URI))
				{
					it.itunes_summary = get_content(itnode);
				}
			}
			f.items.push_back(it);
		}
	}
}

}

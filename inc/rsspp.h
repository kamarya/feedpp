/* Copyright (C) 2008-2012 Andreas Krennmair <ak@newsbeuter.org>
 * Modified work Copyright (C) 2015 Behrooz Aliabadi
 * Licensed under the MIT/X Consortium License. See file LICENSE
 * for more information.
 */

#ifndef RSSPP_H
#define RSSPP_H

#include <string>
#include <vector>
#include <libxml/parser.h>
#include <curl/curl.h>

namespace rsspp {

enum version {
  UNKNOWN = 0,
  RSS_0_91,
  RSS_0_92,
  RSS_1_0,
  RSS_2_0,
  ATOM_0_3,
  ATOM_1_0,
  RSS_0_94,
  ATOM_0_3_NONS,
  TTRSS_JSON,
  NEWSBLUR_JSON
};

struct item {
  std::string title;
  std::string title_type;
  std::string link;
  std::string description;
  std::string description_type;

  std::string author;
  std::string author_email;

  std::string pubDate;
  std::string guid;
  bool guid_isPermaLink;

  std::string enclosure_url;
  std::string enclosure_type;

  // extensions:
  std::string content_encoded;
  std::string itunes_summary;

  // Atom-specific:
  std::string base;
  std::vector<std::string> labels;

  // only required for ttrss support:
  time_t pubDate_ts;
};

struct feed {
  std::string encoding;

  version rss_version;
  std::string title;
  std::string title_type;
  std::string description;
  std::string link;
  std::string language;
  std::string managingeditor;
  std::string dc_creator;
  std::string pubDate;

  std::vector<item> items;
};

}

#endif

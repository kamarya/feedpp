#include <ctime>
#include <cstring>

#include "rsspp_internal.h"
#include "lemon.h"
#include "feedpp.h"

int main(void)
{
    lemon::test<> lemon(83);

    feedpp::parser p;

    // test of RSS 0.91
    feedpp::feed f = p.parse_file("../data/rss091_1.xml");

    lemon.is(f.rss_version, feedpp::RSS_0_91, "RSS type is RSS 0.91");
    lemon.is(f.title, "Example Channel", "RSS feed title is Example Channel");
    lemon.is(f.description, "an example feed", "RSS feed description is 'an example feed'");
    lemon.is(f.link, "http://example.com/", "RSS feed link is http://example.com/");
    lemon.is(f.language, "en", "RSS feed language is en");

    lemon.is(f.items.size(), 1u, "RSS feed contains 1 item");
    lemon.is(f.items[0].title, "1 < 2", "item title with &lt; entity");
    lemon.is(f.items[0].link, "http://example.com/1_less_than_2.html", "item link");
    lemon.is(f.items[0].description, "1 < 2, 3 < 4.\nIn HTML, <b> starts a bold phrase\nand you start a link with <a href=\n", "item description with various entities");
    lemon.is(f.items[0].author, "", "empty author");
    lemon.is(f.items[0].guid, "", "empty guid");

    // test of RSS 0.92
    f = p.parse_file("../data/rss092_1.xml");

    lemon.is(f.rss_version, feedpp::RSS_0_92, "RSS type is RSS 0.92");
    lemon.is(f.title, "Example Channel", "RSS feed title");
    lemon.is(f.description, "an example feed", "RSS feed description");
    lemon.is(f.link, "http://example.com/", "RSS feed link");
    lemon.is(f.language, "en", "RSS feed language");

    lemon.is(f.items.size(), 2u, "feed contains 2 items");

    lemon.is(f.items[1].title, "A second item", "second item title");
    lemon.is(f.items[1].link, "http://example.com/a_second_item.html", "second item link");
    lemon.is(f.items[1].description, "no description", "second item description");
    lemon.is(f.items[1].author, "", "empty second item author");
    lemon.is(f.items[1].guid, "", "empty second item guid");

    // test of RSS 2.0
    f = p.parse_file("../data/rss20_1.xml");

    lemon.is(f.title, "my weblog", "RSS 2.0 feed title");
    lemon.is(f.link, "http://example.com/blog/", "RSS 2.0 feed link");
    lemon.is(f.description, "my description", "RSS 2.0 description");

    lemon.is(f.items.size(), 1u, "RSS 2.0 feed contains 1 item");

    lemon.is(f.items[0].title, "this is an item", "RSS 2.0 item title");
    lemon.is(f.items[0].link, "http://example.com/blog/this_is_an_item.html", "RSS 2.0 item link");
    lemon.is(f.items[0].author, "Andreas Krennmair", "RSS 2.0 item author");
    lemon.is(f.items[0].author_email, "blog@synflood.at", "RSS 2.0 item author email");
    lemon.is(f.items[0].content_encoded, "oh well, this is the content.", "RSS 2.0 item content:encoded");
    lemon.is(f.items[0].pubDate, "Fri, 12 Dec 2008 02:36:10 +0100", "RSS 2.0 item publication date");
    lemon.is(f.items[0].guid, "http://example.com/blog/this_is_an_item.html", "RSS 2.0 item guid");
    lemon.is(f.items[0].guid_isPermaLink, false, "RSS 2.0 item guid is not a permalink");

    // test of RSS 1.0
    f = p.parse_file("../data/rss10_1.xml");
    lemon.is(f.rss_version, feedpp::RSS_1_0, "RSS 1.0 type");

    lemon.is(f.title, "Example Dot Org", "RSS 1.0 feed title");
    lemon.is(f.link, "http://www.example.org", "RSS 1.0 feed link");
    lemon.is(f.description, "the Example Organization web site", "RSS 1.0 feed description");

    lemon.is(f.items.size(), 1u, "RSS 1.0 feed contains 1 item");

    lemon.is(f.items[0].title, "New Status Updates", "RSS 1.0 item title");
    lemon.is(f.items[0].link, "http://www.example.org/status/foo", "RSS 1.0 item link");
    lemon.is(f.items[0].guid, "http://www.example.org/status/", "RSS 1.0 item guid");
    lemon.is(f.items[0].description, "News about the Example project", "RSS 1.0 item description");
    lemon.is(f.items[0].pubDate, "Tue, 30 Dec 2008 07:20:00 +0000", "RSS 1.0 item publication date");

    // test of Atom 1.0

    f = p.parse_file("../data/atom10_1.xml");
    lemon.is(f.rss_version, feedpp::ATOM_1_0, "Atom 1.0 type");

    lemon.is(f.title, "test atom", "Atom 1.0 feed title");
    lemon.is(f.title_type, "text", "Atom 1.0 feed title type");
    lemon.is(f.description, "atom description!", "Atom 1.0 feed description");
    lemon.is(f.pubDate, "Tue, 30 Dec 2008 18:26:15 +0000", "Atom 1.0 feed publication date");
    lemon.is(f.link, "http://example.com/", "Atom 1.0 feed link");

    lemon.is(f.items.size(), 3u, "Atom 1.0 feed contains 3 items");
    lemon.is(f.items[0].title, "A gentle introduction to Atom testing", "Atom 1.0 first item title");
    lemon.is(f.items[0].title_type, "html", "Atom 1.0 first item title type");
    lemon.is(f.items[0].link, "http://example.com/atom_testing.html", "Atom 1.0 first item link");
    lemon.is(f.items[0].guid, "tag:example.com,2008-12-30:/atom_testing", "Atom 1.0 first item guid");
    lemon.is(f.items[0].description, "some content", "Atom 1.0 first item description");

    lemon.is(f.items[1].title, "A missing rel attribute", "Atom 1.0 second item title");
    lemon.is(f.items[1].title_type, "html", "Atom 1.0 second item title type");
    lemon.is(f.items[1].link, "http://example.com/atom_testing.html", "Atom 1.0 second item link");
    lemon.is(f.items[1].guid, "tag:example.com,2008-12-30:/atom_testing1", "Atom 1.0 second item guid");
    lemon.is(f.items[1].description, "some content", "Atom 1.0 second item description");

    lemon.is(f.items[2].title, "alternate link isn't first", "Atom 1.0 third item title");
    lemon.is(f.items[2].title_type, "html", "Atom 1.0 third item title type");
    lemon.is(f.items[2].link, "http://example.com/atom_testing.html", "Atom 1.0 third item link");
    lemon.is(f.items[2].guid, "tag:example.com,2008-12-30:/atom_testing2", "Atom 1.0 third item guid");
    lemon.is(f.items[2].description, "some content", "Atom 1.0 third item link");

    // test of W3CDTF parser
    setlocale(LC_TIME, "en_US.utf8");


    lemon.ok(feedpp::date::validate("Wed, 31 Dec 2008 13:03:15 GMT", REGEX_RFC1123), "regex rfc1123");
    lemon.ok(feedpp::date::validate("Sat, 28 Aug 2004 08:15:38 GMT", REGEX_RFC1123), "regex rfc1123");
    lemon.ok(feedpp::date::validate("Tue, 30 Dec 2008 18:03:15 +0000", REGEX_RFC822), "regex rfc822");
    lemon.ok(feedpp::date::validate("1994-11-05T08:15:30-05:00", REGEX_W3CDTF), "regex w3cdtf");
    lemon.ok(feedpp::date::validate("2015-01-17T18:23:02+06:45", REGEX_ISO8601), "regex iso8601");

    return lemon.done() ? 0 : 1;
}

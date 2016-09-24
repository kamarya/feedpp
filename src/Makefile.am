AM_CXXFLAGS   = -std=c++11
AM_CPPFLAGS   = -I$(top_srcdir)/inc ${libcurl_CFLAGS} ${libxml2_CFLAGS}


lib_LTLIBRARIES         = libfeedpp.la
libfeedpp_la_SOURCES	= atom_parser.cpp \
						  exception.cpp \
						  parser.cpp \
						  parser_factory.cpp \
						  rss_09x_parser.cpp \
						  rss_10_parser.cpp \
						  rss_parser.cpp \
						  utils.cpp

libfeedpp_la_LDFLAGS    = -version-info 0:5:0
libfeedpp_la_LIBADD	    = ${libcurl_LIBS} ${libxml2_LIBS}
pkginclude_HEADERS	    = $(top_srcdir)/inc/*.h

check_PROGRAMS          = unitest
unitest_SOURCES         = unitest.cpp
unitest_LDADD           = ${libcurl_LIBS} ${libxml2_LIBS} libfeedpp.la

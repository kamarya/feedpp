#include "utils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iconv.h>
#include <errno.h>
#include <pwd.h>
#include <libgen.h>
#include <sys/utsname.h>

#include <unistd.h>
#include <sstream>
#include <locale>
#include <cwchar>
#include <cstring>
#include <cstdlib>
#include <cstdarg>

#include <curl/curl.h>

#include <langinfo.h>
#include <libxml/uri.h>

#if HAVE_GCRYPT
#include <gnutls/gnutls.h>
#include <gcrypt.h>
#include <errno.h>
#include <pthread.h>
GCRY_THREAD_OPTION_PTHREAD_IMPL;
#endif

#if HAVE_OPENSSL
#include <openssl/crypto.h>
#endif

namespace feedpp {

std::vector<std::string> utils::tokenize_quoted(const std::string& str, std::string delimiters) {
	/*
	 * This function tokenizes strings, obeying quotes and throwing away comments that start
	 * with a '#'.
	 *
	 * e.g. line: foo bar "foo bar" "a test"
	 * is parsed to 4 elements:
	 * 	[0]: foo
	 * 	[1]: bar
	 * 	[2]: foo bar
	 * 	[3]: a test
	 *
	 * e.g. line: yes great "x\ny" # comment
	 * is parsed to 3 elements:
	 * 	[0]: yes
	 * 	[1]: great
	 * 	[2]: x
	 * 	y
	 *
	 * 	\", \r, \n, \t and \v are replaced with the literals that you know from C/C++ strings.
	 *
	 */
	bool attach_backslash = true;
	std::vector<std::string> tokens;
	std::string::size_type last_pos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos = last_pos;

	while (pos != std::string::npos && last_pos != std::string::npos) {
		if (str[last_pos] == '#') // stop as soon as we found a comment
			break;

		if (str[last_pos] == '"') {
			++last_pos;
			pos = last_pos;
			int backslash_count = 0;
			while (pos < str.length() && (str[pos] != '"' || (backslash_count%2))) {
				if (str[pos] == '\\') {
					++backslash_count;
				} else {
					backslash_count = 0;
				}
				++pos;
			}
			if (pos >= str.length()) {
				pos = std::string::npos;
				std::string token;
				while (last_pos < str.length()) {
					if (str[last_pos] == '\\') {
						if (str[last_pos-1] == '\\') {
							if (attach_backslash) {
								token.append("\\");
							}
							attach_backslash = !attach_backslash;
						}
					} else {
						if (str[last_pos-1] == '\\') {
							append_escapes(token, str[last_pos]);
						} else {
							token.append(1, str[last_pos]);
						}
					}
					++last_pos;
				}
				tokens.push_back(token);
			} else {
				std::string token;
				while (last_pos < pos) {
					if (str[last_pos] == '\\') {
						if (str[last_pos-1] == '\\') {
							if (attach_backslash) {
								token.append("\\");
							}
							attach_backslash = !attach_backslash;
						}
					} else {
						if (str[last_pos-1] == '\\') {
							append_escapes(token, str[last_pos]);
						} else {
							token.append(1, str[last_pos]);
						}
					}
					++last_pos;
				}
				tokens.push_back(token);
				++pos;
			}
		} else {
			pos = str.find_first_of(delimiters, last_pos);
			tokens.push_back(str.substr(last_pos, pos - last_pos));
		}
		last_pos = str.find_first_not_of(delimiters, pos);
	}

	return tokens;
}


std::vector<std::string> utils::tokenize(const std::string& str, std::string delimiters) {
	/*
	 * This function tokenizes a string by the delimiters. Plain and simple.
	 */
	std::vector<std::string> tokens;
	std::string::size_type last_pos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos = str.find_first_of(delimiters, last_pos);

	while (std::string::npos != pos || std::string::npos != last_pos) {
		tokens.push_back(str.substr(last_pos, pos - last_pos));
		last_pos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, last_pos);
	}
	return tokens;
}

std::vector<std::wstring> utils::wtokenize(const std::wstring& str, std::wstring delimiters) {
	/*
	 * This function tokenizes a string by the delimiters. Plain and simple.
	 */
	std::vector<std::wstring> tokens;
	std::wstring::size_type last_pos = str.find_first_not_of(delimiters, 0);
	std::wstring::size_type pos = str.find_first_of(delimiters, last_pos);

	while (std::string::npos != pos || std::string::npos != last_pos) {
		tokens.push_back(str.substr(last_pos, pos - last_pos));
		last_pos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, last_pos);
	}
	return tokens;
}

std::vector<std::string> utils::tokenize_spaced(const std::string& str, std::string delimiters) {
	std::vector<std::string> tokens;
	std::string::size_type last_pos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos = str.find_first_of(delimiters, last_pos);

	if (last_pos != 0) {
		tokens.push_back(std::string(" "));
	}

	while (std::string::npos != pos || std::string::npos != last_pos) {
		tokens.push_back(str.substr(last_pos, pos - last_pos));
		last_pos = str.find_first_not_of(delimiters, pos);
		if (last_pos > pos)
			tokens.push_back(std::string(" "));
		pos = str.find_first_of(delimiters, last_pos);
	}

	return tokens;
}

std::vector<std::string> utils::tokenize_nl(const std::string& str, std::string delimiters) {
	std::vector<std::string> tokens;
	std::string::size_type last_pos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos = str.find_first_of(delimiters, last_pos);
	unsigned int i;

	//LOG(LOG_DEBUG,"utils::tokenize_nl: last_pos = %u",last_pos);
	if (last_pos != std::string::npos) {
		for (i=0; i<last_pos; ++i) {
			tokens.push_back(std::string("\n"));
		}
	}

	while (std::string::npos != pos || std::string::npos != last_pos) {
		tokens.push_back(str.substr(last_pos, pos - last_pos));
		//LOG(LOG_DEBUG,"utils::tokenize_nl: substr = %s", str.substr(last_pos, pos - last_pos).c_str());
		last_pos = str.find_first_not_of(delimiters, pos);
		//LOG(LOG_DEBUG,"utils::tokenize_nl: pos - last_pos = %u", last_pos - pos);
		for (i=0; last_pos != std::string::npos && pos != std::string::npos && i<(last_pos - pos); ++i) {
			tokens.push_back(std::string("\n"));
		}
		pos = str.find_first_of(delimiters, last_pos);
	}

	return tokens;
}

void utils::remove_fs_lock(const std::string& lock_file) {
	//LOG(LOG_DEBUG, "utils::remove_fs_lock: removed lockfile %s", lock_file.c_str());
	::unlink(lock_file.c_str());
}

bool utils::try_fs_lock(const std::string& lock_file, pid_t & pid) {
	int fd;
	// pid == 0 indicates that something went majorly wrong during locking
	pid = 0;

	//LOG(LOG_DEBUG, "utils::try_fs_lock: trying to lock %s", lock_file.c_str());

	// first, we open (and possibly create) the lock file
	fd = ::open(lock_file.c_str(), O_RDWR | O_CREAT, 0600);
	if (fd < 0)
		return false;

	// then we lock it (T_LOCK returns immediately if locking is not possible)
	if (lockf(fd, F_TLOCK, 0) == 0) {
		std::string pidtext = utils::to_string<unsigned int>(getpid());
		// locking successful -> truncate file and write own PID into it
		ftruncate(fd, 0);
		write(fd, pidtext.c_str(), pidtext.length());
		return true;
	}

	// locking was not successful -> read PID of locking process from it
	fd = ::open(lock_file.c_str(), O_RDONLY);
	if (fd >= 0) {
		char buf[32];
		int len = read(fd, buf, sizeof(buf)-1);
		unsigned int upid = 0;
		buf[len] = '\0';
		sscanf(buf, "%u", &upid);
		pid = upid;
		close(fd);
	}
	return false;
}


std::string utils::get_command_output(const std::string& cmd) {
	FILE * f = popen(cmd.c_str(), "r");
	std::string buf;
	char cbuf[1024];
	if (f) {
		size_t s;
		while ((s = fread(cbuf, 1, sizeof(cbuf), f)) > 0) {
			buf.append(cbuf, s);
		}
		pclose(f);
	}
	return buf;
}

void utils::extract_filter(const std::string& line, std::string& filter, std::string& url) {
	std::string::size_type pos = line.find_first_of(":", 0);
	std::string::size_type pos1 = line.find_first_of(":", pos + 1);
	filter = line.substr(pos+1, pos1 - pos - 1);
	pos = pos1;
	url = line.substr(pos+1, line.length() - pos);
	//LOG(LOG_DEBUG, "utils::extract_filter: %s -> filter: %s url: %s", line.c_str(), filter.c_str(), url.c_str());
}

//static size_t my_write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
//	std::string * pbuf = static_cast<std::string *>(userp);
//	pbuf->append(static_cast<const char *>(buffer), size * nmemb);
//	return size * nmemb;
//}


void utils::run_command(const std::string& cmd, const std::string& input) {
	int rc = fork();
	switch (rc) {
	case -1:
		break;
	case 0: { // child:
		int fd = ::open("/dev/null", O_RDWR);
		close(0);
		close(1);
		close(2);
		dup2(fd, 0);
		dup2(fd, 1);
		dup2(fd, 2);
		//LOG(LOG_DEBUG, "utils::run_command: %s '%s'", cmd.c_str(), input.c_str());
		execlp(cmd.c_str(), cmd.c_str(), input.c_str(), NULL);
		//LOG(LOG_DEBUG, "utils::run_command: execlp of %s failed: %s", cmd.c_str(), strerror(errno));
		exit(1);
	}
	default:
		break;
	}
}

std::string utils::run_program(char * argv[], const std::string& input) {
	std::string buf;
	int ipipe[2];
	int opipe[2];
	pipe(ipipe);
	pipe(opipe);

	int rc = fork();
	switch (rc) {
	case -1:
		break;
	case 0: { // child:
		close(ipipe[1]);
		close(opipe[0]);
		dup2(ipipe[0], 0);
		dup2(opipe[1], 1);
		close(2);

		int errfd = ::open("/dev/null", O_WRONLY);
		if (errfd != -1) dup2(errfd, 2);

		execvp(argv[0], argv);
		exit(1);
	}
	default: {
		close(ipipe[0]);
		close(opipe[1]);
		write(ipipe[1], input.c_str(), input.length());
		close(ipipe[1]);
		char cbuf[1024];
		int rc2;
		while ((rc2 = read(opipe[0], cbuf, sizeof(cbuf))) > 0) {
			buf.append(cbuf, rc2);
		}
		close(opipe[0]);
	}
	break;
	}
	return buf;
}

std::string utils::resolve_tilde(const std::string& str) {
	const char * homedir;
	std::string filepath;

	if (!(homedir = ::getenv("HOME"))) {
		struct passwd * spw = ::getpwuid(::getuid());
		if (spw) {
			homedir = spw->pw_dir;
		} else {
			homedir = "";
		}
	}

	if (strcmp(homedir,"")!=0) {
		if (str == "~") {
			filepath.append(homedir);
		} else if (str.substr(0,2) == "~/") {
			filepath.append(homedir);
			filepath.append(1,'/');
			filepath.append(str.substr(2,str.length()-2));
		} else {
			filepath.append(str);
		}
	} else {
		filepath.append(str);
	}

	return filepath;
}

std::string utils::replace_all(std::string str, const std::string& from, const std::string& to) {
	std::string::size_type s = str.find(from);
	while (s != std::string::npos) {
		str.replace(s,from.length(), to);
		s = str.find(from, s + to.length());
	}
	return str;
}


template<class T> std::string utils::to_string(T var) {
	std::stringstream ret;
	ret << var;
	return ret.str();
}

// to avoid linker errors
template std::string utils::to_string<int>(int var);
template std::string utils::to_string<unsigned long>(unsigned long var);
template std::string utils::to_string<unsigned int>(unsigned int var);

std::string utils::absolute_url(const std::string& url, const std::string& link) {
	xmlChar * newurl = xmlBuildURI((const xmlChar *)link.c_str(), (const xmlChar *)url.c_str());
	std::string retval;
	if (newurl) {
		retval = (const char *)newurl;
		xmlFree(newurl);
	} else {
		retval = link;
	}
	return retval;
}

std::string utils::strprintf(const char * format, ...) {
	if (!format)
		return std::string();

	char buffer[1024];

	va_list ap;
	va_start(ap, format);

	unsigned int len = vsnprintf(buffer, sizeof(buffer), format, ap)+1;

	va_end(ap);
	if (len <= sizeof(buffer))
		return buffer;

	va_start(ap, format);

	char * buf = new char[len];
	vsnprintf(buf, len, format, ap);
	va_end(ap);

	std::string ret(buf);
	delete[] buf;

	return ret;
}


unsigned int utils::to_u(const std::string& str) {
	std::istringstream is(str);
	unsigned int u = 0;
	is >> u;
	return u;
}

void utils::append_escapes(std::string& str, char c) {
	switch (c) {
	case 'n':
		str.append("\n");
		break;
	case 'r':
		str.append("\r");
		break;
	case 't':
		str.append("\t");
		break;
	case '"':
		str.append("\"");
		break;
	case '\\':
		break;
	default:
		str.append(1, c);
		break;
	}
}

bool utils::is_valid_color(const std::string& color) {
	const char * colors[] = { "black", "red", "green", "yellow", "blue", "magenta", "cyan", "white", "default", NULL };
	for (unsigned int i=0; colors[i]; i++) {
		if (color == colors[i])
			return true;
	}
	if (strncmp(color.c_str(), "color", 5)==0)
		return true;
	return false;
}

bool utils::is_valid_attribute(const std::string& attrib) {
	const char * attribs[] = { "standout", "underline", "reverse", "blink", "dim", "bold", "protect", "invis", "default", NULL };
	for (unsigned int i=0; attribs[i]; i++) {
		if (attrib == attribs[i])
			return true;
	}
	return false;
}

std::vector<std::pair<unsigned int, unsigned int>> utils::partition_indexes(unsigned int start, unsigned int end, unsigned int parts) {
	std::vector<std::pair<unsigned int, unsigned int>> partitions;
	unsigned int count = end - start + 1;
	unsigned int size = count / parts;

	for (unsigned int i=0; i<parts-1; i++) {
		partitions.push_back(std::pair<unsigned int, unsigned int>(start, start + size - 1));
		start += size;
	}

	partitions.push_back(std::pair<unsigned int, unsigned int>(start, end));
	return partitions;
}


std::string utils::join(const std::vector<std::string>& strings, const std::string& separator) {
	std::string result;

	for (auto str : strings) {
		result.append(str);
		result.append(separator);
	}

	if (result.length() > 0)
		result.erase(result.length()-separator.length(), result.length());

	return result;
}

bool utils::is_special_url(const std::string& url) {
	return url.substr(0,6) == "query:" || url.substr(0,7) == "filter:" || url.substr(0,5) == "exec:";
}

bool utils::is_http_url(const std::string& url) {
	return url.substr(0,7) == "http://" || url.substr(0,8) == "https://";
}

std::string utils::censor_url(const std::string& url) {
	std::string rv;
	if (url.length() > 0 && !utils::is_special_url(url)) {
		const char * myuri = url.c_str();
		xmlURIPtr uri = xmlParseURI(myuri);
		if (uri) {
			if (uri->user) {
				xmlFree(uri->user);
				uri->user = (char *)xmlStrdup((const xmlChar *)"*:*");
			}
			xmlChar * uristr = xmlSaveUri(uri);

			rv = (const char *)uristr;
			xmlFree(uristr);
			xmlFreeURI(uri);
		} else
			return url;
	} else {
		rv = url;
	}
	return rv;
}


void utils::trim(std::string& str) {
	while (str.length() > 0 && ::isspace(str[0])) {
		str.erase(0,1);
	}
	trim_end(str);
}

void utils::trim_end(std::string& str) {
	std::string::size_type pos = str.length()-1;
	while (str.length()>0 && (str[pos] == '\n' || str[pos] == '\r')) {
		str.erase(pos);
		pos--;
	}
}

std::string utils::quote(const std::string& str) {
	std::string rv = replace_all(str, "\"", "\\\"");
	rv.insert(0, "\"");
	rv.append("\"");
	return rv;
}

unsigned int utils::get_random_value(unsigned int max) {
	static bool initialized = false;
	if (!initialized) {
		initialized = true;
		srand(~(time(NULL) ^ getpid() ^ getppid()));
	}
	return static_cast<unsigned int>(rand() % max);
}

std::string utils::quote_if_necessary(const std::string& str) {
	std::string result;
	if (str.find_first_of(" ", 0) == std::string::npos) {
		result = str;
	} else {
		result = utils::replace_all(str, "\"", "\\\"");
		result.insert(0, "\"");
		result.append("\"");
	}
	return result;
}


std::string utils::get_content(xmlNode * node) {
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

std::string utils::get_prop(xmlNode * node, const char * prop, const char * ns) {
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

unsigned long utils::get_auth_method(const std::string& type) {
	if (type == "any")
		return CURLAUTH_ANY;
	if (type == "basic")
		return CURLAUTH_BASIC;
	if (type == "digest")
		return CURLAUTH_DIGEST;
#ifdef CURLAUTH_DIGEST_IE
	if (type == "digest_ie")
		return CURLAUTH_DIGEST_IE;
#else
# warning "proxy-auth-method digest_ie not added due to libcurl older than 7.19.3"
#endif
	if (type == "gssnegotiate")
		return CURLAUTH_GSSNEGOTIATE;
	if (type == "ntlm")
		return CURLAUTH_NTLM;
	if (type == "anysafe")
		return CURLAUTH_ANYSAFE;
	if (type != "") {
		//LOG(LOG_USERERROR, "you configured an invalid proxy authentication method: %s", type.c_str());
	}
	return CURLAUTH_ANY;
}

curl_proxytype utils::get_proxy_type(const std::string& type) {
	if (type == "http")
		return CURLPROXY_HTTP;
	if (type == "socks4")
		return CURLPROXY_SOCKS4;
	if (type == "socks5")
		return CURLPROXY_SOCKS5;
#ifdef CURLPROXY_SOCKS4A
	if (type == "socks4a")
		return CURLPROXY_SOCKS4A;
#endif

	if (type != "") {
		//LOG(LOG_USERERROR, "you configured an invalid proxy type: %s", type.c_str());
	}
	return CURLPROXY_HTTP;
}

std::string utils::escape_url(const std::string& url) {
	return replace_all(replace_all(url,"?","%3F"), "&", "%26");
}

std::string utils::unescape_url(const std::string& url) {
	return replace_all(replace_all(url,"%3F","?"), "%26", "&");
}

std::wstring utils::clean_nonprintable_characters(std::wstring text) {
	for (size_t idx=0; idx<text.size(); ++idx) {
		if (!iswprint(text[idx]))
			text[idx] = L'\uFFFD';
	}
	return text;
}

/*
 * See http://curl.haxx.se/libcurl/c/libcurl-tutorial.html#Multi-threading for a reason why we do this.
 */

#if HAVE_OPENSSL
static std::mutex * openssl_mutexes = NULL;
static int openssl_mutexes_size = 0;

static void openssl_mth_locking_function(int mode, int n, const char * file, int line) {
	if (n < 0 || n >= openssl_mutexes_size) {
		//LOG(LOG_ERROR,"openssl_mth_locking_function: index is out of bounds (called by %s:%d)", file, line);
		return;
	}
	if (mode & CRYPTO_LOCK) {
		//LOG(LOG_DEBUG, "OpenSSL lock %d: %s:%d", n, file, line);
		openssl_mutexes[n].lock();
	} else {
		//LOG(LOG_DEBUG, "OpenSSL unlock %d: %s:%d", n, file, line);
		openssl_mutexes[n].unlock();
	}
}

static unsigned long openssl_mth_id_function(void) {
	return (unsigned long)pthread_self();
}
#endif

void utils::initialize_ssl_implementation(void) {
#if HAVE_OPENSSL
	openssl_mutexes_size = CRYPTO_num_locks();
	openssl_mutexes = new std::mutex[openssl_mutexes_size];
	CRYPTO_set_id_callback(openssl_mth_id_function);
	CRYPTO_set_locking_callback(openssl_mth_locking_function);
#endif

#if HAVE_GCRYPT
	gcry_control (GCRYCTL_SET_THREAD_CBS, &gcry_threads_pthread);
	gnutls_global_init();
#endif
}

}

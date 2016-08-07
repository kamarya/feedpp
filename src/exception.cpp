#include "exception.h"

exception::exception(const std::string& errmsg) : emsg(errmsg) {
}

exception::~exception() throw() {

}

const char* exception::what() const throw() {
	return emsg.c_str();
}

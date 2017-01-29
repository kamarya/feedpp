/* Copyright (C) 2016 Behrooz Aliabadi
 * Licensed under the MIT/X Consortium License.
 * See file LICENSE for more information.
 */

#include "exception.h"

exception::exception(const std::string& errmsg, const int errcode)
: emsg(errmsg)
, ecode(errcode) {}

exception::~exception() throw() {}

const char* exception::what() const throw()
{

	if (ecode)
	{
		std::string err = "code (" + std::to_string(ecode) + ") : " + emsg;
		return err.c_str();
	}

	return emsg.c_str();
}

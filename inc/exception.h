#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#include <exception>
#include <string>

class exception : public std::exception {

 public:
  exception(const std::string& errmsg = "", const int errcode = 0);
  ~exception() throw ();
  virtual const char* what() const throw ();

 private:
  std::string emsg;
  int         ecode;

};

#endif

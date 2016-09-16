#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <exception>
#include <string>

class exception : public std::exception {

 public:
  exception(const std::string& errmsg = "");
  ~exception() throw ();
  virtual const char* what() const throw ();

 private:
  std::string emsg;

};

#endif

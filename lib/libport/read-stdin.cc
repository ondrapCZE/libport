#include <libport/config.h>

#include <boost/lexical_cast.hpp>

#include <libport/cstring>
#include <libport/detect-win32.h>

#if defined LIBPORT_WIN32 || defined WIN32
# include <libport/windows.hh>
#else
# if not defined LIBPORT_URBI_ENV_AIBO
#  include <sys/select.h>
# endif
# include <errno.h>
#endif

#include <libport/compiler.hh>
#include <libport/read-stdin.hh>
#include <libport/exception.hh>


namespace libport
{
  std::string
  read_stdin()
  {
#if not defined LIBPORT_URBI_ENV_AIBO
    char buf[1024];
# if not defined LIBPORT_WIN32 &&  not defined WIN32
    //select
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(0,&fd);
    struct timeval tv;
    tv.tv_sec = tv.tv_usec = 0;
    int r = select(1,&fd,0,0,&tv);
    if (r <= -1)
      throw exception::Exception(__PRETTY_FUNCTION__,
	  std::string("select error on stdin: ") + strerror(errno));
    else if (r>0)
    {
      r = read(0, buf, sizeof buf);
      if (r <= 0) // EOF counts as an 'error'.
      throw exception::Exception(__PRETTY_FUNCTION__,
	  std::string("read error on stdin: ") + ((r==0)?"EOF":strerror(errno)));
      else
	return std::string(buf, r);
    }
# else
    HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD r = WaitForSingleObject(hstdin, 0);
    if (r != WAIT_TIMEOUT)
    {
      DWORD bytesRead;
      if (ReadFile(hstdin, buf, sizeof buf, &bytesRead, 0))
	return std::string(buf, bytesRead);
      else
	throw exception::Exception(__PRETTY_FUNCTION__,
	    std::string("read error on stdin: ")
	      + boost::lexical_cast<std::string>(GetLastError()));
    }
# endif
#endif
    return std::string();
  }
}

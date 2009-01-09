#ifndef SCHEDULER_EXCEPTION_HXX
# define SCHEDULER_EXCEPTION_HXX

# include <cstdlib>
# include <libport/assert.hh>

namespace scheduler
{

  inline
  exception::~exception()
  {
  }

  inline std::string
  exception::what() const throw()
  {
    return "unknown exception";
  }

  inline
  void exception::rethrow() const
  {
    rethrow_();
    // GCC cannot guarantee that an overriden virtual method
    // will not return, so help it here.
    pabort("rethrow_ returned");
  }

} // namespace scheduler

#endif // SCHEDULER_EXCEPTION_HXX
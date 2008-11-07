/**
 ** Test code for libport::Deref.
 */

#include <memory>
#include <boost/shared_ptr.hpp>
#include <libport/deref.hh>
#include <libport/unit-test.hh>
#include <libport/shared-ptr.hh>

using libport::test_suite;

void
check ()
{
# define CHECK(Exp)                             \
  do {                                          \
    std::ostringstream o;                       \
    o << libport::deref << Exp;                 \
    BOOST_CHECK_EQUAL(o.str(), "123");          \
  } while(0)

  int i = 123;
  CHECK(i);
  CHECK(&i);

  const int& ic = i;
  CHECK(ic);
  CHECK(&ic);

  // FIXME: We need to test the non const types too (they fail)!
  const std::auto_ptr<int> ai(new int(123));
  CHECK(ai);

  const boost::shared_ptr<int> si(new int(123));
  CHECK(si);
}

test_suite*
init_test_suite()
{
  test_suite* suite = BOOST_TEST_SUITE("libport::deref");
  suite->add(BOOST_TEST_CASE(check));
  return suite;
}

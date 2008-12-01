#include <boost/bind.hpp>

#include <libport/dirent.h>
#include <libport/unit-test.hh>

using boost::bind;
using libport::test_suite;

static const std::string myself = "dirent";

static void test()
{
  DIR* dir = opendir(".");
  struct dirent* entry;
  bool found = false;

  while ((entry = readdir(dir)))
    found = found || myself == entry->d_name;

  BOOST_CHECK(found);

  closedir(dir);
}

test_suite*
init_test_suite()
{
  test_suite* suite = BOOST_TEST_SUITE("libport's dirent.h test suite");

  suite->add(BOOST_TEST_CASE(test));

  return suite;
}
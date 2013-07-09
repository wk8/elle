#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE prinft
#include <boost/test/unit_test.hpp>

#include <elle/printf.hh>
#include <ostream>

BOOST_AUTO_TEST_CASE(test_empty)
{
  BOOST_CHECK_EQUAL(elle::sprintf(""), "");
}

BOOST_AUTO_TEST_CASE(test_no_param)
{
  BOOST_CHECK_EQUAL(elle::sprintf(" foo bar "), " foo bar ");
}

BOOST_AUTO_TEST_CASE(test_string)
{
  BOOST_CHECK_EQUAL(elle::sprintf("foo%sbaz", "bar"), "foobarbaz");
}

BOOST_AUTO_TEST_CASE(test_too_many)
{
  BOOST_CHECK_THROW(elle::sprintf("%s", "foo", "bar"), std::exception);
}

BOOST_AUTO_TEST_CASE(test_too_few)
{
  BOOST_CHECK_THROW(elle::sprintf("%s%s", "foo"), std::exception);
}

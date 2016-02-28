#define BOOST_TEST_MODULE shader

#include <boost/test/unit_test.hpp>
#include <handle.hpp>

BOOST_AUTO_TEST_CASE(gl_handle)
{
	glrfw::detail::gl_handle handle1;
	BOOST_CHECK(handle1.x_ == 0);
	glrfw::detail::gl_handle handle2(10);
	BOOST_CHECK(handle1 != handle2);
	handle1 = glrfw::detail::gl_handle(10);
    BOOST_CHECK(handle1 == handle2);
}


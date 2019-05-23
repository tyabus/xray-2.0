////////////////////////////////////////////////////////////////////////////
//	Created		: 15.06.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_CORE_TESTING_H_INCLUDED
#define XRAY_CORE_TESTING_H_INCLUDED

#include <xray/intrusive_list.h>

#if XRAY_IN_PCH
#	error	sorry, you cant include testing.h to precompiled header, because it's OS dependent and could bring \
			to pch a lot of unneccessary stuff
#endif // #if XRAY_IN_PCH

#include <xray/os_preinclude.h>
#include <xray/os_include.h>

#pragma warning(disable:4509)

namespace xray {
namespace testing {

XRAY_CORE_API void  initialize				( core::engine * engine );
XRAY_CORE_API void  finalize				( );
XRAY_CORE_API u32   tests_failed_so_far		( );
XRAY_CORE_API bool	run_tests_command_line	( );

class XRAY_CORE_API test_base
{
public:
							test_base	() :	m_next_test(NULL) {}
	virtual					~test_base	() {}
	virtual	void			test		()	=	0;
public:
	test_base*				m_next_test;
};

template <class derived>
class suite_base
{
public:
							suite_base	();
	static 	bool			run_tests	();
	static 	void			add_test	(test_base* const test);
	static 	derived*		singleton	();

private:
	typedef	intrusive_list<test_base, test_base, &test_base::m_next_test>	tests;
	tests					m_tests;
	static	derived*		s_suite;
	static	threading::atomic32_type	s_suite_creation_flag;
	class	memory_helper;
};

} // namespace testing

template class XRAY_CORE_API intrusive_list<testing::test_base, testing::test_base, &testing::test_base::m_next_test>;

} // namespace xray

#define DEFINE_SUITE_HELPERS	public: template <class test_func>	void test (test_func*) { UNREACHABLE_CODE(); }

#define REGISTER_TEST_CLASS(test_class, suite_class)									\
	static	xray::testing::test_class_helper<test_class, suite_class>					\
			s_test_class_helper ## test_class ## suite_class(#test_class, #suite_class);

#define DEFINE_TEST(test_name, suite_class)												\
	class test_name {};																	\
	static	xray::testing::test_func_helper<test_name, suite_class>						\
			s_test_func_helper ## test_name ## suite_class(#test_name, #suite_class);	\
	template <>																			\
	void   suite_class::test<test_name> (test_name*)

#ifdef _MSC_VER
#	define	TEST_THROWS(code, awaited_exception)											\
		ASSERT	(awaited_exception != assert_untyped);										\
		xray::assert_enum previous_awaited_exception	=									\
			xray::testing::detail::set_awaited_exception (awaited_exception);				\
		__try { code; }																		\
		__except(XRAY_EXCEPTION_EXECUTE_HANDLER) {}											\
		xray::testing::detail::check_awaited_exception	(previous_awaited_exception);

#	define TEST_ASSERT_T( assert_type, expression, ... ) \
		__try { R_ASSERT_T_U(assert_type, expression, ##__VA_ARGS__); }						\
		__except (xray::debug::unhandled_exception_filter(GetExceptionCode(), GetExceptionInformation())) {}

#	define	TEST_ASSERT_CMP_T( value1, operation, value2, assert_type, ... )				\
		__try { R_ASSERT_CMP_T_U(value1, operation, value2, assert_type, ##__VA_ARGS__); }	\
		__except (xray::debug::unhandled_exception_filter(GetExceptionCode(), GetExceptionInformation())) {}

#	define TEST_CURE_ASSERT(expression, action, ...)	\
		__try {											\
			if ( !(expression) ) {						\
				static bool ignore_always = false;		\
				XRAY_ASSERT_HELPER		(				\
					ignore_always,						\
					xray::process_error_true,			\
					XRAY_MAKE_STRING(expression),		\
					xray::assert_untyped,				\
					##__VA_ARGS__						\
				);										\
				action;									\
			}											\
		}												\
		__except (xray::debug::unhandled_exception_filter(GetExceptionCode(), GetExceptionInformation())) {}

#	define TEST_CURE_ASSERT_CMP(value1, operation, value2, action, ...)					\
		__try {																				\
			if ( !((value1) operation (value2)) ) {											\
				static bool ignore_always = false;											\
				fixed_string4096 fail_message = xray::debug::detail::make_fail_message		\
												(value1, value2, ##__VA_ARGS__).c_str();	\
				XRAY_ASSERT_HELPER		(													\
					ignore_always,															\
					xray::process_error_true,												\
					XRAY_MAKE_STRING((value1) operation (value2)),							\
					xray::assert_untyped,													\
					"%s",																	\
					fail_message.c_str()													\
				);																			\
				action;																		\
			}																				\
		}																					\
		__except (xray::debug::unhandled_exception_filter(GetExceptionCode(), GetExceptionInformation())) {}
#else // #ifdef _MSC_VER
#	define TEST_THROWS(...)
#	define TEST_ASSERT_T		ASSERT_T
#	define TEST_ASSERT_CMP_T	ASSERT_CMP_T
#	define TEST_CURE_ASSERT		CURE_ASSERT
#	define TEST_CURE_ASSERT_CMP	CURE_ASSERT_CMP
#endif // #ifdef _MSC_VER

#define TEST_ASSERT(expression, ...) \
	TEST_ASSERT_T( xray::assert_untyped, expression, ##__VA_ARGS__ )

#define TEST_ASSERT_CMP(value1, operation, value2, ...)	\
	TEST_ASSERT_CMP_T(value1, operation, value2, xray::assert_untyped, ##__VA_ARGS__)

#include <xray/testing_inline.h>

#endif // #ifndef XRAY_CORE_TESTING_H_INCLUDED
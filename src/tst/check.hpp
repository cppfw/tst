#pragma once

#include <cstddef>
#include <utility>
#include <sstream>
#include <functional>

#include <utki/debug.hpp>

namespace tst{

/**
 * @brief Check for condition with additional failure information.
 * The function checks for the condition to be true.
 * In case the condition is true, the function does nothing.
 * In case the condition is false, the function prepares a failure message by
 * calling the provided function and throws an exception containing the failure information.
 * @param c - condition to check.
 * @param print - function performing output of addional failure message information.
 * @param source_location - object with source file:line information.
 */
void check(
		bool c,
		const std::function<void(std::ostream&)>& print,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	);

// smart pointers have explicit 'operator bool()', so we need to add oveloads for those

template <class type>
void check(
		const std::shared_ptr<type>& p,
		const std::function<void(std::ostream&)>& print,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	check(p != nullptr, print, std::move(source_location));
}

template <class type>
void check(
		const std::unique_ptr<type>& p,
		const std::function<void(std::ostream&)>& print,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	check(p != nullptr, print, std::move(source_location));
}

template <class type>
void check(
		const std::function<type>& p,
		const std::function<void(std::ostream&)>& print,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	check(p != nullptr, print, std::move(source_location));
}

#define CHECK_INTERNAL2(condition, print) tst::check((condition), (print), SL)

/**
 * @brief Check result.
 * The object of this class is returned from check() functions which do not
 * have the 'print' function argument. This object can be used to insert additional
 * failure information in case check has failed.
 * In case the object holds failing check result, the object will throw a check
 * failure exception when it is destroyed.
 */
class check_result{
	friend check_result check(bool, utki::source_location&&);

	bool failed = false;
	utki::source_location source_location;
	std::stringstream ss;

	check_result() : source_location(0, 0, nullptr, nullptr){}

	check_result(utki::source_location&& source_location) :
			failed(true),
			source_location(std::move(source_location))
	{}
public:
	check_result(check_result&&) = default;

	/**
	 * @brief Insert additional failure information.
	 * The operator does nothing in case check has succeeded.
	 * @param v - value to insert into the string stream.
	 * @return reference to itself.
	 */
	template <class type>
	check_result& operator<<(const type& v){
		if(this->failed){
			this->ss << v;
		}
		return *this;
	}
	
	~check_result()noexcept(false);
};

/**
 * @brief Check for condition with additional failure information.
 * The function checks for the condition to be true.
 * @param c - condition to check.
 * @param source_location - object with source file:line information.
 * @return an instance of check_result.
 */
check_result check(
		bool c,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	);

// smart pointers have explicit 'operator bool()', so we need to add oveloads for those

template <class type>
check_result check(
		const std::shared_ptr<type>& p,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	return check(p != nullptr, std::move(source_location));
}

template <class type>
check_result check(
		const std::unique_ptr<type>& p,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	return check(p != nullptr, std::move(source_location));
}

template <class type>
check_result check(
		const std::function<type>& p,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	return check(p != nullptr, std::move(source_location));
}

#define CHECK_INTERNAL1(condition) tst::check((condition), SL)

/**
 * @brief Check for condition macro.
 * This is a convenience macro which wraps a call to tst::check() function.
 * It hides the need of typing the trailing source_location argument of the function.
 */
#define CHECK(...) UTKI_GET_MACRO(__VA_ARGS__, _10, _9, _8, _7, _6, _5, _4, _3, CHECK_INTERNAL2, CHECK_INTERNAL1)(__VA_ARGS__)

/**
 * @brief Check for equality.
 * This is a convenience function which checks for equality of two values.
 * Under the hood it calls to tst::check(), but also, it automatically prints the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param print - function performing output of addional failure message information.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
void check_eq(
		const parameter& a,
		const parameter& b,
		const std::function<void(std::ostream&)>& print,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	check(
			a == b,
			[&](auto& o){
				o << "check_eq(" << a << ", " << b << ")";
				if(print){
					print(o);
				}
			},
			std::move(source_location)
		);
}

#define CHECK_EQ_INTERNAL3(a, b, print) tst::check_eq((a), (b), (print), SL)

/**
 * @brief Check for equality.
 * This is a convenience function which checks for equality of two values.
 * Under the hood it calls to tst::check(), but also, it automatically prints the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
check_result check_eq(
		const parameter& a,
		const parameter& b,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	auto ret = check(a == b, std::move(source_location));
	ret << "check_eq(" << a << ", " << b << ")";
	return ret;
}

#define CHECK_EQ_INTERNAL2(a, b) tst::check_eq((a), (b), SL)

/**
 * @brief Check for equality macro.
 * This is a convenience macro which wraps a call to tst::check_eq() function.
 * It hides the need of typing the trailing source_location argument of the function.
 */
#define CHECK_EQ(...) UTKI_GET_MACRO(__VA_ARGS__, _10, _9, _8, _7, _6, _5, _4, CHECK_EQ_INTERNAL3, CHECK_EQ_INTERNAL2)(__VA_ARGS__)

/**
 * @brief Check for inequality.
 * This is a convenience function which checks for inequality of two values.
 * Under the hood it calls to tst::check(), but also, it automatically prints the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param print - function performing output of addional failure message information.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
void check_ne(
		const parameter& a,
		const parameter& b,
		const std::function<void(std::ostream&)>& print,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	check(
			a != b,
			[&](auto& o){
				o << "check_ne(" << a << ", " << b << ")";
				if(print){
					print(o);
				}
			},
			std::move(source_location)
		);
}

#define CHECK_NE_INTERNAL3(a, b, print) tst::check_ne((a), (b), (print), SL)

/**
 * @brief Check for inequality.
 * This is a convenience function which checks for inequality of two values.
 * Under the hood it calls to tst::check(), but also, it automatically prints the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
check_result check_ne(
		const parameter& a,
		const parameter& b,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	auto ret = check(a != b, std::move(source_location));
	ret << "check_ne(" << a << ", " << b << ")";
	return ret;
}

#define CHECK_NE_INTERNAL2(a, b) tst::check_ne((a), (b), SL)

/**
 * @brief Check for inequality macro.
 * This is a convenience macro which wraps a call to tst::check_ne() function.
 * It hides the need of typing the trailing source_location argument of the function.
 */
#define CHECK_NE(...) UTKI_GET_MACRO(__VA_ARGS__, _10, _9, _8, _7, _6, _5, _4, CHECK_NE_INTERNAL3, CHECK_NE_INTERNAL2)(__VA_ARGS__)

/**
 * @brief Check for less than.
 * This is a convenience function which checks for one value being less than another value.
 * Under the hood it calls to tst::check(), but also, it automatically prints the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param print - function performing output of addional failure message information.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
void check_lt(
		const parameter& a,
		const parameter& b,
		const std::function<void(std::ostream&)>& print,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	check(
			a < b,
			[&](auto& o){
				o << "check_lt(" << a << ", " << b << ")";
				if(print){
					print(o);
				}
			},
			std::move(source_location)
		);
}

#define CHECK_LT_INTERNAL3(a, b, print) tst::check_lt((a), (b), (print), SL)

/**
 * @brief Check for less than.
 * This is a convenience function which checks for one value being less than another value.
 * Under the hood it calls to tst::check(), but also, it automatically prints the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
check_result check_lt(
		const parameter& a,
		const parameter& b,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	auto ret = check(a < b, std::move(source_location));
	ret << "check_lt(" << a << ", " << b << ")";
	return ret;
}

#define CHECK_LT_INTERNAL2(a, b) tst::check_lt((a), (b), SL)

/**
 * @brief Check for less than macro.
 * This is a convenience macro which wraps a call to tst::check_lt() function.
 * It hides the need of typing the trailing source_location argument of the function.
 */
#define CHECK_LT(...) UTKI_GET_MACRO(__VA_ARGS__, _10, _9, _8, _7, _6, _5, _4, CHECK_LT_INTERNAL3, CHECK_LT_INTERNAL2)(__VA_ARGS__)

/**
 * @brief Check for greater than.
 * This is a convenience function which checks for one value being greater than another value.
 * Under the hood it calls to tst::check(), but also, it automatically prints the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param print - function performing output of addional failure message information.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
void check_gt(
		const parameter& a,
		const parameter& b,
		const std::function<void(std::ostream&)>& print,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	check(
			a > b,
			[&](auto& o){
				o << "check_gt(" << a << ", " << b << ")";
				if(print){
					print(o);
				}
			},
			std::move(source_location)
		);
}

#define CHECK_GT_INTERNAL3(a, b, print) tst::check_gt((a), (b), (print), SL)

/**
 * @brief Check for greater than.
 * This is a convenience function which checks for one value being greater than another value.
 * Under the hood it calls to tst::check(), but also, it automatically prints the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
check_result check_gt(
		const parameter& a,
		const parameter& b,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	auto ret = check(a > b, std::move(source_location));
	ret << "check_gt(" << a << ", " << b << ")";
	return ret;
}

#define CHECK_GT_INTERNAL2(a, b) tst::check_gt((a), (b), SL)

/**
 * @brief Check for greater than macro.
 * This is a convenience macro which wraps a call to tst::check_gt() function.
 * It hides the need of typing the trailing source_location argument of the function.
 */
#define CHECK_GT(...) UTKI_GET_MACRO(__VA_ARGS__, _10, _9, _8, _7, _6, _5, _4, CHECK_GT_INTERNAL3, CHECK_GT_INTERNAL2)(__VA_ARGS__)

/**
 * @brief Check for less than or equal.
 * This is a convenience function which checks for one value being less than or equal to another value.
 * Under the hood it calls to tst::check(), but also, it automatically prints the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param print - function performing output of addional failure message information.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
void check_le(
		const parameter& a,
		const parameter& b,
		const std::function<void(std::ostream&)>& print,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	check(
			a <= b,
			[&](auto& o){
				o << "check_le(" << a << ", " << b << ")";
				if(print){
					print(o);
				}
			},
			std::move(source_location)
		);
}

#define CHECK_LE_INTERNAL3(a, b, print) tst::check_le((a), (b), (print), SL)

/**
 * @brief Check for less than or equal.
 * This is a convenience function which checks for one value being less than or equal to another value.
 * Under the hood it calls to tst::check(), but also, it automatically prints the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
check_result check_le(
		const parameter& a,
		const parameter& b,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	auto ret = check(a <= b, std::move(source_location));
	ret << "check_le(" << a << ", " << b << ")";
	return ret;
}

#define CHECK_LE_INTERNAL2(a, b) tst::check_le((a), (b), SL)

/**
 * @brief Check for less or equal than macro.
 * This is a convenience macro which wraps a call to tst::check_le() function.
 * It hides the need of typing the trailing source_location argument of the function.
 */
#define CHECK_LE(...) UTKI_GET_MACRO(__VA_ARGS__, _10, _9, _8, _7, _6, _5, _4, CHECK_LE_INTERNAL3, CHECK_LE_INTERNAL2)(__VA_ARGS__)

/**
 * @brief Check for greater than or equal.
 * This is a convenience function which checks for one value being greater than or equal to another value.
 * Under the hood it calls to tst::check(), but also, it automatically prints the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param print - function performing output of addional failure message information.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
void check_ge(
		const parameter& a,
		const parameter& b,
		const std::function<void(std::ostream&)>& print,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	check(
			a >= b,
			[&](auto& o){
				o << "check_ge(" << a << ", " << b << ")";
				if(print){
					print(o);
				}
			},
			std::move(source_location)
		);
}

#define CHECK_GE_INTERNAL3(a, b, print) tst::check_ge((a), (b), (print), SL)

/**
 * @brief Check for greater than or equal.
 * This is a convenience function which checks for one value being greater than or equal to another value.
 * Under the hood it calls to tst::check(), but also, it automatically prints the
 * input values in case of check failure.
 * @param a - fisrt value.
 * @param b - second value.
 * @param source_location - object with source file:line information.
 */
template <class parameter>
check_result check_ge(
		const parameter& a,
		const parameter& b,
		utki::source_location&& source_location
#if M_CPP >= 20
				= utki::std_source_location::current()
#endif
	)
{
	auto ret = check(a >= b, std::move(source_location));
	ret << "check_ge(" << a << ", " << b << ")";
	return ret;
}

#define CHECK_GE_INTERNAL2(a, b) tst::check_ge((a), (b), SL)

/**
 * @brief Check for greater or equal than macro.
 * This is a convenience macro which wraps a call to tst::check_ge() function.
 * It hides the need of typing the trailing source_location argument of the function.
 */
#define CHECK_GE(...) UTKI_GET_MACRO(__VA_ARGS__, _10, _9, _8, _7, _6, _5, _4, CHECK_GE_INTERNAL3, CHECK_GE_INTERNAL2)(__VA_ARGS__)

}

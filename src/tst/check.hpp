#pragma once

#include <cstddef>
#include <utility>
#include <sstream>
#include <functional>

#include <utki/debug.hpp>

namespace tst{

void check(
		bool c,
		const std::function<void(std::ostream&)>& print,
		const std::pair<const char*, size_t>& source_location
	);

#define CHECK2(condition, print) tst::check((condition), (print), SL)

void check(
		bool c,
		const std::pair<const char*, size_t>& source_location
	);

#define CHECK1(condition) tst::check((condition), SL)

#define CHECK(...) UTKI_GET_MACRO(__VA_ARGS__, _10, _9, _8, _7, _6, _5, _4, _3, CHECK2, CHECK1)(__VA_ARGS__)

template <class parameter>
void check_eq(
		const parameter& a,
		const parameter& b,
		const std::function<void(std::ostream&)>& print,
		const std::pair<const char*, size_t>& source_location
	)
{
	check(
			a == b,
			[&](auto& o){
				o << "check_eq(" << a << ", " << b << ")";
				if(print){
					o << ": ";
					print(o);
				}
			},
			source_location
		);
}

#define CHECK_EQ3(a, b, print) tst::check_eq((a), (b), (print), SL)

template <class parameter>
void check_eq(
		const parameter& a,
		const parameter& b,
		const std::pair<const char*, size_t>& source_location
	)
{
	check_eq(a, b, nullptr, source_location);
}

#define CHECK_EQ2(a, b) tst::check_eq((a), (b), SL)

#define CHECK_EQ(...) UTKI_GET_MACRO(__VA_ARGS__, _10, _9, _8, _7, _6, _5, _4, CHECK_EQ3, CHECK_EQ2)(__VA_ARGS__)

}

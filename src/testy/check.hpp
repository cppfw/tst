#pragma once

#include <cstddef>
#include <utility>
#include <sstream>
#include <functional>

namespace testy{

/**
 * @brief Source location macro.
 * Constructs an std::pair with current filename and current line number.
 */
#define SL std::make_pair(__FILE__, __LINE__)

void check(bool c, const std::pair<const char*, size_t>& source_location);

void check(
		bool c,
		const std::function<std::string()>& message_factory,
		const std::pair<const char*, size_t>& source_location
	);

template <class parameter>
void check_eq(
		const parameter& a,
		const parameter& b,
		const std::function<std::string()>& message_factory,
		const std::pair<const char*, size_t>& source_location
	)
{
	if(a == b){
		return;
	}

	std::stringstream ss;
	ss << "check_eq(" << a << ", " << b << ")";

	if(message_factory){
		ss << "; " << message_factory();
	}

	check(false, [&](){return ss.str();}, source_location);
}

template <class parameter>
void check_eq(
		const parameter& a,
		const parameter& b,
		const std::pair<const char*, size_t>& source_location
	)
{
	check_eq(a, b, nullptr, source_location);
}

}

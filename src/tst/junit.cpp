#include "junit.hxx"

using namespace tst;

void junit::set_result(
		const std::string& suite_name,
		const std::string& test_name,
		suite::status result,
		std::string&& error_message
	)
{
	std::lock_guard<decltype(this->mutex)> lock_guard(this->mutex);

	auto si = this->suites.find(suite_name);
	ASSERT(si != this->suites.end())

	auto& s = si->second;

	auto pi = s.tests.find(test_name);
	ASSERT(pi != s.tests.end())

	// TODO:
}

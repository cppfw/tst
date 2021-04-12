#include "../../src/tst/application.hpp"
#include "../../src/tst/check.hpp"

#include "../harness/testees.hpp"

namespace{
class fixture{
public:
	fixture() = default;

	fixture(const std::pair<int, int>& p) :
			a(p.first)
	{}

	fixture(const fixture&) = delete;

	int a = 10;
};
}

class application : public tst::application{
public:
	application() :
			tst::application("basic tests", "all tests pass")
	{}

	bool init()override;
};

bool application::init(){
	auto& suite = this->create_suite("factorial");

	suite.add(
			"positive_arguments_must_produce_expected_result",
			[](){
				tst::check(factorial(1) == 1, SL);
				tst::check_eq(factorial(2), 2, SL);
				tst::check_eq(factorial(3), 6, [](auto& o){o << "hello world!";}, SL);
				tst::check(factorial(8) == 40320, SL);
			}
		);
	
	suite.add_disabled("disabled_test", [](){tst::check(false, SL);});

	suite.add<fixture>(
			"factorial_of_value_from_fixture",
			[](auto& f){
				tst::check_eq(factorial(f.a), 3628800, SL);
			}
		);
	
	suite.add_disabled<fixture>("disabled_fixture_test", [](auto& f){tst::check(false, SL);});

	suite.add<std::pair<int, int>>(
			"positive_arguments_must_produce_expected_result",
			{
				{1, 1},
				{2, 2},
				{3, 6},
				{8, 40320}
			},
			[](const auto& i){
				tst::check(factorial(i.first) == i.second, SL);
			}
		);
	
	suite.add_disabled<std::pair<int, int>>(
			"disabled_param_test",
			{
				{1, 1},
				{2, 2},
				{3, 6},
				{8, 40320}
			},
			[](const auto& i){tst::check(false, SL);}
		);
	
	suite.add<std::pair<int, int>, fixture>(
			"factorial_of_value_from_fixture",
			{
				{1, 1},
				{2, 2},
				{3, 6},
				{8, 40320}
			},
			[](const auto& i, auto& f){
				tst::check(factorial(i.first) == i.second, SL);
			}
		);
	
	suite.add_disabled<std::pair<int, int>, fixture>(
			"disabled_param_fixture_test",
			{
				{1, 1},
				{2, 2},
				{3, 6},
				{8, 40320}
			},
			[](const auto& i, auto& f){tst::check(false, SL);}
		);
	
	return true;
}

std::unique_ptr<tst::application> create_application(){
	return std::make_unique<application>();
}

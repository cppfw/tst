#include "../../src/testy/init.hpp"
#include "../../src/testy/check.hpp"

#include "testees.hpp"

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

void testy::init(testy::tester& tester){
	auto& suite = tester.create_suite("factorial");

	suite.add(
			"positive_arguments_must_produce_expected_result",
			[](){
				testy::check(factorial(1) == 1, SL);
				testy::check_eq(factorial(2), 2, SL);
				testy::check_eq(factorial(3), 6, [](auto& o){o << "hello world!";}, SL);
				testy::check(factorial(8) == 40320, SL);
			}
		);
	
	suite.add_disabled("disabled_test", [](){testy::check(false, SL);});

	suite.add<fixture>(
			"factorial_of_value_from_fixture",
			[](auto& f){
				testy::check_eq(factorial(f.a), 3628800, SL);
			}
		);
	
	suite.add_disabled<fixture>("disabled_fixture_test", [](auto& f){testy::check(false, SL);});

	suite.add<std::pair<int, int>>(
			"positive_arguments_must_produce_expected_result",
			{
				{1, 1},
				{2, 2},
				{3, 6},
				{8, 40320}
			},
			[](const auto& i){
				testy::check(factorial(i.first) == i.second, SL);
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
			[](const auto& i){testy::check(false, SL);}
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
				testy::check(factorial(i.first) == i.second, SL);
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
			[](const auto& i, auto& f){testy::check(false, SL);}
		);
}

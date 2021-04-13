#include "../../src/tst/application.hpp"
#include "../../src/tst/check.hpp"
#include "../../src/tst/set.hpp"

#include "../harness/testees.hpp"

#include <thread>

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

namespace{
tst::set set1("factorial", [](tst::suite& suite){
	suite.add(
			"positive_arguments_must_produce_expected_result",
			[](){
				CHECK(factorial(1) == 1);
				tst::check_eq(factorial(2), 2, SL);
				CHECK_EQ(factorial(3), 6, [](auto& o){o << "hello world!";});
				tst::check(factorial(8) == 40320, SL);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		);
	suite.add_disabled("disabled_test", [](){tst::check(false, SL);});
});
}

namespace{
tst::set set2("factorial", [](tst::suite& suite){
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
});	
}

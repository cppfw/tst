#include "../../src/testy/init.hpp"
#include "../../src/testy/check.hpp"

#include "testees.hpp"

void testy::init(testy::tester& tester){
	auto& suite = tester.create_suite("basic");

	suite.add(
			"factorial/positive",
			[](){
				testy::check(factorial(1) == 1, SL);
				testy::check_eq(factorial(2), 2, SL);
				testy::check_eq(factorial(3), 7, [](){return "hello world!";}, SL);
				testy::check(factorial(8) == 40320, SL);
			}
		);

	suite.add<std::pair<int, int>>(
			"factorial/positive",
			{
				{1, 1},
				{2, 2},
				{3, 6},
				{8, 40320}
			},
			[](auto i){
				testy::check(factorial(i.first) == i.second, SL);
			}
		);

}

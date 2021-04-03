#include "../../src/testu/init.hpp"
#include "../../src/testu/check.hpp"

#include "testees.hpp"

void testu::init(testu::tester& t){
	t.add(
			"factorial/positive",
			[](){
				testu::check(factorial(1) == 1, FL);
				testu::check(factorial(2) == 2, FL);
				testu::check(factorial(3) == 6, FL);
				testu::check(factorial(8) == 40320, FL);
			}
		);

	t.add<std::pair<int, int>>(
			"factorial/positive",
			{
				{1, 1},
				{2, 2},
				{3, 6},
				{8, 40320}
			},
			[](auto i){
				testu::check(factorial(i.first) == i.second, FL);
			}
		);

}

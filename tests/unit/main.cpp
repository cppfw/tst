#include "../../src/testu/init.hpp"
#include "../../src/testu/assert.hpp"

#include "testees.hpp"

void testu::init(testu::tester& t){
	t.add(
			"factorial/positive",
			[](){
				testu::assert_true(factorial(1) == 1);
				testu::assert_true(factorial(2) == 2);
				testu::assert_true(factorial(3) == 6);
				testu::assert_true(factorial(8) == 40320);
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
				testu::assert_true(factorial(i.first) == i.second);
			}
		);

}

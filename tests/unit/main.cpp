#include "../../src/testy/init.hpp"
#include "../../src/testy/check.hpp"

#include "testees.hpp"

void testy::init(testy::tester& t){
	t.add(
			"factorial/positive",
			[](){
				testy::check(factorial(1) == 1, FL);
				testy::check(factorial(2) == 2, FL);
				testy::check(factorial(3) == 6, FL);
				testy::check(factorial(8) == 40320, FL);
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
				testy::check(factorial(i.first) == i.second, FL);
			}
		);

}

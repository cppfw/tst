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
				tst::check_ne(factorial(2), -1, SL);
				tst::check_lt(factorial(2), 10, SL);
				tst::check_gt(factorial(2), 1, SL);
				tst::check_le(factorial(2), 2, SL);
				tst::check_le(factorial(2), 3, SL);
				tst::check_ge(factorial(2), 2, SL);
				tst::check_ge(factorial(2), 1, SL);
				CHECK_EQ(factorial(3), 6, [](auto& o){o << "hello world!";});
				CHECK_NE(factorial(3), 7, [](auto& o){o << "hello world!";});
				CHECK_LT(factorial(3), 7, [](auto& o){o << "hello world!";});
				CHECK_GT(factorial(3), 5, [](auto& o){o << "hello world!";});
				CHECK_LE(factorial(3), 6, [](auto& o){o << "hello world!";});
				CHECK_LE(factorial(3), 7, [](auto& o){o << "hello world!";});
				CHECK_GE(factorial(3), 6, [](auto& o){o << "hello world!";});
				CHECK_GE(factorial(3), 5, [](auto& o){o << "hello world!";});
				tst::check(factorial(8) == 40320, SL);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		);
	suite.add_disabled("disabled_test", [](){tst::check(false, SL);});
});
}

namespace{
tst::set set2("factorial", [](tst::suite& suite){
	suite.add(
			"factorial_of_value_from_fixture",
			[](){
				fixture f;
				tst::check_eq(factorial(f.a), 3628800, SL);
			}
		);

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
	
	suite.add<std::pair<int, int>>(
			"factorial_of_value_from_fixture",
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
});	
}

namespace{
tst::set set3("check_pointers", [](auto& suite){
	suite.add("check_is_possible_for_simple_pointer", [](){
		int a;
		int* p = &a;
		tst::check(p, nullptr, SL);
		tst::check(p, SL);
	});

	suite.add("check_is_possible_for_shared_ptr", [](){
		auto p = std::make_shared<std::pair<int, int>>(13, 4);
		tst::check(p, nullptr, SL);
		tst::check(p, SL);
	});

	suite.add("check_is_possible_for_unique_ptr", [](){
		auto p = std::make_unique<std::pair<int, int>>(13, 4);
		tst::check(p, nullptr, SL);
		tst::check(p, SL);
	});

	suite.add("check_is_possible_for_std_function", [](){
		std::function<void()> f = [](){};
		tst::check(f, nullptr, SL);
		tst::check(f, SL);
	});
});
}
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
				tst::check(factorial(2) == 2, SL) << "hello world!";
				tst::check_eq(factorial(2), 2, SL) << "hello world!";
				tst::check_ne(factorial(2), -1, SL) << "hello world!";
				tst::check_lt(factorial(2), 10, SL) << "hello world!";
				tst::check_gt(factorial(2), 1, SL) << "hello world!";
				tst::check_le(factorial(2), 2, SL) << "hello world!";
				tst::check_le(factorial(2), 3, SL) << "hello world!";
				tst::check_ge(factorial(2), 2, SL) << "hello world!";
				tst::check_ge(factorial(2), 1, SL) << "hello world!";
				tst::check(factorial(2) == 2,  [](auto& o){o << "hello world!";}, SL);
				tst::check_eq(factorial(3), 6, [](auto& o){o << "hello world!";}, SL);
				tst::check_ne(factorial(3), 7, [](auto& o){o << "hello world!";}, SL);
				tst::check_lt(factorial(3), 7, [](auto& o){o << "hello world!";}, SL);
				tst::check_gt(factorial(3), 5, [](auto& o){o << "hello world!";}, SL);
				tst::check_le(factorial(3), 6, [](auto& o){o << "hello world!";}, SL);
				tst::check_le(factorial(3), 7, [](auto& o){o << "hello world!";}, SL);
				tst::check_ge(factorial(3), 6, [](auto& o){o << "hello world!";}, SL);
				tst::check_ge(factorial(3), 5, [](auto& o){o << "hello world!";}, SL);
#if M_CPP >= 20
				tst::check(factorial(2) == 2) << "hello world!";
				tst::check_eq(factorial(2), 2) << "hello world!";
				tst::check_ne(factorial(2), -1) << "hello world!";
				tst::check_lt(factorial(2), 10) << "hello world!";
				tst::check_gt(factorial(2), 1) << "hello world!";
				tst::check_le(factorial(2), 2) << "hello world!";
				tst::check_le(factorial(2), 3) << "hello world!";
				tst::check_ge(factorial(2), 2) << "hello world!";
				tst::check_ge(factorial(2), 1) << "hello world!";
				tst::check(factorial(2) == 2,  [](auto& o){o << "hello world!";});
				tst::check_eq(factorial(3), 6, [](auto& o){o << "hello world!";});
				tst::check_ne(factorial(3), 7, [](auto& o){o << "hello world!";});
				tst::check_lt(factorial(3), 7, [](auto& o){o << "hello world!";});
				tst::check_gt(factorial(3), 5, [](auto& o){o << "hello world!";});
				tst::check_le(factorial(3), 6, [](auto& o){o << "hello world!";});
				tst::check_le(factorial(3), 7, [](auto& o){o << "hello world!";});
				tst::check_ge(factorial(3), 6, [](auto& o){o << "hello world!";});
				tst::check_ge(factorial(3), 5, [](auto& o){o << "hello world!";});
#endif
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
			{tst::flag::no_parallel},
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
tst::set empty_set("factorial", [](auto&){});
}

namespace{
tst::set set3("check_pointers", [](auto& suite){
	suite.add("check_is_possible_for_simple_pointer", [](){
		int a;
		int* p = &a;
		tst::check(p, nullptr, SL);
		tst::check(p, SL) << "Hello world!";
#if M_CPP >=20
		tst::check(p, nullptr);
		tst::check(p) << "Hello world!";
#endif
	});

	suite.add("check_is_possible_for_shared_ptr", [](){
		auto p = std::make_shared<std::pair<int, int>>(13, 4);
		tst::check(p, nullptr, SL);
		tst::check(p, SL) << "Hello world!";
#if M_CPP >=20
		tst::check(p, nullptr);
		tst::check(p) << "Hello world!";
#endif
	});

	suite.add("check_is_possible_for_unique_ptr", [](){
		auto p = std::make_unique<std::pair<int, int>>(13, 4);
		tst::check(p, nullptr, SL);
		tst::check(p, SL) << "Hello world!";
#if M_CPP >=20
		tst::check(p, nullptr);
		tst::check(p) << "Hello world!";
#endif
	});

	suite.add("check_is_possible_for_std_function", [](){
		std::function<void()> f = [](){};
		tst::check(f, nullptr, SL);
		tst::check(f, SL) << "Hello world!";
#if M_CPP >=20
		tst::check(f, nullptr);
		tst::check(f) << "Hello world!";
#endif
	});
});
}

namespace{
tst::set empty_set_with_empty_suite("empty_suite", [](auto&){});
}

namespace{
tst::set parametrized_set("paramterized_by_string", [](auto& suite){
	suite.template add<std::string>(
		"string_has_non_zero_length",
		{
			"hello",
			"world"
		},
		[](auto& p){
#if M_CPP >= 20
			tst::check_ne(p.size(), 0);
#else
			tst::check_ne(p.size(), size_t(0), SL);
#endif
		}
	);
});
}

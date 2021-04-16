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

namespace{
struct some_unknown_exception{};
}

class application : public tst::application{
public:
	application() :
			tst::application("failing tests", "some tests are failing")
	{}

	void init()override;
};

void application::init(){
	auto& suite = this->get_suite("factorial");

	suite.add(
			"positive_arguments_must_produce_expected_result",
			[](){
				tst::check(factorial(1) == 1, SL);
				tst::check_eq(factorial(2), 2, SL);
				tst::check_eq(factorial(3), 6, [](auto& o){o << "hello world!";}, SL);
				tst::check(factorial(8) == 40320, SL);
				throw std::runtime_error("thrown by test");
			}
		);
	
	suite.add(
			"test_which_throws_unknown_exception",
			[](){
				throw some_unknown_exception();
			}
		);

	suite.add(
			"test_which_fails_check_eq_with_custom_message",
			[](){
				tst::check_eq(factorial(3), 7, [](auto& o){o << "hello world!";}, SL);
			}
		);

	suite.add_disabled("disabled_test", [](){tst::check(false, SL);});

	suite.add(
			"factorial_of_value_from_fixture",
			[](){
				fixture f;
				tst::check_eq(factorial(f.a), 3628801, SL); // will fail
			}
		);

	suite.add<std::pair<int, int>>(
			"positive_arguments_must_produce_expected_result",
			{
				{1, 1},
				{2, 2},
				{3, 7}, // will fail
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
				{2, 3}, // will fail
				{3, 6},
				{8, 40320}
			},
			[](const auto& i){tst::check(false, SL);}
		);
	
	suite.add<std::pair<int, int>>(
			"factorial_of_value_from_fixture",
			{
				{1, 2}, // will fail
				{2, 2},
				{3, 6},
				{8, 40320}
			},
			[](const auto& i){
				tst::check(factorial(i.first) == i.second, SL);
			}
		);
}

std::unique_ptr<tst::application> tst::create_application(){
	return std::make_unique<::application>();
}

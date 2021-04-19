#include <tst/set.hpp>
#include <tst/check.hpp>

namespace{
tst::set some_set("some_suite",[](tst::suite& s){
	s.add(
			"some_test",
			[](){
				tst::check(std::max(3, 4) == 4, SL);
			}
		);
});
}

#include <tst/set.hpp>
#include <tst/check.hpp>

namespace{
const tst::set("test_set", [](tst::suite& suite){
    suite.add("sample_test", [](){
        std::string s = "Hi!";
        tst::check_eq(s.size(), size_t(3), SL);
    });
});
}

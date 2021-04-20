#include "../../src/tst/set.hpp"
#include "../../src/tst/check.hpp"

namespace{
tst::set set("failing_checks", [](auto& suite){
    suite.add("check", [](){
        tst::check(false, SL) << "Hello world!";
    });

    suite.add("check_print", [](){
        tst::check(false, [](auto&o){o << "failed!";}, SL);
    });

    suite.add("check_eq", [](){
        tst::check_eq(1, 2, SL) << "Hello world!";
    });

    suite.add("check_eq_print", [](){
        tst::check_eq(1, 2, [](auto&o){o << "failed!";}, SL);
    });

    suite.add("check_ne", [](){
        tst::check_ne(2, 2, SL) << "Hello world!";
    });

    suite.add("check_ne_print", [](){
        tst::check_ne(2, 2, [](auto&o){o << "failed!";}, SL);
    });

    suite.add("check_lt", [](){
        tst::check_lt(2, 2, SL) << "Hello world!";
    });

    suite.add("check_lt_print", [](){
        tst::check_lt(2, 2, [](auto&o){o << "failed!";}, SL);
    });

    suite.add("check_gt", [](){
        tst::check_gt(2, 2, SL) << "Hello world!";
    });

    suite.add("check_gt_print", [](){
        tst::check_gt(2, 2, [](auto&o){o << "failed!";}, SL);
    });

    suite.add("check_le", [](){
        tst::check_le(2, 1, SL) << "Hello world!";
    });

    suite.add("check_le_print", [](){
        tst::check_le(2, 1, [](auto&o){o << "failed!";}, SL);
    });

    suite.add("check_ge", [](){
        tst::check_ge(2, 3, SL) << "Hello world!";
    });

    suite.add("check_ge_print", [](){
        tst::check_ge(2, 3, [](auto&o){o << "failed!";}, SL);
    });
});
}

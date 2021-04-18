After a closer look at `Boost.UT` I could come up with some _subjective and opinionated_ comparison of `tst` and `Boost.UT` at least on several things.

I don't think this kind of comparison belongs to the `tst` github repository, so I just post it here.

## JUnit report generation

As far as I could understand, `Boost.UT` does not generate JUnit XML report out of the box. Although, it is certainly possible to implement a "plugin" for that.

`tst` is able to generate JUnit XML reports out of the box. The name of the generated XML file is set via command line argument to the test runner application.

## Parallel tests running

As far as I could understand, `Boost.UT` does not support that too, out of the box.

`tst` allows running tests in a number of parallel threads. The number of threads to use is set via command line argument to the test runner application.

## Command line arguments

`Boost.UT` does not provide CLI argumetns parsing. Well, it is not a CLI args parsing library anyway.

`tst` is integrated with CLI arguments parsing library, which it uses for parsing `tst`'s default CLI arguments. It also allows user to add custom CLI arguments to the parser to handle them and configure the test run. E.g. some test cases might need to know some directory where test data files reside, that can be supplied via CLI. So, user gets CLI for free.

## Suite names

`Boost.UT` allows declaring tests inside of suites. But I could not find how to set the name of the suite. I'd expect the suites to be named and be discoverable. Instead, as I understood, `Boost.UT`'s test suites are essentially same as `tst`'s test sets (`tst::set`).

`tst` structures test cases into suites. So, it is possible to specify to run only tests from specific test suite. See `tst`'s run lists feature.

## Custom failure message

`Boost.UT` uses same approach as `GoogleTest`, when the assertion function returns a stream to which values can be inserted.
```cpp
expect(arg > 0) << "arg = " << arg;
```
It looks simple and convenient. Although, there is a small, and, apparently, rare problem with it. In case the check passes, all the arguments of the `<<` operator of the stream are still invoked and inserted to a stream (in case of successful check the stream just does nothing). So, there can be undesired double invocation of something, for example if trying to output some return value of a fucntion: `expect(arg > 0) << func();`.

`tst` uses another approach, with supplying a callback function to perform the stream output, which is invoked **only** in case the check fails. And since the test will be stopped on the failed check, there is no risk that invoking some functions during those stream outputs will affect subsequent test execution.
```cpp
int a = 3;
tst::check(a == 4, [&](auto& o){o << "a = " << a;}, SL);
```
It looks a bit more awkward, but in return it is safer in that way.

## Syntax of test cases

This is a matter of preferrance, but consider:

`Boost.UT`
```cpp
"hello world"_test = [] {
	//...
};
```
What we see here? A string with custom `_test` literal suffix, perhaps it creates some rvalue object. Then we assign a lambda function to it. Basically it is not very intuitive to me what's happening here. When is the test executed? Is the string a test id? Why do we assign lambda function to a string?

`tst`
```cpp
suite.add(
	"positive_arguments_must_produce_expected_result",
	[](){
		//...
	}
);
```
What we see here? Well, we have `suite` to which we _add_ a string-named lambda function. I.e. adding a test, what else can we add to the suite?
Much more intuitive, in my opinion.

## Syntax of parametrized tests

In `Boost.UT` there is a [number of different styles](https://github.com/boost-ext/ut/blob/master/example/parameterized.cpp) to add a parametrized test case. All of them are pretty cryptic due to heavy usage of overloaded operators of custom "non-public" classes. Except for the `for`-loop method, in all other methods the list of parameter values goes after the test procedure definition. I find this inconvenient, as I want to see list of parameter value next to the test name. This is what I used to from the times I was coding a lot of unit tests in C#.

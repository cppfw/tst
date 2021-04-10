#include "reporter.hxx"

#include <fstream>

#include "settings.hxx"

using namespace tst;

void reporter::report(
		const full_id& id,
		suite::status result,
		std::string&& message
	)
{
	std::lock_guard<decltype(this->mutex)> lock_guard(this->mutex);

	auto si = this->suites.find(id.suite);
	ASSERT(si != this->suites.end())

	auto& s = si->second;

	auto pi = s.tests.find(id.test);
	ASSERT(pi != s.tests.end())

	auto& info = pi->second;

	info.result = result;
	info.message = std::move(message);

	switch(result){
		case decltype(result)::passed:
			++s.num_passed;
			++this->num_passed;
			break;
		case decltype(result)::failed:
			++s.num_failed;
			++this->num_failed;
			break;
		case decltype(result)::errored:
			++s.num_errors;
			++this->num_errors;
			break;
		default:
			break;
	}
}

void reporter::print_num_tests_passed(std::ostream& o)const{
	if(settings::inst().is_cout_terminal){
		o << "\e[1;32m" << this->num_passed << "\e[0m";
	}else{
		o << this->num_passed;
	} 
	o << " test(s) passed" << std::endl;
}

void reporter::print_num_tests_disabled(std::ostream& o)const{
	if(this->num_disabled == 0){
		return;
	}

	if(settings::inst().is_cout_terminal){
		std::cout << "\e[0;33m" << this->num_disabled << "\e[0m";
	}else{
		std::cout << this->num_disabled;
	}
	std::cout << " test(s) disabled" << std::endl;
}

void reporter::print_num_tests_failed(std::ostream& o)const{
	size_t num = this->num_failed + this->num_errors;
	if(num == 0){
		return;
	}

	if(settings::inst().is_cout_terminal){
		std::cout << "\e[1;31m" << num  << "\e[0m";
	}else{
		std::cout << num;
	}
	std::cout << " test(s) failed" << std::endl;
}

void reporter::print_outcome(std::ostream& o)const{
	if(this->is_failed()){
		// print FAILED word
		if(tst::settings::inst().is_cout_terminal){
			o << "\t\e[1;31mFAILED\e[0m" << std::endl;
		}else{
			o << "\tFAILED" << std::endl;
		}
	}else{
		// print PASSED word
		if(tst::settings::inst().is_cout_terminal){
			o << "\t\e[1;32mPASSED\e[0m" << std::endl;
		}else{
			o << "\tPASSED" << std::endl;
		}
	}
}

void reporter::write_junit_report(const std::string& file_name)const{
	std::ofstream f(file_name, std::ios::binary);

	f << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << '\n';
	f << "<testsuites>" << '\n';

	for(const auto& si : this->suites){
		auto& s = si.second;
		f << "\t<testsuite"
				" name='" << si.first << "'"
				" tests='" << s.size() << "'"
				" disabled='" << s.num_disabled << "'"
				" failures='" << this->num_failed << "'"
				" errors='" << this->num_errors << "'>" << '\n';
		
		for(const auto& ti : s.tests){
			auto& t = ti.second;

			f << "\t\t<testcase"
					" name='" << ti.first << "'"
					" status='" << suite::status_to_string(t.result) << '\'';

			switch(t.result){
				case suite::status::errored:
				case suite::status::failed:
					f << '>' << '\n';
					f << "\t\t\t<" << (t.result == suite::status::failed ? "failure" : "error")
							<< " message='" << t.message << "'/>" << '\n';
					f << "\t\t</testcase>";
					break;
				default:
					f << "/>";
			}

			f << '\n';
		}

		f << "\t</testsuite>" << '\n';
	}

	f << "</testsuites>" << '\n';
	f.flush();
}

#include "util.hxx"

#include <algorithm>
#include <sstream>

#include "settings.hxx"

void tst::validate_id(const std::string& id){
	auto i = std::find_if(
			id.begin(),
			id.end(),
			[](std::remove_reference_t<decltype(id)>::value_type c){
				return !is_valid_id_char(c);
			}
		);
	if(i != id.end()){
		std::stringstream ss;
		ss << "test case id validation failed: character '" << *i << "' is not allowed; id = " << id;
		throw std::invalid_argument(ss.str());
	}
}

void tst::print_warning(std::ostream& o, const std::string& message){
	std::stringstream ss;
	if(settings::inst().colored_output){
		ss << "\033[1;35mwarning\033[0m: ";
	}else{
		ss << "warning: ";
	}
	ss << message << '\n';
	o << ss.str();
}

#pragma once

#include <utki/singleton.hpp>
#include <utki/util.hpp>

namespace tst{

struct settings : public utki::singleton<settings>{
	bool is_cout_terminal = utki::is_cout_terminal();
};

}

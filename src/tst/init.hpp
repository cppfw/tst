#pragma once

#include <utki/config.hpp>

#include "tester.hpp"

namespace tst{

#if M_OS == M_OS_WINDOWS
__declspec(dllexport)
#endif
bool init(tst::tester& t);

}

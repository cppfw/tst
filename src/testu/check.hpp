#pragma once

namespace testu{

struct file_line{
	const char* file;
	unsigned line;
};

#define FL {__FILE__, __LINE__}

void check(bool c, file_line fl);

}

#include <clargs/parser.hpp>

int main(int argc, const char** argv){
	bool help = false;

	clargs::parser p;
	p.add("help", "display help information", [&help](){help = true;});

	p.parse(argc, argv);

	return 0;
}

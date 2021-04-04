#include "testees.hpp"

int factorial(int n){
	int ret = 1;
	for(int i = 2; i <= n; ++i){
		ret *= i;
	}
	return ret;
}

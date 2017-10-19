#include <iostream>
#include "function.hpp"

int main(void)
{
	std::function<Function<float>(Function<float>)> func = [](Function<float> x) -> Function<float>
	{
		return x;
	};
	
	Function<float> test(func);
#if 0
	test = [](Function<float> x) -> Function<float>
	{
		return Function<float>::sin(x);
	};
#endif
	try{
		std::cout << test.f(0.0f) << std::endl;
	} catch (const char* what) {
		std::cout << "Error : " << what << std::endl;
	}

	//std::cout << "func(0) = " << test.f(0.0f) << std::endl;
	return 0;
}

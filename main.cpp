#include "function.hpp"

int main(void)
{
	using namespace Function<T>;	
	Function<float> test = [](Function<float> x)
	{
		return 2 * x * x + 3 + x + 5;
	}



	return 0;
}

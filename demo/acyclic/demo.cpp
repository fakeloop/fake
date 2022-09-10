#include <iostream>

namespace demo
{
	
	void simple();
	void sequence();
	void branch();
	void exception();
	void aspect();
	void awaiter();
	void awaited();
	void coroutine();
	void nested();
	
}

int main(int _argc, char* _argv[])
{
	demo::simple(), std::cout << std::endl;
	demo::sequence(), std::cout << std::endl;
	demo::branch(), std::cout << std::endl;
	demo::exception(), std::cout << std::endl;
	demo::aspect(), std::cout << std::endl;
	demo::awaiter(), std::cout << std::endl;
	demo::awaited(), std::cout << std::endl;
	demo::coroutine(), std::cout << std::endl;
	demo::nested(), std::cout << std::endl;
	
	return 0;
}

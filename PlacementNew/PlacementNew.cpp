#include <iostream>

#include <malloc.h>

struct Node
{
	int _a;

	Node(const int& a)
	{
		_a = a;
	}
};

int main()
{
	Node* buffer = (Node*)malloc(sizeof(Node) * 10);
	Node* copyBuffer = (Node*)malloc(sizeof(Node) * 10);

	for (size_t i = 0; i < 10; i++)
	{
		new (buffer + i) Node(i);
	}

	for (size_t i = 0; i < 10; i++)
	{
		new (copyBuffer + i) Node(buffer[i]);
	}

	for (size_t i = 0; i < 10; i++)
	{
		std::cout << buffer[i]._a << std::endl;
		std::cout << copyBuffer[i]._a << std::endl;
	}

	std::cin.get();
}

#pragma once

#include <string>
#include <stack>

constexpr char OPENING_PARENTHESES = '(';
constexpr char CLOSING_PARENTHESES = ')';

namespace ParenthesesBalance
{
	bool IsBalanced(const std::string& experssion)
	{
		if (experssion.empty())
			return false;

		std::stack<char> stack;

		for (size_t i = 0; i < experssion.length(); i++)
		{
			const char& character = experssion.at(i);

			if (character == OPENING_PARENTHESES)
			{
				stack.push(character);
			}
			else if (character == CLOSING_PARENTHESES)
			{
				if (stack.empty())
					return false;

				if (stack.top() == OPENING_PARENTHESES)
				{
					stack.pop();
				}
				else
				{
					return false;
				}
			}
		}

		return stack.empty();
	}
}
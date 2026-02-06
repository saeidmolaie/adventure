#pragma once

#include <malloc.h>
#include <stdexcept>

template<typename TObject, size_t CAPACITY>
class StaticArray
{

private:
	TObject* _buffer;

public:
	StaticArray()
	{
		_buffer = Allocate(CAPACITY);

		for (size_t i = 0; i < CAPACITY; i++)
		{
			new (_buffer + i) TObject();
		}
	}

	~StaticArray()
	{
		for (size_t i = 0; i < CAPACITY; i++)
		{
			_buffer[i].~TObject();
		};

		std::free(_buffer);
	}

	StaticArray(const StaticArray& other)
	{
		_buffer = Allocate(CAPACITY);

		for (size_t i = 0; i < CAPACITY; i++)
		{
			new (_buffer + i) TObject(other._buffer[i]);
		}
	}

	StaticArray& operator=(const StaticArray& other)
	{
		if (this != &other)
		{
			for (size_t i = 0; i < CAPACITY; i++)
			{
				new (_buffer + i) TObject(other._buffer[i]);
			}
		}

		return *this;
	}

	StaticArray(StaticArray&& other)
	{
		_buffer = other._buffer;
		other._buffer = nullptr;
	}

	StaticArray& operator=(StaticArray&& other)
	{
		if (this != &other)
		{
			for (size_t i = 0; i < CAPACITY; i++)
			{
				_buffer[i].~TObject();
			};

			std::free(_buffer);

			_buffer = other._buffer;
			other._buffer = nullptr;
		}

		return *this;
	}

public:
	TObject& operator[](const size_t index)
	{
		return _buffer[index];
	}

	const TObject& operator[](const size_t index) const
	{
		return _buffer[index];
	}

	TObject& At(const size_t index)
	{
		if (index >= CAPACITY)
			throw std::out_of_range(
				"StaticArray index out of range.");

		return _buffer[index];
	}

	const TObject& At(const size_t index) const
	{
		if (index >= CAPACITY)
			throw std::out_of_range(
				"StaticArray index out of range.");

		return _buffer[index];
	}

	constexpr size_t Capacity() const
	{
		return CAPACITY;
	}

private:
	TObject* Allocate(const size_t capacity)
	{
		return (TObject*)std::malloc(sizeof(TObject) * capacity);
	}
};
#pragma once

#include <malloc.h> 
#include <cstring>
#include <stdexcept>
#include <type_traits>

static constexpr size_t DEFAULT_CAPACITY = 10;
static constexpr size_t GROWTH_FACTOR = 2;
static constexpr size_t SHRINK_DIVISOR = 4;

template<typename TObject>
class DynamicArray
{

private:
	size_t _minCapacity = 0;
	size_t _capacity = 0;
	size_t _size = 0;

	TObject* _buffer = nullptr;

public:
	DynamicArray()
	{
	}

	DynamicArray(size_t capacity) : _minCapacity(capacity)
	{
		_capacity = _minCapacity;
		_buffer = Allocate(_capacity);
	}

	~DynamicArray()
	{
		FreeCurrentBuffer();
	}

	DynamicArray(const DynamicArray& other)
	{
		_minCapacity = other._minCapacity;
		_capacity = other._capacity;
		_size = other._size;
		_buffer = Allocate(_capacity);

		for (size_t i = 0; i < _size; i++)
		{
			new (_buffer + i) TObject(other._buffer[i]);
		}
	}

	DynamicArray& operator=(const DynamicArray& other)
	{
		if (this != &other)
		{
			_minCapacity = other._minCapacity;
			_capacity = other._capacity;
			_size = other._size;

			FreeCurrentBuffer();

			_buffer = Allocate(_capacity);

			if constexpr (std::is_trivially_copyable_v<TObject>)
			{
				std::memcpy(_buffer, other._buffer, sizeof(TObject) * _size);
			}
			else
			{
				for (size_t i = 0; i < _size; i++)
				{
					new (_buffer + i) TObject(other._buffer[i]);
				}
			}
		}

		return *this;
	}

	DynamicArray(DynamicArray&& other)
	{
		_minCapacity = other._minCapacity;
		_capacity = other._capacity;
		_size = other._size;
		_buffer = other._buffer;

		other._buffer = nullptr;
		other._size = 0;
		other._capacity = 0;
		other._minCapacity = 0;
	}

	DynamicArray& operator=(DynamicArray&& other) noexcept
	{
		if (this != &other)
		{
			FreeCurrentBuffer();

			_minCapacity = other._minCapacity;
			_capacity = other._capacity;
			_size = other._size;
			_buffer = other._buffer;

			other._buffer = nullptr;
			other._size = 0;
			other._capacity = 0;
			other._minCapacity = 0;
		}

		return *this;
	}

public:
	void Insert(const TObject& object)
	{
		if (ShouldGrow())
		{
			size_t newCapacity =
				(_capacity == 0) ? DEFAULT_CAPACITY : _capacity * GROWTH_FACTOR;

			TObject* newBuffer = nullptr;

			if (_buffer == nullptr)
			{
				newBuffer = Allocate(newCapacity);
			}
			else
			{
				newBuffer = Reallocate(_buffer, newCapacity);
				FreeCurrentBuffer();
			}

			_buffer = newBuffer;
			_capacity = newCapacity;
		}

		new (_buffer + _size) TObject(object);
		_size++;
	}

	void Remove(const TObject& object)
	{
		for (size_t i = 0; i < _size; i++)
		{
			if (object == _buffer[i])
			{
				for (size_t j = i; j < _size - 1; j++)
				{
					new (_buffer + i) TObject(std::move(_buffer[j + 1]));
				}

				_buffer[_size - 1].~TObject();
				_size--;
				return;
			}
		}
	}

	void TrimExcess()
	{
		if (!ShouldShrink())
			return;

		size_t newCapacity = _minCapacity;

		if (_minCapacity == 0)
			newCapacity = DEFAULT_CAPACITY;

		if (_capacity > newCapacity)
		{
			newCapacity = _size;
		}

		TObject* newBuffer = Reallocate(_buffer, newCapacity);

		FreeCurrentBuffer();

		_buffer = newBuffer;
		_capacity = newCapacity;
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
		if (index >= _capacity)
			throw std::out_of_range(
				"DynamicArray index out of range.");

		return _buffer[index];
	}

	const TObject& At(const size_t index) const
	{
		if (index >= _capacity)
			throw std::out_of_range(
				"DynamicArray index out of range.");

		return _buffer[index];
	}

	size_t Capacity() const
	{
		return _capacity;
	}

	size_t Size() const
	{
		return _size;
	}

private:
	bool ShouldGrow()
	{
		return _capacity == 0 || _size == _capacity;
	}

	bool ShouldShrink()
	{
		return _capacity > DEFAULT_CAPACITY && _size <= _capacity / SHRINK_DIVISOR;
	}

private:
	void FreeCurrentBuffer()
	{
		if (_buffer == nullptr)
			return;

		for (size_t i = 0; i < _size; i++)
		{
			_buffer[i].~TObject();
		};

		std::free(_buffer);
		_buffer = nullptr;
	}

	TObject* Reallocate(TObject* source, const size_t newCapacity)
	{
		TObject* newBuffer = Allocate(newCapacity);

		if constexpr (std::is_trivially_copyable_v<TObject>)
		{
			std::memcpy(newBuffer, source, sizeof(TObject) * newCapacity);
		}
		else
		{
			for (size_t i = 0; i < _size; i++)
			{
				new (newBuffer + i) TObject(std::move(source[i]));
			}
		}

		return newBuffer;
	}

	TObject* Allocate(const size_t capacity)
	{
		return (TObject*)std::malloc(sizeof(TObject) * capacity);
	}
};
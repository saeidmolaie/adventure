#pragma once

#include <stdexcept>

template< typename TObject >
class Stack
{
	TObject* _buffer = nullptr;
	size_t _capacity = 0;
	size_t _head     = 0;

	public:
		explicit Stack(const size_t capacity) noexcept
		{
			_capacity = capacity;
			_buffer   = Allocate(_capacity);

			if (_buffer == nullptr)
				throw std::bad_alloc();
		}

		Stack(const Stack& other)
		{
			CopyFrom(other);
		}

		Stack& operator=(const Stack& other)
		{
			CopyFrom(other);
			return *this;
		}

		Stack(Stack&& other) noexcept
		{
			MoveFrom(other);
		}

		Stack& operator=(Stack&& other) noexcept
		{
			MoveFrom(other);
			return *this;
		}

		~Stack() noexcept
		{
			Deallocate();
		}

		const TObject& Top() const
		{
			ThrowIfEmpty();
			return _buffer[_head - 1];
		}

		void Push(TObject&& object)
		{
			ThrowIfFull();
			new(_buffer + _head) TObject(std::move(object));

			_head++;
		}

		TObject Pop()
		{
			ThrowIfEmpty();
			TObject object = std::move(_buffer[_head - 1]);

			_buffer[_head - 1].~TObject();
			_head--;

			return object;
		}

		bool Full() const noexcept
		{
			return _head == _capacity;
		}

		bool Empty() const noexcept
		{
			return _head == 0;
		}

		size_t Size() const noexcept
		{
			return _head;
		}

		size_t Capacity() const noexcept
		{
			return _capacity;
		}

	private:
		void CopyFrom(const Stack& other)
		{
			if (this == &other)
				return;

			Deallocate();

			_capacity = other._capacity;
			_head     = other._head;
			_buffer   = Allocate(_capacity);

			for (size_t index = 0; index < _head; index++)
			{
				new(_buffer + index) TObject(other._buffer[index]);
			}
		}

		void MoveFrom(Stack&& other)
		{
			if (this == &other)
				return;

			Deallocate();

			_capacity = other._capacity;
			_head     = other._head;
			_buffer   = other._buffer;

			other._capacity = 0;
			other._head     = 0;
			other._buffer   = nullptr;
		}

		void ThrowIfEmpty() const
		{
			if (Empty())
				throw std::out_of_range("Stack is empty!");
		}

		void ThrowIfFull() const
		{
			if (Full())
				throw std::out_of_range("Stack is Full!");
		}

		void Deallocate()
		{
			if (_buffer == nullptr)
				return;

			if (!Empty())
			{
				for (int index = 0; index < _head; index++)
				{
					_buffer[index].~TObject();
				}
			}

			::operator delete(_buffer);
			_buffer = nullptr;
		}

		static TObject* Allocate(const size_t capacity)
		{
			return static_cast<TObject*>(
				operator new(sizeof(TObject) * capacity));
		}
};
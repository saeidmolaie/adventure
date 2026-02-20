#pragma once

#include <stdexcept>

static constexpr size_t DEFAULT_CAPACITY = 10;

template< typename TObject >
class DynamicArray
{
	private:
		size_t _minCapacity = 0;
		size_t _capacity    = 0;
		size_t _size        = 0;
		TObject* _buffer    = nullptr;

	public:
		DynamicArray()
		{
		}

		explicit DynamicArray(const size_t capacity)
		{
			_minCapacity = capacity;
			_capacity    = capacity;
			_buffer      = Allocate(capacity);
		}

		DynamicArray(const DynamicArray& other)
		{
			CopyFrom(other);
		}

		DynamicArray& operator=(const DynamicArray& other)
		{
			CopyFrom(other);
			return *this;
		}

		DynamicArray(DynamicArray&& other) noexcept
		{
			MoveFrom(other);
		}

		DynamicArray& operator=(DynamicArray&& other) noexcept
		{
			MoveFrom(other);
			return *this;
		}

		~DynamicArray()
		{
			Deallocate();
		}

	public:
		TObject& operator[](const size_t index)
		{
			return At(index);
		}

		const TObject& operator[](const size_t index) const
		{
			return At(index);
		}

		TObject& At(const size_t index)
		{
			ValidateIndex(index);
			return _buffer[index];
		}

		const TObject& At(const size_t index) const
		{
			ValidateIndex(index);
			return _buffer[index];
		}

		size_t Size() const noexcept
		{
			return _size;
		}

		size_t Capacity() const noexcept
		{
			return _capacity;
		}

		void Insert(const TObject& object)
		{
			if (ShouldGrow())
			{
				if (_capacity == 0 || _buffer == nullptr)
				{
					_capacity = DEFAULT_CAPACITY;
					_buffer   = Allocate(DEFAULT_CAPACITY);

					if (_buffer == nullptr)
						throw std::bad_alloc();
				}
				else
				{
					const size_t newCapacity =
							_size > 100
								? _capacity * 1.5
								: _capacity * 2;

					TObject* newBuffer = Reallocate(_buffer, _size, newCapacity);

					if (newBuffer == nullptr)
						throw std::bad_alloc();

					Deallocate();

					_capacity = newCapacity;
					_buffer   = newBuffer;
				}
			}

			new(_buffer + _size) TObject(object);
			_size++;
		}

		void Remove(const TObject& object)
		{
			for (size_t index = 0; index < _size; index++)
			{
				if (_buffer[index] == object)
				{
					for (size_t j = index; j < _size - 1; ++j)
					{
						_buffer[j] = std::move(_buffer[j + 1]);
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

			if (newCapacity < _capacity)
				newCapacity = _capacity;

			if (newCapacity > _size)
				newCapacity = _size;

			if (newCapacity == _capacity)
				return;

			TObject* newBuffer = Reallocate(_buffer, _size, newCapacity);

			if (newBuffer == nullptr)
				throw std::bad_alloc();

			Deallocate();

			_capacity = newCapacity;
			_buffer   = newBuffer;
		}

	private:
		void CopyFrom(const DynamicArray& other)
		{
			if (this == &other)
				return;

			Deallocate();

			_minCapacity = other._minCapacity;
			_capacity    = other._capacity;
			_size        = other._size;
			_buffer      = Allocate(_capacity);

			for (size_t index = 0; index < _size; index++)
			{
				new(_buffer + index) TObject(other._buffer[index]);
			}
		}

		void MoveFrom(DynamicArray&& other)
		{
			if (this == &other)
				return;

			Deallocate();

			_minCapacity = other._minCapacity;
			_capacity    = other._capacity;
			_size        = other._size;
			_buffer      = other._buffer;

			other._minCapacity = 0;
			other._capacity    = 0;
			other._size        = 0;
			other._buffer      = nullptr;
		}

		bool ShouldGrow() const
		{
			return _size == _capacity;
		}

		bool ShouldShrink() const
		{
			if (_capacity == 0)
				return false;

			return _size * 10 < _capacity * 9;
		}

		void ValidateIndex(const size_t index) const
		{
			if (index >= _size)
				throw std::out_of_range("Index out of range");
		}

		void Deallocate()
		{
			if (_buffer == nullptr)
				return;

			for (int index = 0; index < _size; index++)
			{
				_buffer[index].~TObject();
			}

			::operator delete(_buffer);
		}

	private:
		static TObject* Allocate(const size_t capacity)
		{
			return static_cast<TObject*>(::operator new(sizeof(TObject) * capacity));
		}

		static TObject* Reallocate(TObject* buffer, const size_t currentSize, const size_t newCapacity)
		{
			TObject* newBuffer = Allocate(newCapacity);

			if (newBuffer == nullptr)
				return nullptr;

			for (size_t index = 0; index < currentSize; index++)
			{
				new(newBuffer + index) TObject(buffer[index]);
			}

			return newBuffer;
		}
};
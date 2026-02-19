#pragma once

#include <stdexcept>

template< typename TObject, size_t SIZE >
class StaticArray
{
	private:
		TObject _buffer[SIZE] {};

	public:
		TObject& operator[](const size_t index) noexcept
		{
			return _buffer[index];
		}

		const TObject& operator[](const size_t index) const noexcept
		{
			return _buffer[index];
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

		static size_t Size() noexcept
		{
			return SIZE;
		}

	private:
		static void ValidateIndex(const size_t index)
		{
			if (index >= SIZE)
				throw std::out_of_range("Index out of range");
		}
};
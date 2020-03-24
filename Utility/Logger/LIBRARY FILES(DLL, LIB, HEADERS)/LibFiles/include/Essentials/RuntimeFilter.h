#pragma once
#ifndef RUNTIMEFILTER_H
#define RUNTIMEFILTER_H
#include "ExportConfig.h"
#include "LogFundamentals.h"

namespace log490
{

	template <typename T>
	class InputFilter
	{
	public:
		using filterFunction = void (*)(stream_t& stream, T data);
		void call(stream_t& stream, T data, size_t filterIndex) const;

		static const InputFilter instance;
	private:
		InputFilter();

		static void falseFunction(stream_t& stream, T data);
		static void trueFunction(stream_t& stream, T data);

		filterFunction ptrs[2];
	};

	class RuntimeLogFilter
	{
	public:
		virtual ~RuntimeLogFilter() { }
		template <typename T>
		RuntimeLogFilter& operator<<(const T& data);
		template <typename T>
		RuntimeLogFilter& operator<<(T&& data);
	protected:
		RuntimeLogFilter();

		void setIndex(stream_t* stream);
	private:
		size_t filterIndex;
		stream_t* messageStream;
	};

	template <typename T>
	const InputFilter<T> InputFilter<T>::instance;

	template<typename T>
	inline void InputFilter<T>::call(stream_t& stream, T data, size_t filterIndex) const
	{
		(*ptrs[filterIndex])(stream, data);
	}

	template<typename T>
	inline InputFilter<T>::InputFilter()
	{
		ptrs[0] = &InputFilter<T>::falseFunction;
		ptrs[1] = &InputFilter<T>::trueFunction;
	}

	template<typename T>
	inline void InputFilter<T>::falseFunction(stream_t& stream, T data) {  }

	template<typename T>
	inline void InputFilter<T>::trueFunction(stream_t& stream, T data)
	{
		stream << data;
	}

	template<typename T>
	inline RuntimeLogFilter& RuntimeLogFilter::operator<<(const T& data)
	{
		InputFilter<const T&>::instance.call(*messageStream, data, filterIndex);
		return *this;
	}

	template<typename T>
	inline RuntimeLogFilter& RuntimeLogFilter::operator<<(T&& data)
	{
		InputFilter<T&&>::instance.call(*messageStream, data, filterIndex);
		return *this;
	}

}


#endif

#pragma once

namespace core
{
	class CNoncopyable
	{
	public:
		CNoncopyable(const CNoncopyable&) = delete;
		CNoncopyable& operator=(const CNoncopyable&) = delete;
	protected:
		CNoncopyable() = default;
		//~CNoncopyable() = default;
	};
}
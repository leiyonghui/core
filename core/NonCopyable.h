#pragma once

namespace core
{
	class CNoncopyable
	{
	protected:
		CNoncopyable() = default;
		//virtual ~CNoncopyable() = default;

		CNoncopyable(const CNoncopyable&) = delete;
		CNoncopyable& operator=(const CNoncopyable&) = delete;
	};
}
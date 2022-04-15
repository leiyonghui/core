#pragma once
#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>

using namespace std::literals::chrono_literals;
namespace core
{
	class TimeHelp
	{
	public:
		using system_clock = std::chrono::system_clock;
		using steady_clock = std::chrono::steady_clock;

		static int GetYear(time_t t);

		static int GetMonth(time_t t);

		static int GetDay(time_t t);

		static int GetHour(time_t t);

		static int GetMinute(time_t t);

		static int GetSecond(time_t t);

		static time_t now() {
#if 0
			return time(NULL);//返回保有从纪元开始至今秒数的整数类型值
#else
			return system_clock::to_time_t(system_clock::now());//转换系统时钟时间点为 std::time_t
#endif
		};

		static std::chrono::milliseconds miliseconds()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch());
		}

		static std::chrono::nanoseconds clock() {
			return steady_clock::now() - START_CLOCK;
		};

		static std::chrono::milliseconds ns2ms(const std::chrono::nanoseconds& ns) {
			return std::chrono::duration_cast<std::chrono::milliseconds>(ns);
		}

		static std::chrono::milliseconds clock_ms() {
			return ns2ms(clock());
		}

		static std::string TimeToString(time_t t);

		static void StartUp();

	private:
		static std::chrono::time_point<steady_clock> START_CLOCK;
	};

	class CheckTime
	{
	public:
		using uint64 = unsigned long long;

		void CheckBegin();
		void CheckPoint(std::string key);

	private:
		uint64 _beginTick;

		std::map<std::string, uint64> _aveTime;  // 平均时间
		std::map<std::string, uint64> _maxTicks;
	};
}
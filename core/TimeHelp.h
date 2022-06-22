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

		static int GetYear(const time_t& t);

		static int GetMonth(const time_t& t);

		static int GetDay(const time_t& t);

		static int GetHour(const time_t& t);

		static int GetMinute(const time_t& t);

		static int GetSecond(const time_t& t);

		static time_t now_time() {
#if 0
			return time(NULL);//返回保有从纪元开始至今秒数的整数类型值
#else
			return system_clock::to_time_t(system_clock::now() + OFFEST_MILLI);//转换系统时钟时间点为 std::time_t
#endif
		};

		static system_clock::time_point now()
		{
			return system_clock::now() + OFFEST_MILLI;
		}

		static std::chrono::milliseconds now_milli()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>((system_clock::now() + OFFEST_MILLI).time_since_epoch());
		}

		static std::chrono::milliseconds ns2ms(const std::chrono::nanoseconds& ns) {
			return std::chrono::duration_cast<std::chrono::milliseconds>(ns);
		}

		static std::chrono::nanoseconds ms2ns(const std::chrono::milliseconds& ms) {
			return std::chrono::duration_cast<std::chrono::nanoseconds>(ms);
		}

		static std::chrono::nanoseconds clock_nano() {
			return steady_clock::now() - START_CLOCK + ms2ns(OFFEST_MILLI);
		};

		static std::chrono::milliseconds clock() {
			return ns2ms(clock_nano()) + OFFEST_MILLI;
		}

		static std::string TimeToString(const time_t& t);

		static void StartUp();

		static void SetOffest(const std::chrono::milliseconds& ms);

	private:
		static std::chrono::time_point<steady_clock> START_CLOCK;
		static std::chrono::milliseconds OFFEST_MILLI;
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
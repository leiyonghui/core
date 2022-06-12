#include "TimeHelp.h"
#include <iostream>
#include <sstream>

namespace core
{
	using namespace std;
	using system_clock = std::chrono::system_clock;
	using steady_clock = std::chrono::steady_clock;

	std::chrono::time_point<steady_clock> TimeHelp::START_CLOCK = steady_clock::now();

	void TimeHelp::StartUp()
	{
		START_CLOCK = steady_clock::now();
	}

	int TimeHelp::GetYear(time_t t) {
		return std::localtime(&t)->tm_year;
	}

	int TimeHelp::GetMonth(time_t t) {
		return std::localtime(&t)->tm_mon;
	}

	int TimeHelp::GetDay(time_t t) {
		return std::localtime(&t)->tm_mday;
	}

	int TimeHelp::GetHour(time_t t) {
		return std::localtime(&t)->tm_hour;
	}

	int TimeHelp::GetMinute(time_t t) {
		return std::localtime(&t)->tm_min;
	}

	int TimeHelp::GetSecond(time_t t) {
		return std::localtime(&t)->tm_sec;
	}

	string TimeHelp::TimeToString(time_t t)
	{
		if (t > 0)
		{
			char buf[100];
			//const char* fmt = "%02d-%02d %02d:%02d:%02d";
			const char* fmt = "%02d:%02d:%02d";
			auto ti = *localtime(&t);
#ifdef _WIN32
			int n = sprintf_s(buf, fmt/*, ti.tm_year + 1900*//*, ti.tm_mon + 1, ti.tm_mday*/, ti.tm_hour, ti.tm_min, ti.tm_sec);
#endif
#ifdef __linux
			int n = sprintf(buf, fmt/*, ti.tm_year + 1900*//*, ti.tm_mon + 1, ti.tm_mday*/, ti.tm_hour, ti.tm_min, ti.tm_sec);
#endif
			return string(buf, n);
		}
		else
		{
			return string("0000-00-00 00:00:00");
		}
	};

	void CheckTime::CheckBegin()
	{
		_beginTick = TimeHelp::clock().count();
	}

	void CheckTime::CheckPoint(std::string key)
	{
		if (_maxTicks.find(key) == _maxTicks.end())
		{
			_maxTicks[key] = 0;
			_aveTime[key] = 0;
		}

		auto dis = TimeHelp::clock().count() - _beginTick;
		_aveTime[key] = uint64((uint64(_aveTime[key]) + dis) / 2);

		if (_maxTicks[key] < dis)
		{
			_maxTicks[key] = dis;

			//if (_aveTime[key] > 200)
			{
				std::stringstream os;
				os << "key:" << key.c_str() << " time:" << _maxTicks[key] << " ave time:" << _aveTime[key];
				std::cout << TimeHelp::TimeToString(TimeHelp::now()) << " " << os.str() << endl;
			}

		}
	}
}
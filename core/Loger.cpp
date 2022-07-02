#include "Loger.h"
#include <fstream>

namespace core
{
	static const char* __prefixs[] = {
		"%d %02d:%02d:%02d.%03d DEBUG ",
		"%d %02d:%02d:%02d.%03d ",
		"%d %02d:%02d:%02d.%03d WARNING ",
		"%d %02d:%02d:%02d.%03d ERROR ",
		"%d INFO "
	};

	LogAppender::LogAppender():_level(Debug)
	{
	}
	void LogAppender::setOnmsg(const OnMsg& func)
	{
		_onmsg = func;
	}

	void LogAppender::setLevel(int32 level)
	{
		_level = level;
	}

	int32 LogAppender::getLevel() const
	{
		return _level;
	}

	void LogAppenderImpl::start()
	{
		if (_runing)
			return;
		_runing = true;
		_th = std::thread([this]() {
			loop();
		});
	}

	void LogAppenderImpl::stop()
	{
		if (_runing)
		{
			_runing = false;
			_queue.stop();
			_th.join();
		}		
	}

	void LogAppenderImpl::loop()
	{
		while (_runing)
		{
			std::list<LogMsg> list;
			_queue.take(list, 10);
			for (auto& msg : list)
			{
				appender(msg);
				if (_onmsg)
					_onmsg(msg);
			}
		}
		std::list<LogMsg> list;
		_queue.flush(list);

		if (!list.empty())
		{
			for (auto& msg : list)
			{
				appender(msg);
				if (_onmsg)
					_onmsg(msg);
			}
		}
	}

	void  ConsoleLogAppender::appender(const LogMsg& msg)
	{
		std::string str;
		str.reserve(msg.log.size() + 1);
		str.append(msg.log);
		str.push_back('\n');
		fwrite(msg.log.c_str(), 1, msg.log.length(), stdout);
		//std::cout << msg.log << std::endl;
	}

	FileLogAppender::FileLogAppender(const std::string& filename, bool console) :ConsoleLogAppender(), _filename(filename), _timed(false), _console(console), _pid(core::GetPid()), _outs(2)
	{
		size_t pos = _filename.find("%T");
		if (pos != string::npos)
		{
			_filename.replace(pos, 2, "%04d-%02d-%02d");
			_timed = true;
		}
	}

	FileLogAppender::~FileLogAppender()
	{
		for(auto iter : _outs)
		{
			if (iter.second)
			{
				iter.second->flush();
				iter.second->close();
				delete iter.second;
			}
		}
	}

	void FileLogAppender::appender(const LogMsg& msg)
	{
		TimeInfo ti = TimeHelp::GetTimeInfo(msg.time / 1000);
		char buf[64];
		int n = sprintf(buf, __prefixs[msg.level], _pid, ti.hour, ti.min, ti.sec, msg.time % 1000);
		string str;
		str.reserve(n + msg.log.size() + 1);
		str.append(buf, buf + n);
		str.append(msg.log);
		str.push_back('\n');
		if (_timed)
		{
			int32 outIdx = int32(ti.yday % _outs.size());
			std::pair<int32, std::ofstream*>& out = _outs[outIdx];
			if (out.first != ti.yday && out.second != NULL)
			{
				out.second->flush();
				out.second->close();
				delete out.second;
				out.first = 0;
				out.second = NULL;
			}
			if (out.second == NULL)
			{
				char name[256];
				sprintf(name, _filename.c_str(), ti.year, ti.mon, ti.day);
				out.first = ti.yday;
				out.second = new std::ofstream(name, std::ofstream::out | std::ofstream::app | std::ofstream::binary);
			}
			(*out.second) << str;
			out.second->flush();
			if (_console)
			{
				fwrite(str.c_str(), 1, str.length(), stdout);
			}
		}
		else
		{
			std::pair<int32, std::ofstream*>& out = _outs[0];
			if (out.second == NULL)
			{
				out.second = new std::ofstream(_filename.c_str(), std::ofstream::out | std::ofstream::app | std::ofstream::binary);
			}
			(*out.second) << str;
			out.second->flush();
			if (_console)
			{
				fwrite(str.c_str(), 1, str.length(), stdout);
			}
		}
	}

	void FileLogAppender::stop()
	{
		ConsoleLogAppender::stop();

		for (auto& iter : _outs)
		{
			if (iter.second)
			{
				iter.second->flush();
				iter.second->close();
				delete iter.second;
				iter.second = nullptr;
			}
		}
	}

	void Logger::trace(const std::ostringstream& log)
	{
		LogMsg msg;
		msg.level = ELogLevel::Trace;
		msg.time = TimeHelp::now_milli().count();
		msg.log = log.str();
		if (_logAppender->getLevel() <= msg.level)
			_logAppender->push(std::move(msg));
	}

	void Logger::debug(const std::ostringstream& log)
	{
		LogMsg msg;
		msg.level = ELogLevel::Debug;
		msg.time = TimeHelp::now_milli().count();
		msg.log = log.str();
		if (_logAppender->getLevel() <= msg.level)
			_logAppender->push(std::move(msg));
	}

	void Logger::warning(const std::ostringstream& log)
	{
		LogMsg msg;
		msg.level = ELogLevel::Warning;
		msg.time = TimeHelp::now_milli().count();
		msg.log = log.str();
		if (_logAppender->getLevel() <= msg.level)
			_logAppender->push(std::move(msg));
	}

	void Logger::error(const std::ostringstream& log)
	{
		LogMsg msg;
		msg.level = ELogLevel::Error;
		msg.time = TimeHelp::now_milli().count();
		msg.log = log.str();
		if (_logAppender->getLevel() <= msg.level)
			_logAppender->push(std::move(msg));
	}

	void Logger::info(const std::ostringstream& log)
	{
		LogMsg msg;
		msg.level = ELogLevel::Info;
		msg.time = TimeHelp::now_milli().count();
		msg.log = log.str();
		_logAppender->push(std::move(msg));
	}

	const char* get_short_file(const char* file, size_t size)
	{
		for (auto i = int32(size) - 2; i >= 0; --i)
		{
			switch (file[i]) { case '/': case '\\': return file + i + 1; };
		}
		return file;
	}

	
}
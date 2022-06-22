#include "Loger.h"

namespace core
{
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
		//std::string str;
		//str.reserve(msg.log.size() + 1);
		//str.append(msg.log);
		//str.push_back('\n');
		//fwrite(msg.log.c_str(), 1, msg.log.length(), stdout);
		std::cout << msg.log << std::endl;
	}

	void FileLogAppender::appender(const LogMsg& msg)
	{
		if (_console)
			ConsoleLogAppender::appender(msg);
		
	}

	void Logger::trace(const std::ostringstream& log)
	{
		LogMsg msg;
		msg.level = ELogLevel::Trace;
		msg.log = log.str();
		if (_logAppender->getLevel() <= msg.level)
			_logAppender->push(std::move(msg));
	}

	void Logger::debug(const std::ostringstream& log)
	{
		LogMsg msg;
		msg.level = ELogLevel::Debug;
		msg.log = log.str();
		if (_logAppender->getLevel() <= msg.level)
			_logAppender->push(std::move(msg));
	}

	void Logger::warning(const std::ostringstream& log)
	{
		LogMsg msg;
		msg.level = ELogLevel::Warning;
		msg.log = log.str();
		if (_logAppender->getLevel() <= msg.level)
			_logAppender->push(std::move(msg));
	}

	void Logger::error(const std::ostringstream& log)
	{
		LogMsg msg;
		msg.level = ELogLevel::Error;
		msg.log = log.str();
		if (_logAppender->getLevel() <= msg.level)
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
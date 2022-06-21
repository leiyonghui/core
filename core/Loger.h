#pragma once
#include "Singleton.h"
#include "NonCopyable.h"
#include "SyncQueue.h"
#include "Types.h"
#include <sstream>

namespace core
{
	struct LogMsg {
		int32 level;
		int64 time;
		std::string log;
	};

	enum ELogLevel {
		Debug = 0,
		Trace,
		Warning,
		Error,
	};

    class LogAppender
    {
    public:
		LogAppender();
		virtual ~LogAppender() = default;

        using OnMsg = std::function<void(const LogMsg& msg)>;

        void setOnmsg(const OnMsg& func);

        void setLevel(int32 level);

        int32 getLevel() const;

        virtual void push(LogMsg&& msg) = 0;

        virtual void start() = 0;

        virtual void stop() = 0;

        virtual void appender(const LogMsg& msg) = 0;

    protected:
        OnMsg _onmsg;
        int32 _level;
    };


    class LogAppenderImpl : public LogAppender
    {
    public:
        LogAppenderImpl() :LogAppender(),_runing(false)
        {

        }

        virtual void push(LogMsg&& msg) override
        {
            _queue.put(std::move(msg));
        }

        virtual void start() override;

        virtual void stop() override;

        void loop();

    protected:
		volatile bool _runing;
		SyncQueue<LogMsg> _queue;
        std::thread _th;
    };

    class ConsoleLogAppender : public LogAppenderImpl
    {
    public:
        void appender(const LogMsg& msg) override;
    };

    class FileLogAppender : public ConsoleLogAppender
    {
    public:
        FileLogAppender(const std::string& fileName, bool console):ConsoleLogAppender(), _fileName(fileName), _timed(false), _console(console)
        {
            
        }

        void appender(const LogMsg& msg) override;

    protected:
		std::string _fileName;
		bool _timed;
		bool _console;
    };

    class Logger : public CSingleton<Logger>, public CNoncopyable
    {
    public:

        Logger(LogAppender* logAppender): _logAppender(logAppender)
        {
            _logAppender->start();
        }

        virtual~Logger() = default;

        void trace(const std::ostringstream& log);

        void debug(const std::ostringstream& log);

        void warning(const std::ostringstream& log);

        void error(const std::ostringstream& log);

    protected:
        LogAppender* _logAppender;
    };
   
	template<class ...Args>
	inline std::ostringstream printf_log(const std::string& time, const char* file, int line, const Args& ...args)
	{
        std::ostringstream oss;
        oss << time << " " << file << ":" << line;
		((oss << " " << args), ...);
        return oss;
	}

    const char* get_short_file(const char* file, size_t size);

#define core_log_trace(...) core::Logger::Instance()->trace(printf_log(TimeHelp::TimeToString(time(NULL)), get_short_file(__FILE__, sizeof(__FILE__)), __LINE__, ## __VA_ARGS__))
#define core_log_error(...) core::Logger::Instance()->error(printf_log(TimeHelp::TimeToString(time(NULL)), get_short_file(__FILE__, sizeof(__FILE__)), __LINE__, "ERROR", ## __VA_ARGS__))
#define core_log_warning(...) core::Logger::Instance()->warning(printf_log(TimeHelp::TimeToString(time(NULL)), get_short_file(__FILE__, sizeof(__FILE__)), __LINE__, "WARNING", ## __VA_ARGS__))
#define core_log_debug(...) core::Logger::Instance()->debug(printf_log(TimeHelp::TimeToString(time(NULL)), get_short_file(__FILE__, sizeof(__FILE__)), __LINE__, "DEBUG", ## __VA_ARGS__))
};

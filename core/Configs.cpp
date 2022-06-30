#include "Configs.h"

#ifdef _WIN32
#include <process.h>

#endif
#ifdef __linux
#include <unistd.h>
#endif // __Liunx

namespace core
{
	using StacktraceFunc = std::function<std::string(int32 idx, int32 num, const std::string& prefix)>;
	static StacktraceFunc __stacktrace;

	std::string GetStacktrace(int32 idx, int32 num, const std::string& prefix)
	{
		return __stacktrace(idx, num, prefix);
	}

	void SetStacktrace(const std::function<std::string(int32 idx, int32 num, const std::string& prefix)>& func)
	{
		__stacktrace = func;
	}

	int GetPid()
	{
#ifdef _WIN32
		return _getpid();
#endif // _WIN32
#ifdef __linux
		return getpid();
#endif // __linux


	}
}
#pragma once
#include <functional>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <string.h>
#include <list>
#include <array>
#include <memory>
#include <assert.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <any>
#include "Types.h"
#include "TimeHelp.h"
#include "Singleton.h"
#include "Macro.h"
#include "Loger.h"

namespace core
{
	using std::placeholders::_1;
	using std::placeholders::_2;
	using std::placeholders::_3;
	using std::placeholders::_4;
	using std::placeholders::_5;

	std::string GetStacktrace(int32 idx, int32 num, const std::string& prefix);

	void SetStacktrace(const std::function<std::string(int32 idx, int32 num, const std::string& prefix)>& func);

	int GetPid();
}



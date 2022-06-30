#pragma once
#include "Configs.h"

namespace core
{
	class ResourceMonitor : public CSingleton<ResourceMonitor>
	{
	public:
		using Container = std::map<std::string, int32>;

		void addCounter(const std::string& name, int32 add)
		{
			std::lock_guard<std::mutex> locker(_mutex);
			_container[name] += add;
		}

		void delCounter(const std::string& name, int32 add)
		{
			std::lock_guard<std::mutex> locker(_mutex);
			_container[name] -= add;
		}

		void showinfo()
		{
			core_log_trace("================ResourceMonitor================");
			for (auto& [name, count] : _container)
				core_log_info(name, "count:", count);
			core_log_trace("===============================================");
		}

	protected:
		Container _container;
		std::mutex _mutex;
	};
}
#pragma once
#include "Configs.h"
#include "core/NonCopyable.h"

namespace core
{
	class MsgQueue : CNoncopyable
	{
	public:

		void poll(bool& busy);
	};
}
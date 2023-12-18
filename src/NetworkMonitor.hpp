#ifndef _NetworkMonitor_hpp_
#define _NetworkMonitor_hpp_

#include <Core/Core.h>

using namespace Upp;

struct NetworkMonitor {
public:
	NetworkMonitor(){};
	~NetworkMonitor() { Stop(); };

	bool Start();
	bool Stop();

	void ClearEvents()
	{
		NetworkDetected.Clear();
		WhenBitRate.Clear();
	};

	Event<Index<String>&, Index<String>&> NetworkDetected;
	Event<int64, int64> WhenBitRate;

private:
	void Read();

	One<Thread> thread_{};
	ConditionVariable cv_{};
	Mutex mThread_{};
};

#endif

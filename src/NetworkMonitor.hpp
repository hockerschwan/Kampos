#ifndef _NetworkMonitor_hpp_
#define _NetworkMonitor_hpp_

#include <Core/Core.h>

using namespace Upp;

struct NetworkMonitor {
public:
	NetworkMonitor() { Start(); };
	~NetworkMonitor() { Stop(); };

	bool Start();
	bool Stop();

private:
	void Read();

	One<Thread> thread_{};
	ConditionVariable cv_{};
	Mutex mThread_{};
};

#endif

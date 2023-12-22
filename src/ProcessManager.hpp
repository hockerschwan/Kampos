#ifndef _ProcessManager_hpp_
#define _ProcessManager_hpp_

#include <Core/Core.h>

using namespace Upp;

struct ProcessManager {
public:
	ProcessManager(){};
	~ProcessManager() { Stop(); };

	bool ClientInstalled();
	const Id GetCurrentId() const { return pick(clone(uuid_)); };

	bool Start(const Id& uuid, bool stop = true);
	bool Stop();

	void ClearEvents()
	{
		WhenStarted.Clear();
		WhenStopped.Clear();
	};

	Event<Id> WhenStarted;
	Event<> WhenStopped;

private:
	void Read();
	void Log(const String& str);

	void SetUUID(const Id& uuid)
	{
		Mutex::Lock lock(mId_);
		uuid_ = uuid;
	};

	void Started() { WhenStarted(uuid_); };
	void Stopped() { WhenStopped(); };

	One<LocalProcess> process_{};
	One<Thread> thread_{};
	ConditionVariable cv_{};
	Mutex mThread_{};

	Id uuid_{};
	Mutex mId_{};
};

#endif

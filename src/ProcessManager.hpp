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
	void Read(const Id& uuid);
	void Log(const String& str);

	void SetUUID(const Id& uuid)
	{
		Mutex::Lock lock(mId_);
		uuid_ = uuid;
	};

	One<LocalProcess> process_{};
	One<Thread> thread_{};
	ConditionVariable cv_{};
	Mutex mThread_{};

	Id uuid_{};
	Mutex mId_{};
};

#endif

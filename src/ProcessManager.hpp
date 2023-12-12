#ifndef _ProcessManager_hpp_
#define _ProcessManager_hpp_

#include <Core/Core.h>
#include <iphlpapi.h>

using namespace Upp;

struct ProcessManager {
public:
	ProcessManager(){};
	~ProcessManager() { Stop(); };

	bool ClientInstalled();
	const Id GetCurrentId() const { return pick(clone(uuid_)); };

	bool Start(const Id& uuid);
	bool Stop();

	Event<Id> WhenStarted;
	Event<> WhenStopped;
	Event<int64, int64> WhenBitRate;

private:
	void Read();

	void SetUUID(const Id& uuid)
	{
		Mutex::Lock lock(mId_);
		uuid_ = uuid;
	};

	bool GetAdapterInfo(IN const char* name, OUT MIB_IFROW& info);

	void Started() { WhenStarted(uuid_); };
	void Stopped() { WhenStopped(); };
	void BitRate() { WhenBitRate(bitsRecv_, bitsSent_); };

	One<LocalProcess> process_{};
	One<Thread> thread_{};
	ConditionVariable cv_{};
	Mutex mThread_{};

	Id uuid_{};
	Mutex mId_{};

	int64 bitsRecv_ = 0;
	int64 bitsSent_ = 0;
};

#endif

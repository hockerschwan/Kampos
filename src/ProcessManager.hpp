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

	bool Start(const Id& uuid);
	bool Stop();

private:
	void Read();

	One<LocalProcess> process_{};
	Thread thread_{};
	Id uuid_{};
};

#endif

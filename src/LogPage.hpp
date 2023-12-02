#ifndef _LogPage_hpp_
#define _LogPage_hpp_

#include "Logger.hpp"
#include "common.hpp"

struct LogPage : WithLogPageLayout<ParentCtrl> {
	typedef LogPage CLASSNAME;

public:
	LogPage();
	~LogPage() { Shutdown(); };

private:
	void Read();
	void Append(const LogEntry& entry);

	Thread thread_{};
	Mutex mutex_{};
};

#endif

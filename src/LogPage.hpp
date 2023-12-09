#ifndef _LogPage_hpp_
#define _LogPage_hpp_

#include "Logger.hpp"
#include "common.hpp"

struct LogPage : WithLogPageLayout<ParentCtrl> {
public:
	LogPage();
	~LogPage() { thread_.Wait(); };

	void ScrollBottom() { arrayCtrl_.ScrollEnd(); };

private:
	void Read();
	void Append(const LogEntry& entry);

	Thread thread_{};
	Mutex mutex_{};
};

#endif

#ifndef _LogPage_hpp_
#define _LogPage_hpp_

#include "Logger.hpp"
#include "common.hpp"

struct LogPage : WithLogPageLayout<ParentCtrl> {
public:
	LogPage();

	void ScrollBottom() { arrayCtrl_.ScrollEnd(); };

private:
	void Read();
	void Append(const LogEntry& entry);
};

#endif

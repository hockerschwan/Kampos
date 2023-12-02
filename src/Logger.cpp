#include "Helper.hpp"
#include "Logger.hpp"

Logger::Logger()
{
	RollLogFiles();

	auto sec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
	auto time = TimeFromUTC(sec.count());
	auto logName = GetHomeDirectory() << GetAppName() << "\\logs\\" << FormatTime(time, "YYYYMMDD-hhmmss") << ".log";
	StdLogSetup(LOG_FILE, logName);

	queue_.Reserve(16);
}

void Logger::Log(const String& value)
{
	auto t = Helper::GetTime();
	auto entry = LogEntry(t, value);

	LOG(entry);

	{
		Mutex::Lock _(mutex_);
		queue_.AddTail(entry);
	}
	if(!Thread::IsShutdownThreads()) {
		cv_.Signal();
	}
}

bool Logger::Read(LogEntry& out)
{
	Mutex::Lock _(mutex_);
	if(queue_.GetCount() == 0) {
		out = LogEntry::GetVoid();
		return false;
	}

	out = queue_.Head();
	queue_.DropHead();
	return queue_.GetCount() > 0;
}

int Logger::CountFiles(const String& path)
{
	FindFile find{};
	find.Search(path);

	auto count = 0;
	while(true) {
		auto file = find.GetName();
		++count;
		if(!find.Next()) {
			break;
		}
	}
	return count;
}

void Logger::RollLogFiles(byte num)
{
	auto logDir = GetHomeDirectory() << GetAppName() << "\\logs\\";
	RealizeDirectory(logDir);

	auto path = logDir + "*.log";
	while(CountFiles(path) >= num) {
		FindFile find(path);
		if(!find.GetName().IsEmpty()) {
			if(!FileDelete(find.GetPath())) {
				++num;
			}
		}
	}
}

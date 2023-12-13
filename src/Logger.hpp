#ifndef _Logger_hpp_
#define _Logger_hpp_

#include <Core/Core.h>

using namespace Upp;

struct LogEntry {
	String Timestamp{};
	String Message{};

	LogEntry(){};
	LogEntry(const String& timestamp, const String& message)
	{
		Timestamp = timestamp;
		Message = message;
	};

	const String ToString() const { return String() << Timestamp << " " << Message; };

	static LogEntry GetVoid() { return LogEntry{}; }

	bool operator==(const LogEntry& rhs) const { return Timestamp == rhs.Timestamp && Message == rhs.Message; };
	bool operator!=(const LogEntry& rhs) const { return !(*this == rhs); };
};

struct Logger {
public:
	Logger();
	~Logger() { Log("Bye."); };

	void Log(const String& value);
	void Log(const int& value) { Log(IntStr(value)); };
	void Log(const int64& value) { Log(IntStr64(value)); };
	void Log(const double& value) { Log(DblStr(value)); };

	const Array<LogEntry> Get();

	void ClearEvents() { WhenLog.Clear(); };

	Event<> WhenLog;

private:
	int CountFiles(const String& path);
	void RollLogFiles(byte num = 10);

	Array<LogEntry> queue_{};
	Mutex mutex_{};
};

#endif

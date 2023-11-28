#include "MainWindow.hpp"
#include <Core/Core.h>
#include <shlobj.h>

#define IMAGECLASS AppIcons
#define IMAGEFILE <Kampos/src/icons.iml>
#include <Draw/iml_source.h>

int IsAlreadyRunning()
{
	auto exe = GetAppName() += ".exe";
	auto cmd = String("tasklist /fi \"imagename eq ");
	cmd << exe << "\" /fo csv";

	auto str = Sys(cmd);
	if(str.IsEmpty()) {
		return -1;
	}

	auto pid = FormatUnsigned(GetCurrentProcessId());
	auto rows = Split(str, "\r\n");
	for(auto& row : rows) {
		row.Replace("\"", "");
		auto words = Split(row, ",");

		if(words[0] != exe) {
			continue;
		}

		if(words[1] != pid) {
			return 1;
		}
	}
	return 0;
}

int CountFiles(const String& path)
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

void RollLogFiles(byte num = 10)
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

INITBLOCK
{
	SetAppName("Kampos");

	auto n = IsAlreadyRunning();
	if(n != 0) {
		Exit(n);
	}

	{
		WCHAR* wcRoaming;
		if(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &wcRoaming) != S_OK) {
			Exit(-2);
		}

		auto roaming = String(wcRoaming);
		if(!roaming.EndsWith("\\")) {
			roaming += "\\";
		}
		SetHomeDirectory(roaming);
		UseHomeDirectoryConfig(true);
	}

	{
		RollLogFiles();

		auto sec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
		auto time = TimeFromUTC(sec.count());
		SetDateFormat("%1:04d-%2:02d-%3:02d");
		auto logName = GetHomeDirectory() << GetAppName() << "\\logs\\" << FormatTime(time, "YYYYMMDD-hhmmss") << ".log";
		StdLogSetup(LOG_FILE | LOG_TIMESTAMP, logName);
	}
}

GUI_APP_MAIN
{
	auto w = new MainWindow();
	w->OpenMain();

	Ctrl::EventLoop();
}

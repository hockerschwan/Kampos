#include "ConfigManager.hpp"
#include "LogPage.hpp"

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<ConfigManager> gConfigManager;

LogPage::LogPage()
{
	CtrlLayout(*this);

	{
		arrayCtrl_.AddColumn("Timestamp");
		arrayCtrl_.AddColumn("Message");

		auto w = Helper::CalcSize("00:00:00.000", arrayCtrl_.HeaderTab(0).GetFont(), 10);
		arrayCtrl_.HeaderTab(0).Fixed(w + 16).AlignCenter();

		arrayCtrl_.WhenBar = [&](Bar& bar) {
			bar.Add("Copy", Rescale(AppIcons::Copy(), Zx(15), Zx(15)), [&] {
				auto t = arrayCtrl_.Get(arrayCtrl_.GetCursor(), 0);
				auto m = arrayCtrl_.Get(arrayCtrl_.GetCursor(), 1);
				AppendClipboardText(String(t) << " " << m);
			});
		};
	}

	thread_.RunNice([&] { Read(); });
}

void LogPage::Read()
{
	while(!IsShutdown() && !IsShutdownThreads()) {
		gLogger->GetConditionVariable().Wait(mutex_, 500);
		while(true) {
			LogEntry entry{};
			auto more = gLogger->Read(entry);
			if(entry != LogEntry::GetVoid()) {
				Append(entry);
			}
			if(!more) {
				break;
			}
		}
	}
}

void LogPage::Append(const LogEntry& entry)
{
	if(IsShutdown() || IsShutdownThreads()) {
		return;
	}

	auto limit = ScanInt(gConfigManager->Load("LogDisplayLimit"));

	GuiLock __;
	arrayCtrl_.Add(entry.Timestamp, entry.Message);
	while(arrayCtrl_.GetCount() > limit) {
		arrayCtrl_.Remove(0);
	}
	arrayCtrl_.ScrollEnd();
}

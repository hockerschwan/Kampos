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
		arrayCtrl_.HeaderTab(0).Fixed(w + 16);

		arrayCtrl_.WhenBar = [&](Bar& bar) {
			bar.Add("Copy", Upp::CtrlImg::copy(), [&] {
				auto t = arrayCtrl_.Get(arrayCtrl_.GetCursor(), 0);
				auto m = arrayCtrl_.Get(arrayCtrl_.GetCursor(), 1);
				AppendClipboardText(String(t) << " " << m);
			});
		};
	}

	mutex_ = &gLogger->GetMutex();
	cv_ = &gLogger->GetConditionVariable();
	thread_.RunNice(THISFN(Read));
}

void LogPage::Read()
{
	while(true) {
		cv_->Wait(*mutex_);
		while(true) {
			LogEntry entry{};
			if(!gLogger->Read(entry)) {
				Append(entry);
				break;
			}
			Append(entry);
		}
	}
}

void LogPage::Append(const LogEntry& entry)
{
	auto limit = ScanInt(gConfigManager->Load("LogDisplayLimit"));

	EnterGuiMutex();

	arrayCtrl_.Add(entry.Timestamp, entry.Message);
	while(arrayCtrl_.GetCount() > limit) {
		arrayCtrl_.Remove(0);
	}
	arrayCtrl_.ScrollEnd();

	LeaveGuiMutex();
}

#include "Helper.hpp"
#include "Logger.hpp"
#include "MainWindow.hpp"

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<MainWindow> gMainWindow;

MainWindow::MainWindow()
	: tunnelsPage_(MakeOne<TunnelsPage>())
	, rulesPage_(MakeOne<RulesPage>())
	, logPage_(MakeOne<LogPage>())
	, settingsPage_(MakeOne<SettingsPage>())
	, tray_(MakeOne<TrayIcon>())
{
	CtrlLayout(*this);
	Zoomable().Sizeable().Icon(AppIcons::Icon16, AppIcons::Icon24);
	SetMinSize(Zsz(720, 480));
	WhenClose = [&] { Hide(); };

	navigation_.ItemWidth(Zx(61)).ItemHeight(Zy(56)); // 65,64 @100%
	navigation_.RowMode().NoRoundSize().AutoHideSb(true).SetDisplay(CenteredImageDisplay());
	navigation_.Add(Rescale(AppIcons::Tunnels(), Zx(30), Zx(30)), true); // 32 @100%
	navigation_.Add(Rescale(AppIcons::Rules(), Zx(30), Zx(30)), true);
	navigation_.Add(Rescale(AppIcons::Log(), Zx(30), Zx(30)), true);
	navigation_.Add(Rescale(AppIcons::Settings(), Zx(30), Zx(30)), true);
	navigation_.WhenSel = [&] { SetContent(); };

	tray_->WhenBar = [&](Bar& bar) { ShowTrayMenu(bar); };
	tray_->WhenLeftDown = [&] { ShowWindow(); };
	tray_->Icon(AppIcons::Icon16);
}

void MainWindow::SetContent()
{
	while(content_.GetChildCount() > 0) {
		content_.RemoveChild(content_.GetIndexChild(0));
	}

	switch(navigation_.GetCursor()) {
	case 0:
		content_.Add(tunnelsPage_->SizePos());
		break;
	case 1:
		content_.Add(rulesPage_->SizePos());
		break;
	case 2:
		content_.Add(logPage_->SizePos());
		break;
	case 3:
		content_.Add(settingsPage_->SizePos());
		break;
	}
}

void MainWindow::ShowExitPrompt()
{
	exitPromptShown_ = true;
	if(PromptYesNo("Are you sure you want to exit?") == 1) {
		gLogger->Log("Shutting down...");
		Hide();
		ShutdownThreads();

		tray_.Clear();
		tunnelsPage_.Clear();
		rulesPage_.Clear();
		logPage_.Clear();
		settingsPage_.Clear();

		Thread t{};
		t.Run([&] { t.Sleep(100); });
		t.Wait();

		gLogger->Log("Destroying MainWindow...");
		auto del = gMainWindow.get_deleter();
		del(gMainWindow.release());
		return;
	}
	exitPromptShown_ = false;
}

void MainWindow::ShowTrayMenu(Bar& bar)
{
	bar.Add("Exit", CtrlImg::remove, [&] { ShowExitPrompt(); });
}

void MainWindow::ShowWindow()
{
	if(exitPromptShown_) {
		return;
	}

	if(IsShown()) {
		if(IsMinimized()) {
			Overlap();
		}
		else {
			TopMost(true).TopMost(false);
		}
	}
	else {
		Show();
	}
}
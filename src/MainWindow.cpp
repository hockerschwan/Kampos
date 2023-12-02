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
	WhenClose = THISBACK(Hide);

	navigation_.RowMode().NoRoundSize().AutoHideSb(true).ItemHeight(64).ItemWidth(65).SetDisplay(CenteredImageDisplay());
	navigation_.Add(AppIcons::Tunnels(), true);
	navigation_.Add(AppIcons::Rules(), true);
	navigation_.Add(AppIcons::Log(), true);
	navigation_.Add(AppIcons::Settings(), true);
	navigation_.WhenSel = THISBACK(SetContent);

	tray_->WhenBar = THISBACK(ShowTrayMenu);
	tray_->WhenLeftDown = THISBACK(ShowWindow);
	tray_->Icon(AppIcons::Icon16);
}

void MainWindow::ClearContent()
{
	while(content_.GetChildCount() > 0) {
		content_.RemoveChild(content_.GetIndexChild(0));
	}
}

void MainWindow::SetContent()
{
	ClearContent();

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

		Sleep(100);

		gLogger->Log("Destroying MainWindow...");
		auto del = gMainWindow.get_deleter();
		del(gMainWindow.release());
		return;
	}
	exitPromptShown_ = false;
}

void MainWindow::ShowTrayMenu(Bar& bar) { bar.Add("Exit", CtrlImg::remove, THISBACK(ShowExitPrompt)); }

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
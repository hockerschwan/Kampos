#include "Helper.hpp"
#include "MainWindow.hpp"

MainWindow::MainWindow()
{
	CtrlLayout(*this);
	Zoomable().Sizeable().Icon(AppIcons::IconX16, AppIcons::IconX24);
	WhenClose = THISBACK(Hide);

	navigation_.RowMode().NoRoundSize().AutoHideSb(true).ItemHeight(64).ItemWidth(65).SetDisplay(CenteredImageDisplay());
	navigation_.Add(AppIcons::Tunnels(), true);
	navigation_.Add(AppIcons::Rules(), true);
	navigation_.Add(AppIcons::Log(), true);
	navigation_.Add(AppIcons::Settings(), true);
	navigation_.WhenSel = THISBACK(SetContent);

	tray_.WhenBar = THISBACK(ShowTrayMenu);
	tray_.WhenLeftDown = THISBACK(ShowWindow);
	tray_.Icon(AppIcons::IconX16);

	LOG(Helper::RoamingPath());
	LOG(Helper::AppPath());
	LOG(Helper::LogsPath());
	LOG(Helper::TunnelsPath());
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
		content_.Add(tunnelsPage_.SizePos());
		break;
	case 1:
		content_.Add(rulesPage_.SizePos());
		break;
	case 2:
		content_.Add(logPage_.SizePos());
		break;
	case 3:
		content_.Add(settingsPage_.SizePos());
		break;
	}
}

void MainWindow::ShowExitPrompt()
{
	exitPromptShown_ = true;
	if(PromptYesNo("Are you sure you want to exit?") == 1) {
		delete this;
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
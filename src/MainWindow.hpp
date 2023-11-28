#ifndef _MainWindow_hpp_
#define _MainWindow_hpp_

#include "LogPage.hpp"
#include "RulesPage.hpp"
#include "SettingsPage.hpp"
#include "TunnelsPage.hpp"

struct MainWindow : public WithMainWindowLayout<TopWindow> {
	typedef MainWindow CLASSNAME;

public:
	MainWindow();

private:
	void ClearContent();
	void SetContent();
	void ShowExitPrompt();
	void ShowTrayMenu(Bar& bar);
	void ShowWindow();

	TunnelsPage tunnelsPage_;
	RulesPage rulesPage_;
	LogPage logPage_;
	SettingsPage settingsPage_;
	TrayIcon tray_;

	bool exitPromptShown_ = false;
};

#endif
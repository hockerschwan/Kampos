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

	void Exit() { ShowExitPrompt(); };

private:
	void ClearContent();
	void SetContent();
	void ShowExitPrompt();
	void ShowTrayMenu(Bar& bar);
	void ShowWindow();

	One<TunnelsPage> tunnelsPage_;
	One<RulesPage> rulesPage_;
	One<LogPage> logPage_;
	One<SettingsPage> settingsPage_;
	One<TrayIcon> tray_;

	bool exitPromptShown_ = false;
};

#endif

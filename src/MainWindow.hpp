#ifndef _MainWindow_hpp_
#define _MainWindow_hpp_

#include "LogPage.hpp"
#include "RulesPage.hpp"
#include "SettingsPage.hpp"
#include "TunnelsPage.hpp"

struct MainWindow : public WithMainWindowLayout<TopWindow> {
public:
	MainWindow();

private:
	void SetContent();
	void SetTitle();
	void ShowExitPrompt();
	void ShowTrayMenu(Bar& bar);
	void ShowTunnelsSubMenu(Bar& bar);
	void ShowWindow();

	One<TunnelsPage> tunnelsPage_;
	One<RulesPage> rulesPage_;
	One<LogPage> logPage_;
	One<SettingsPage> settingsPage_;
	One<TrayIcon> tray_;

	bool exitPromptShown_ = false;

	int64 bitsRecv_ = -1;
	int64 bitsSent_ = -1;
};

#endif

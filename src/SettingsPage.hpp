#ifndef _SettingsPage_hpp_
#define _SettingsPage_hpp_

#include "common.hpp"

struct SettingsPanel : WithSettingsLayout<ParentCtrl> {
public:
	SettingsPanel();

	Event<> WhenExit;

private:
	void ExitPressed()
	{
		Event<> h = WhenExit;
		h();
	};

	Button::Style exitStyle_;
};

struct SettingsPage : WithSettingsPageLayout<ParentCtrl> {
public:
	SettingsPage();

	void Layout() override { panel_.HSizePosZ(); };

	Event<> WhenExit;

private:
	void ExitPressed()
	{
		Event<> h = WhenExit;
		h();
	};

	SettingsPanel panel_;
};

#endif

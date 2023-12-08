#ifndef _SettingsPage_hpp_
#define _SettingsPage_hpp_

#include "common.hpp"

struct SettingsPanel : WithSettingsLayout<ParentCtrl> {
public:
	SettingsPanel();

private:
	Button::Style exitStyle_;
};

struct SettingsPage : WithSettingsPageLayout<ParentCtrl> {
public:
	SettingsPage();

	void Layout() override { panel_.HSizePosZ(); };

private:
	SettingsPanel panel_;
};

#endif

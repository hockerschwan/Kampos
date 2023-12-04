#ifndef _SettingsPage_hpp_
#define _SettingsPage_hpp_

#include "common.hpp"

struct SettingsPage : WithSettingsPageLayout<ParentCtrl> {
public:
	SettingsPage();

private:
	Button::Style exitStyle_;
};

#endif

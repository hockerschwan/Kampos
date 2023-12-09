#ifndef _NewNameDialog_hpp_
#define _NewNameDialog_hpp_

#include "TunnelsManager.hpp"
#include "common.hpp"

extern std::unique_ptr<TunnelsManager> gTunnelsManager;

struct NewNameDialog : public WithNewNameDialogLayout<TopWindow> {
public:
	NewNameDialog()
	{
		Sizeable().Icon(Rescale(AppIcons::Pen(), Zx(15), Zx(15)));
		CtrlLayoutOKCancel(*this, "Enter name (without extension)");

		ok.Disable();

		text.WhenAction = [&] {
			auto str = TrimBoth(text.GetText().ToString());

			if(str.IsEmpty()) {
				ok.Disable();
				return;
			}

			for(const auto& item : ~(Helper::ForbiddenChars())) {
				if(str.Find(item.key) >= 0) {
					ok.Disable();
					return;
				}
			}

			FindFile find{};
			find.Search(Helper::TunnelsPath() + "*.conf");
			while(true) {
				auto name = find.GetName();
				name.TrimEnd(".conf");
				if(ToLower(str) == ToLower(name)) {
					ok.Disable();
					return;
				}

				if(!find.Next()) {
					break;
				}
			}

			ok.Enable();
		};
	};
};

#endif

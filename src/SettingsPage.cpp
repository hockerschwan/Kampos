#include "ConfigManager.hpp"
#include "MainWindow.hpp"
#include "SettingsPage.hpp"

extern std::unique_ptr<ConfigManager> gConfigManager;
extern std::unique_ptr<MainWindow> gMainWindow;

Button::Style ExitBtnStyle()
{
	Button::Style s = Button::StyleNormal();
	s.look[0] = s.look[2] = Color(0xAB, 0x24, 0);
	s.look[1] = Color(0xD4, 0x2D, 0);
	s.textcolor[0] = s.textcolor[1] = s.textcolor[2] = White();
	s.focus_use_ok = false;
	return s;
}

SettingsPage::SettingsPage()
{
	CtrlLayout(*this);

	{
		optionStartHidden_.Set(ScanInt(gConfigManager->Load("StartHidden", "0")));
		optionStartHidden_.WhenAction = [&] { gConfigManager->Store("StartHidden", optionStartHidden_.Get()); };
		auto w = Helper::CalcSize(optionStartHidden_.GetLabel(), optionStartHidden_.GetFont()) + 16;
		optionStartHidden_.LeftPosZ(optionStartHidden_.GetPos().x.GetA(), w);
	}

	{
		optionUseTask_.Set(gConfigManager->TaskExists());
		optionUseTask_.WhenAction = [&] {
			gConfigManager->ManageTask(optionUseTask_.Get() == 1);

			auto b = gConfigManager->TaskExists();
			if(b != (optionUseTask_.Get() == 1)) { // revert
				optionUseTask_.Set(b);
			}
		};
		auto w = Helper::CalcSize(optionUseTask_.GetLabel(), optionUseTask_.GetFont()) + 16;
		optionUseTask_.LeftPosZ(optionUseTask_.GetPos().x.GetA(), w);
	}

	{
		auto wTxt = Helper::CalcSize(textLogLimit_.GetText(), textLogLimit_.GetFont());
		textLogLimit_.LeftPosZ(textLogLimit_.GetPos().x.GetA(), wTxt + 8);

		spinLogLimit_.LeftPosZ(textLogLimit_.GetPos().x.GetA() + textLogLimit_.GetSize().cx, spinLogLimit_.GetSize().cx);
		spinLogLimit_.SetText(
			IntStr(clamp(StrInt(gConfigManager->Load("LogDisplayLimit", "0")), spinLogLimit_.GetMin(), spinLogLimit_.GetMax())));
		spinLogLimit_.WhenAction = [&] {
			gConfigManager->Store("LogDisplayLimit", IntStr(clamp(StrInt(spinLogLimit_.GetText().ToString()),
			                                                      spinLogLimit_.GetMin(), spinLogLimit_.GetMax())));
		};
	}

	{
		auto wTxt = Helper::CalcSize(textWSLogLevel_.GetText(), textWSLogLevel_.GetFont());
		textWSLogLevel_.LeftPosZ(textWSLogLevel_.GetPos().x.GetA(), wTxt + 8);

		dropWSLogLevel_.Add("none", "None");
		dropWSLogLevel_.Add("info", "Info");
		dropWSLogLevel_.Add("debug", "Debug");
		auto level = gConfigManager->Load("WireSockLogLevel", "none");
		auto index = max(0, dropWSLogLevel_.FindKey(level));
		dropWSLogLevel_.SetIndex(index);

		auto wMax = 0;
		for(auto i = 0; i < dropWSLogLevel_.GetCount(); ++i) {
			auto text = dropWSLogLevel_.GetValue(i).ToString();
			auto w = Helper::CalcSize(text, GetStdFont());
			wMax = max(wMax, w);
		}
		dropWSLogLevel_.LeftPosZ(textWSLogLevel_.GetPos().x.GetA() + textWSLogLevel_.GetSize().cx, wMax + 20);

		dropWSLogLevel_.WhenAction = [&] {
			gConfigManager->Store("WireSockLogLevel", dropWSLogLevel_.GetKey(dropWSLogLevel_.GetIndex()));
		};
	}

	{
		btnOpenSettingsFolder_.SetImage(Rescale(AppIcons::Folder, 16, 16));
		auto w = Helper::CalcSize(btnOpenSettingsFolder_.GetLabel(), btnOpenSettingsFolder_.GetFont());
		btnOpenSettingsFolder_.LeftPosZ(btnOpenSettingsFolder_.GetPos().x.GetA(), w + 32);

		btnOpenSettingsFolder_.WhenAction = [] {
			auto path = (GetConfigFolder() << "\\").ToWString().ToStd();

			SHELLEXECUTEINFOW info{};
			info.cbSize = sizeof(info);
			info.fMask = SEE_MASK_NO_CONSOLE;
			info.lpFile = path.c_str();
			info.nShow = SW_SHOWNORMAL;
			ShellExecuteExW(&info);
		};
	}

	{
		exitStyle_ = ExitBtnStyle();
		btnExit_.SetStyle(exitStyle_);

		auto w = Helper::CalcSize(btnExit_.GetLabel(), btnExit_.GetFont(), 60);
		btnExit_.RightPosZ(btnExit_.GetPos().x.GetA(), w);

		btnExit_.WhenAction = [] { gMainWindow->Exit(); };
	}

	{
		imgAboutApp_.SetImage(Rescale(AppIcons::Icon64, 36, 36));

		textAppName_.SetText(GetAppName());

		auto ver = Helper::GetAppVersion();
		textAppVersion_.SetText(String("Version ") << IntStr(ver.r) << "." << IntStr(ver.g) << "." << IntStr(ver.b));

		btnOpenGithub_.SetImage(AppIcons::GitHub);
		auto w = Helper::CalcSize(btnOpenGithub_.GetLabel(), btnOpenGithub_.GetFont());
		btnOpenGithub_.LeftPosZ(btnOpenGithub_.GetPos().x.GetA(), w + 32);
		btnOpenGithub_.WhenAction = [] {
			SHELLEXECUTEINFOW info{};
			info.cbSize = sizeof(info);
			info.fMask = SEE_MASK_NO_CONSOLE;
			info.lpFile = L"https://github.com/hockerschwan/Kampos";
			info.nShow = SW_SHOWNORMAL;
			ShellExecuteExW(&info);
		};
	}
}

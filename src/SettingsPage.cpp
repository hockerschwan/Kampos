#include "ConfigManager.hpp"
#include "SettingsPage.hpp"

extern std::unique_ptr<ConfigManager> gConfigManager;

Button::Style ExitBtnStyle()
{
	Button::Style s = Button::StyleNormal();
	s.look[0] = s.look[2] = Color(0xAB, 0x24, 0);
	s.look[1] = Color(0xD4, 0x2D, 0);
	s.textcolor[0] = s.textcolor[1] = s.textcolor[2] = White();
	s.focus_use_ok = false;
	return s;
}

SettingsPanel::SettingsPanel()
{
	CtrlLayout(*this);

	{
		optionStartHidden_.Set(ScanInt(gConfigManager->Load("StartHidden", "0")));
		optionStartHidden_.WhenAction = [&] { gConfigManager->Store("StartHidden", optionStartHidden_.Get()); };
		auto w = Helper::CalcSize(optionStartHidden_.GetLabel(), optionStartHidden_.GetFont()) + Zx(15);
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
		auto w = Helper::CalcSize(optionUseTask_.GetLabel(), optionUseTask_.GetFont()) + Zx(15);
		optionUseTask_.LeftPosZ(optionUseTask_.GetPos().x.GetA(), w);
	}

	{
		auto wTxt = Helper::CalcSize(textLogLimit_.GetText(), textLogLimit_.GetFont());
		textLogLimit_.LeftPos(Zx(textLogLimit_.GetPos().x.GetA()), wTxt + Zx(12));

		spinLogLimit_.LeftPos(textLogLimit_.GetPos().x.GetA() + textLogLimit_.GetPos().x.GetB(), spinLogLimit_.GetSize().cx);
		spinLogLimit_.SetText(
			IntStr(clamp(StrInt(gConfigManager->Load("LogDisplayLimit", "0")), spinLogLimit_.GetMin(), spinLogLimit_.GetMax())));
		spinLogLimit_.WhenAction = [&] {
			gConfigManager->Store("LogDisplayLimit", IntStr(clamp(StrInt(spinLogLimit_.GetText().ToString()),
			                                                      spinLogLimit_.GetMin(), spinLogLimit_.GetMax())));
		};
	}

	{
		auto wTxt = Helper::CalcSize(textWSPath_.GetText(), textWSPath_.GetFont());
		textWSPath_.LeftPos(Zx(textWSPath_.GetPos().x.GetA()), wTxt + Zx(12));

		auto wBtn = Helper::CalcSize(btnWSPath_.GetLabel(), btnWSPath_.GetFont(), Zx(56));
		btnWSPath_.LeftPos(textWSPath_.GetPos().x.GetA() + textWSPath_.GetPos().x.GetB(), wBtn);
		btnWSPath_.WhenAction = [&] {
			FileSel sel{};
			sel.Type("wiresock-client.exe", "wiresock-client.exe");
			sel.ActiveDir(GetFileDirectory(gConfigManager->Load("WireSockPath")));

			if(sel.ExecuteOpen() != 1) {
				return;
			}

			auto path = sel.GetFile(0);
			editWSPath_.SetText(path);
			gConfigManager->Store("WireSockPath", path);
		};

		auto x = btnWSPath_.GetPos().x.GetA() + btnWSPath_.GetPos().x.GetB() + Zx(8);
		editWSPath_.HSizePos(x, Zx(editWSPath_.GetPos().x.GetB()));
		editWSPath_.SetText(gConfigManager->Load("WireSockPath", String::GetVoid()));
	}

	{
		textWSLogLevel_.LeftPos(textWSPath_.GetPos().x.GetA(), textWSPath_.GetPos().x.GetB());

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
		dropWSLogLevel_.LeftPos(textWSLogLevel_.GetPos().x.GetA() + textWSLogLevel_.GetPos().x.GetB() + 1, wMax + Zx(24));

		dropWSLogLevel_.WhenAction = [&] {
			gConfigManager->Store("WireSockLogLevel", dropWSLogLevel_.GetKey(dropWSLogLevel_.GetIndex()));
		};
	}

	{
		btnOpenSettingsFolder_.SetImage(Rescale(AppIcons::Folder(), Zx(15), Zx(15))); // 16 @100%
		auto w = Helper::CalcSize(btnOpenSettingsFolder_.GetLabel(), btnOpenSettingsFolder_.GetFont());
		btnOpenSettingsFolder_.LeftPosZ(btnOpenSettingsFolder_.GetPos().x.GetA(), w + Zx(30));

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

		auto w = Helper::CalcSize(btnExit_.GetLabel(), btnExit_.GetFont(), Zx(56));
		btnExit_.LeftPosZ(btnExit_.GetPos().x.GetA(), w);

		btnExit_.WhenAction = [&] { ExitPressed(); };
	}

	{
		imgAboutApp_.SetImage(Rescale(AppIcons::Icon64, Zx(34), Zx(34))); // 36 @100%

		textAppName_.SetText(GetAppName());

		auto ver = Helper::GetAppVersion();
		textAppVersion_.SetText(String("Version ")
		                        << IntStr(ver.Get(0)) << "." << IntStr(ver.Get(1)) << "." << IntStr(ver.Get(2)));

		btnOpenGithub_.SetImage(Rescale(AppIcons::GitHub(), Zx(15), Zx(15)));
		auto w = Helper::CalcSize(btnOpenGithub_.GetLabel(), btnOpenGithub_.GetFont());
		btnOpenGithub_.LeftPosZ(btnOpenGithub_.GetPos().x.GetA(), w + Zx(30));
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

SettingsPage::SettingsPage()
{
	CtrlLayout(*this);
	panel_.WhenExit = [&] { ExitPressed(); };

	scroll_.Add(panel_.HSizePosZ());
	scroll_.AddPane(panel_);
}

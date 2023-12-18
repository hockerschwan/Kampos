#include "Helper.hpp"
#include "Logger.hpp"
#include "MainWindow.hpp"
#include "NetworkMonitor.hpp"
#include "ProcessManager.hpp"
#include "TunnelsManager.hpp"

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<TunnelsManager> gTunnelsManager;
extern std::unique_ptr<ProcessManager> gProcessManager;
extern std::unique_ptr<NetworkMonitor> gNetworkMonitor;
extern std::unique_ptr<MainWindow> gMainWindow;

MainWindow::MainWindow()
	: tunnelsPage_(MakeOne<TunnelsPage>())
	, rulesPage_(MakeOne<RulesPage>())
	, logPage_(MakeOne<LogPage>())
	, settingsPage_(MakeOne<SettingsPage>())
	, tray_(MakeOne<TrayIcon>())
{
	CtrlLayout(*this);
	Zoomable().Sizeable().Icon(AppIcons::Icon16, AppIcons::Icon24);
	SetMinSize(Zsz(800, 570));
	WhenClose = [&] { Hide(); };

	gProcessManager->WhenStarted << [&](Id& id) { SetTitle(); };
	gProcessManager->WhenStopped << [&] {
		bitsRecv_ = bitsSent_ = -1;
		SetTitle();
	};

	gNetworkMonitor->WhenBitRate << [&](uint64 r, uint64 s) {
		bitsRecv_ = r;
		bitsSent_ = s;
		SetTitle();
	};

	settingsPage_->WhenExit = [&] { ShowExitPrompt(); };

	navigation_.ItemWidth(Zx(61)).ItemHeight(Zy(56)); // 65,64 @100%
	navigation_.RowMode().NoRoundSize().AutoHideSb(true).SetDisplay(CenteredImageDisplay());
	navigation_.Add(Rescale(AppIcons::Tunnels(), Zx(30), Zx(30)), true); // 32 @100%
	navigation_.Add(Rescale(AppIcons::Rules(), Zx(30), Zx(30)), true);
	navigation_.Add(Rescale(AppIcons::Log(), Zx(30), Zx(30)), true);
	navigation_.Add(Rescale(AppIcons::Settings(), Zx(30), Zx(30)), true);
	navigation_.WhenSel = [&] { SetContent(); };

	tray_->WhenBar = [&](Bar& bar) { ShowTrayMenu(bar); };
	tray_->WhenLeftDown = [&] { ShowWindow(); };
	tray_->Icon(AppIcons::Icon16);

	SetTitle();

	gNetworkMonitor->Start();
}

void MainWindow::SetContent()
{
	while(content_.GetChildCount() > 0) {
		content_.RemoveChild(content_.GetIndexChild(0));
	}

	switch(navigation_.GetCursor()) {
	case 0:
		content_.Add(tunnelsPage_->SizePos());
		break;
	case 1:
		content_.Add(rulesPage_->SizePos());
		break;
	case 2:
		content_.Add(logPage_->SizePos());
		logPage_->ScrollBottom();
		break;
	case 3:
		content_.Add(settingsPage_->SizePos());
		break;
	}
}

void MainWindow::SetTitle()
{
	PostCallback([&] {
		auto id = gProcessManager->GetCurrentId();
		if(id.IsNull()) {
			Title("Disconnected");
			tray_->Tip("Disconnected");
			return;
		}

		TunnelConfig cfg{};
		if(!gTunnelsManager->GetConfig(id, cfg)) {
			return;
		}

		String str{};
		str << "Connected to " << cfg.Interface.Name;

		if(bitsRecv_ >= 0 || bitsSent_ >= 0) {
			double r{};
			String pfxR = Helper::FormatWithPrefix(bitsRecv_, r);
			if(pfxR.IsEmpty()) {
				str << " | R:" << Format("%5d", r) << " bps";
			}
			else {
				str << " | R:" << Format("%5>.1f", r) << pfxR << "bps";
			}

			double s{};
			String pfxS = Helper::FormatWithPrefix(bitsSent_, s);
			if(pfxS.IsEmpty()) {
				str << " S:" << Format("%5d", s) << " bps";
			}
			else {
				str << " S:" << Format("%5>.1f", s) << pfxS << "bps";
			}
		}

		if(this->IsShown()) {
			Title(str);
		}

		auto tip = str;
		tip.Replace(" | ", "\n");
		tray_->Tip(tip);
	});
}

void MainWindow::ShowExitPrompt()
{
	exitPromptShown_ = true;
	if(PromptYesNo("Are you sure you want to exit?") == 1) {
		gLogger->Log("Shutting down...");
		Hide();
		Shutdown();

		Thread::BeginShutdownThreads();
		gLogger->ClearEvents();

		gProcessManager->Stop();
		gProcessManager->ClearEvents();

		gNetworkMonitor->Stop();
		gNetworkMonitor->ClearEvents();

		ShutdownThreads();
		Thread::EndShutdownThreads();

		tray_.Clear();
		tunnelsPage_.Clear();
		rulesPage_.Clear();
		logPage_.Clear();
		settingsPage_.Clear();

		gLogger->Log("Destroying MainWindow...");
		auto del = gMainWindow.get_deleter();
		del(gMainWindow.release());
		return;
	}
	exitPromptShown_ = false;
}

void MainWindow::ShowTrayMenu(Bar& bar)
{
	auto iconSize = Zx(15);

	if(!gProcessManager->GetCurrentId().IsNull()) {
		bar.Add("Disconnect", Rescale(AppIcons::Disconnect(), iconSize, iconSize), [&] { gProcessManager->Stop(); });
	}

	bar.Sub("Tunnels", Rescale(AppIcons::Tunnels(), iconSize, iconSize), [&](Bar& b) { ShowTunnelsSubMenu(b); });
	bar.Separator();
	bar.Add("Exit", CtrlImg::remove, [&] { ShowExitPrompt(); });
}

void MainWindow::ShowTunnelsSubMenu(Bar& bar)
{
	auto current = gProcessManager->GetCurrentId();
	auto tunnels = gTunnelsManager->GetTunnels();
	for(const auto& item : ~(tunnels)) {
		auto id = item.key;
		if(id == current) {
			bar.Add(item.value.Interface.Name, [id] { gProcessManager->Start(id); }).Radio(true);
		}
		else {
			bar.Add(item.value.Interface.Name, [id] { gProcessManager->Start(id); });
		}
	}
}

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

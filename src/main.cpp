#include "ConfigManager.hpp"
#include "Logger.hpp"
#include "MainWindow.hpp"
#include "ProcessManager.hpp"
#include "TunnelsManager.hpp"
#include <Core/Core.h>
#include <shlobj.h>

#define IMAGECLASS AppIcons
#define IMAGEFILE <Kampos/src/icons.iml>
#include <Draw/iml_source.h>

std::unique_ptr<Logger> gLogger;
std::unique_ptr<ConfigManager> gConfigManager;
std::unique_ptr<TunnelsManager> gTunnelsManager;
std::unique_ptr<ProcessManager> gProcessManager;
std::unique_ptr<MainWindow> gMainWindow;

int IsAlreadyRunning()
{
	auto exe = GetAppName() += ".exe";
	auto cmd = String("tasklist /fi \"imagename eq ");
	cmd << exe << "\" /fo csv";

	auto str = Sys(cmd);
	if(str.IsEmpty()) {
		return -1;
	}

	auto pid = FormatUnsigned(GetCurrentProcessId());
	auto rows = Split(str, "\r\n");
	for(auto& row : rows) {
		row.Replace("\"", "");
		auto words = Split(row, ",");

		if(words[0] != exe) {
			continue;
		}

		if(words[1] != pid) {
			return 1;
		}
	}
	return 0;
}

INITBLOCK
{
	SetAppName("Kampos");

	auto n = IsAlreadyRunning();
	if(n != 0) {
		Exit(n);
	}

	{
		WCHAR* wcRoaming;
		if(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &wcRoaming) != S_OK) {
			Exit(-2);
		}

		auto roaming = String(wcRoaming);
		if(!roaming.EndsWith("\\")) {
			roaming += "\\";
		}
		SetHomeDirectory(roaming);
		UseHomeDirectoryConfig(true);
	}

	gLogger = std::make_unique<Logger>();
	gConfigManager = std::make_unique<ConfigManager>();
	gTunnelsManager = std::make_unique<TunnelsManager>();
	gProcessManager = std::make_unique<ProcessManager>();
}

GUI_APP_MAIN
{
	if(!gProcessManager->ClientInstalled()) {
		auto message = String() << t_("Could not find") << " wiresock-client.exe&" << t_("Download from");
		message << " [^https://www.wiresock.net/wiresock-vpn-client/download-wiresock-vpn-client/^ " << t_("here") << "]";
		if(Prompt(BEEP_EXCLAMATION, Ctrl::GetAppName(), CtrlImg::exclamation(), message, true, t_("Continue"), t_("Exit")) != 1) {
			Exit(-3);
		}
	}

	gLogger->Log("Application started.");

	bool hide = ScanInt(gConfigManager->Load("StartHidden", "0")) == 1;

	gMainWindow = std::make_unique<MainWindow>();
	gMainWindow->OpenMain();
	gLogger->Log("MainWindow opened.");
	if(hide) {
		gMainWindow->Hide();
	}

	Ctrl::EventLoop();
}

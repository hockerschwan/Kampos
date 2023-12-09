#include "ConfigManager.hpp"
#include "Helper.hpp"
#include "Logger.hpp"
#include "MainWindow.hpp"
#include "ProcessManager.hpp"
#include "TunnelsManager.hpp"
#include <plugin/pcre/Pcre.h>

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<ConfigManager> gConfigManager;
extern std::unique_ptr<TunnelsManager> gTunnelsManager;
extern std::unique_ptr<MainWindow> gMainWindow;

bool ProcessManager::ClientInstalled()
{
	auto path = gConfigManager->Load("WireSockPath", String::GetVoid());
	return FileExists(path);
}

bool ProcessManager::Start(const Id& uuid)
{
	if(!ClientInstalled()) {
		uuid_ = String::GetVoid();
		return false;
	}
	else if(!Stop()) {
		return false;
	}

	TunnelConfig cfg{};
	auto tunnels = gTunnelsManager->GetTunnels();
	for(const auto& item : ~(tunnels)) {
		auto& id = item.key;
		if(id == uuid) {
			cfg = clone(item.value);
			break;
		}
	}
	if(cfg.Interface.UUID == Helper::GetVoidUuid()) {
		return false;
	}

	auto cmd = String("\"") << gConfigManager->Load("WireSockPath") << "\" run -config \"";
	cmd << Helper::TunnelsPath() << cfg.Interface.Name << ".conf\" ";
	cmd << "-log-level " << gConfigManager->Load("WireSockLogLevel", "none") << " -lac";

	process_.Attach(new LocalProcess(cmd));
	thread_.Create();
	if(!thread_->RunNice([&] { Read(); })) {
		uuid_ = String::GetVoid();
		return false;
	}

	gLogger->Log(String("Client spawned: connecting to ") << cfg.Interface.Name);
	uuid_ = uuid;
	return true;
}

bool ProcessManager::Stop()
{
	if(!process_.IsEmpty()) {
		process_->Kill();
		process_.Clear();
	}

	if(!thread_.IsEmpty() && thread_->IsOpen()) {
		thread_->Wait();
		thread_.Clear();
		gLogger->Log("Client terminated");
	}

	uuid_ = String::GetVoid();

	if(gMainWindow && !gMainWindow->IsShutdown()) {
		Thread t{};
		t.Run([&] {
			t.Sleep(110);
			Stopped();
		});
	}

	return process_.IsEmpty() && (thread_.IsEmpty() || !thread_->IsOpen());
}

void ProcessManager::Read()
{
	RegExp re("^([\\d -:]{20}?)");

	String out{};
	while(!Thread::IsShutdownThreads() && !process_.IsEmpty() && process_->IsRunning()) {
		out.Clear();
		while(true) {
			process_->Read(out);
			if(out.IsEmpty()) {
				break;
			}

			for(auto& line : Split(out, "\r\n")) {
				if(re.Match(line)) {
					re.Replace(line, String::GetVoid(), false);
				}
				gLogger->Log(TrimBoth(line));
			}
		}

		thread_->Sleep(100);
	}
}

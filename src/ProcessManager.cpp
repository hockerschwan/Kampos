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

bool ProcessManager::Start(const Id& uuid, bool stop)
{
	if(!ClientInstalled()) {
		SetUUID(String::GetVoid());
		return false;
	}

	if(!stop && uuid_ == uuid) {
		return true;
	}
	else if(!Stop()) {
		return false;
	}

	TunnelConfig cfg{};
	if(!gTunnelsManager->GetConfig(uuid, cfg)) {
		return false;
	}

	auto cmd = String("\"") << gConfigManager->Load("WireSockPath") << "\" run -config \"";
	cmd << Helper::TunnelsPath() << cfg.Interface.Name << ".conf\" ";
	cmd << "-log-level " << gConfigManager->Load("WireSockLogLevel", "none") << " -lac";

	for(const auto& cmd : cfg.Interface.PreUp) {
		auto str = Sys(cmd);
		Log(str);
	}

	process_.Attach(new LocalProcess(cmd));
	thread_.Create();
	if(!thread_->RunNice([&] { Read(); })) {
		SetUUID(String::GetVoid());
		return false;
	}

	gLogger->Log(String("Client spawned: connecting to ") << cfg.Interface.Name);

	Thread t{};
	t.Run([&, uuid] {
		t.Sleep(250);
		if(process_.IsEmpty() || !process_->IsRunning()) {
			return;
		}
		SetUUID(uuid);

		TunnelConfig cfg{};
		if(!gTunnelsManager->GetConfig(uuid_, cfg)) {
			return;
		}

		WhenStarted(uuid_);
		for(const auto& cmd : cfg.Interface.PostUp) {
			auto str = Sys(cmd);
			Log(str);
		}
	});

	return true;
}

bool ProcessManager::Stop()
{
	SetUUID(String::GetVoid());

	if(!process_.IsEmpty()) {
		process_->Kill();
		process_.Clear();
	}

	if(!thread_.IsEmpty() && thread_->IsOpen()) {
		cv_.Signal();
		thread_.Clear();
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

		cv_.Wait(mThread_, 100);
	}
	process_.Clear();
	SetUUID(String::GetVoid());

	TunnelConfig cfg{};
	auto b = gTunnelsManager->GetConfig(uuid_, cfg);
	if(b) {
		for(const auto& cmd : cfg.Interface.PreDown) {
			auto str = Sys(cmd);
			gLogger->Log(str);
		}
	}

	gLogger->Log("Client terminated");
	if(gMainWindow && !gMainWindow->IsShutdown()) {
		WhenStopped();
	}

	if(b) {
		for(const auto& cmd : cfg.Interface.PostDown) {
			auto str = Sys(cmd);
			gLogger->Log(str);
		}
	}

	return;
}

void ProcessManager::Log(const String& str)
{
	if(str.IsEmpty()) {
		return;
	}

	for(auto& line : Split(str, "\r\n")) {
		line = TrimRight(line);
		if(!line.IsEmpty()) {
			gLogger->Log(line);
		}
	}
}

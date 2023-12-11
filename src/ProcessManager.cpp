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
		SetUUID(String::GetVoid());
		return false;
	}

	if(!Stop()) {
		return false;
	}

	TunnelConfig cfg{};
	if(!gTunnelsManager->GetConfig(uuid, cfg)) {
		return false;
	}

	auto cmd = String("\"") << gConfigManager->Load("WireSockPath") << "\" run -config \"";
	cmd << Helper::TunnelsPath() << cfg.Interface.Name << ".conf\" ";
	cmd << "-log-level " << gConfigManager->Load("WireSockLogLevel", "none") << " -lac";

	process_.Attach(new LocalProcess(cmd));
	thread_.Create();
	if(!thread_->RunNice([&] { Read(); })) {
		SetUUID(String::GetVoid());
		return false;
	}

	gLogger->Log(String("Client spawned: connecting to ") << cfg.Interface.Name);
	SetUUID(uuid);
	Started();
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

	MIB_IFROW adapter{};
	auto name = "WireSock Virtual Adapter";
	unsigned long bytesRecv = 0;
	unsigned long bytesSent = 0;
	bitsRecv_ = bitsSent_ = 0;
	int counter = 0;

	while(!Thread::IsShutdownThreads() && !process_.IsEmpty() && process_->IsRunning()) {
		{
			memset(&adapter, 0, sizeof(adapter));
			if(GetAdapterInfo("WireSock Virtual Adapter", adapter)) {
				if(adapter.dwInOctets == 0 || adapter.dwOutOctets == 0) {
					bytesRecv = bytesSent = 0;
				}
				else {
					bitsRecv_ += 8 * (adapter.dwInOctets - bytesRecv);
					bitsSent_ += 8 * (adapter.dwOutOctets - bytesSent);

					bytesRecv = adapter.dwInOctets;
					bytesSent = adapter.dwOutOctets;
				}
			}

			if(++counter == 10) {
				BitRate();
				counter = 0;
				bitsRecv_ = bitsSent_ = 0;
			}
		}

		{
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
		}

		cv_.Wait(mThread_, 100);
	}

	gLogger->Log("Client terminated");
	SetUUID(String::GetVoid());
	if(gMainWindow && !gMainWindow->IsShutdown()) {
		Stopped();
	}
	return;
}

bool ProcessManager::GetAdapterInfo(const char* name, MIB_IFROW& info)
{
	MIB_IFROW ifrow{};
	{
		auto size = 128;
		unsigned long outBufLen = sizeof(IP_ADAPTER_INFO) * size;
		auto adapterInfo = std::make_unique<IP_ADAPTER_INFO[]>(size);

		if(GetAdaptersInfo(adapterInfo.get(), &outBufLen) != ERROR_SUCCESS) {
			return false;
		}

		IP_ADAPTER_INFO info_ = adapterInfo[0];
		while(info_.Next != nullptr) {
			if(strcmp(info_.Description, name) == 0) {
				break;
			}
			info_ = *info_.Next;
		}

		ifrow.dwIndex = info_.Index;
		if(GetIfEntry(&ifrow) != NO_ERROR) {
			return false;
		}
	}

	if(strcmp((LPCSTR)ifrow.bDescr, name) != 0) {
		return false;
	}

	info = ifrow;
	return true;
}

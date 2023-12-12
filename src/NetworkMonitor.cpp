#include "Logger.hpp"
#include "NetworkMonitor.hpp"
#include <iphlpapi.h>

extern std::unique_ptr<Logger> gLogger;

bool NetworkMonitor::Start()
{
	thread_.Create();
	if(!thread_->RunNice([&] { Read(); })) {
		return false;
	}

	return true;
}

bool NetworkMonitor::Stop()
{
	cv_.Broadcast();
	thread_.Clear();

	return thread_.IsEmpty();
}

void NetworkMonitor::Read()
{
	while(!Thread::IsShutdownThreads()) {
		// get physical adapters
		Array<String> adaptersEthernet{};
		Array<String> adaptersWifi{};
		{
			auto size = 128;
			unsigned long outBufLen = sizeof(IP_ADAPTER_ADDRESSES_LH) * size;
			auto adapterInfo = std::make_unique<IP_ADAPTER_ADDRESSES_LH[]>(size);

			if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, adapterInfo.get(), &outBufLen) != ERROR_SUCCESS) {
				continue;
			}

			IP_ADAPTER_ADDRESSES_LH info = adapterInfo[0];
			while(info.Next != nullptr) {
				if(info.Flags & IP_ADAPTER_REGISTER_ADAPTER_SUFFIX) {
					auto uuid = String(info.AdapterName);
					uuid.TrimStart("{");
					uuid.TrimEnd("}");

					switch(info.IfType) {
					case MIB_IF_TYPE_ETHERNET:
						adaptersEthernet.Add(uuid);
						break;
					case IF_TYPE_IEEE80211:
						adaptersWifi.Add(uuid);
						break;
					}
				}

				info = *info.Next;
			}
		}

		// get connections
		Array<String> connectionsEthernet{};
		Array<String> connectionsWifi{};
		{
			auto cmpol = GetWinRegString("CMPOL", "SOFTWARE\\Microsoft\\WcmSvc");
			auto len = cmpol.GetLength();
			Array<String> lines{};
			String str{};
			for(int i = 0; i < len - 1; ++i) {
				if(cmpol[i] == 0) {
					lines.Add(str);
					str.Clear();
				}
				else {
					str << ToAscii(cmpol[i]);
				}
			}

			for(int i = 0; i < lines.GetCount(); i += 4) {
				auto id = ToUpper(lines[i]);
				auto name = lines[i + 1];
				if(FindIndex(adaptersEthernet, id) >= 0) {
					connectionsEthernet.Add(name);
				}
				else if(FindIndex(adaptersWifi, id) >= 0) {
					connectionsWifi.Add(name);
				}
			}
		}

		cv_.Wait(mThread_, 5000);
	}
}

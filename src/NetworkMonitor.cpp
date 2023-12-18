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
	cv_.Signal();
	thread_.Clear();

	return thread_.IsEmpty();
}

void NetworkMonitor::Read()
{
	Index<String> connectionsEthernetOld{};
	Index<String> connectionsWifiOld{};

	WString virtualAdapterName{"WireSock Virtual Adapter"};
	int64 bitsRecvOld = 0, bitsSentOld = 0;

	while(!Thread::IsShutdownThreads()) {
		ArrayMap<String, String> connections{}; // uuid, name
		{
			auto cmpol = GetWinRegString("CMPOL", "SOFTWARE\\Microsoft\\WcmSvc");
			Array<String> lines{};
			String line{};
			for(const auto& ch : cmpol) {
				if(ch == '\0') {
					if(line.IsEmpty()) {
						break;
					}
					lines.Add(line);
					line.Clear();
					continue;
				}
				line << ch;
			}
			for(int i = 0; i < lines.GetCount(); i += 4) {
				auto id = lines[i];
				auto name = lines[i + 1];
				connections.Add(id, name);
			}
		}

		Index<String> connectionsEthernet{};
		Index<String> connectionsWifi{};

		MIB_IFROW virtualAdapter{};

		DWORD size = 0;
		if(GetNumberOfInterfaces(&size) != NO_ERROR) {
			continue;
		}

		unsigned long outBufLen = sizeof(IP_ADAPTER_ADDRESSES_LH) * size;
		auto adapterInfo = std::make_unique<IP_ADAPTER_ADDRESSES_LH[]>(size);

		if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, adapterInfo.get(), &outBufLen) != ERROR_SUCCESS) {
			continue;
		}

		IP_ADAPTER_ADDRESSES_LH info = adapterInfo[0];
		while(true) {
			if(info.OperStatus == 1) {
				if(info.IfType == IF_TYPE_PROP_VIRTUAL) {
					if(virtualAdapterName.Compare(info.Description) == 0) {
						virtualAdapter.dwIndex = info.IfIndex;
						GetIfEntry(&virtualAdapter);
					}
				}
				else if(info.IfType == MIB_IF_TYPE_ETHERNET || info.IfType == IF_TYPE_IEEE80211) {
					String id{ToLower(info.AdapterName)};
					id.TrimStart("{");
					id.TrimEnd("}");

					if(connections.Find(id) >= 0) {
						auto name = connections.Get(id);

						switch(info.IfType) {
						case MIB_IF_TYPE_ETHERNET:
							connectionsEthernet.FindAdd(name);
							break;
						case IF_TYPE_IEEE80211:
							connectionsWifi.FindAdd(name);
							break;
						}
					}
				}
			}

			if(info.Next == nullptr) {
				break;
			}
			info = *info.Next;
		}

		{
			if(connectionsEthernet != connectionsEthernetOld || connectionsWifi != connectionsWifiOld) {
				connectionsEthernetOld = clone(connectionsEthernet);
				connectionsWifiOld = clone(connectionsWifi);

				NetworkDetected(connectionsEthernet, connectionsWifi);
			}
		}

		{
			int64 bitsRecv = 0, bitsSent = 0;
			if(virtualAdapter.dwOperStatus >= IF_OPER_STATUS_CONNECTED) {
				bitsRecv = 8 * (virtualAdapter.dwInOctets - bitsRecvOld);
				bitsSent = 8 * (virtualAdapter.dwOutOctets - bitsSentOld);

				bitsRecvOld = virtualAdapter.dwInOctets;
				bitsSentOld = virtualAdapter.dwOutOctets;
			}
			WhenBitRate(bitsRecv, bitsSent);
		}

		cv_.Wait(mThread_, 1000);
	}
}

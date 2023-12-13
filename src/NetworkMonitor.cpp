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
					WString name{};
					auto i = 0;
					while(true) {
						WCHAR ch = info.FriendlyName[i];
						if(ch == '\0') {
							break;
						}
						name << ch;

						++i;
					};

					switch(info.IfType) {
					case MIB_IF_TYPE_ETHERNET:
						connectionsEthernet.FindAdd(name.ToString());
						break;
					case IF_TYPE_IEEE80211:
						connectionsWifi.FindAdd(name.ToString());
						break;
					}
				}
			}

			if(info.Next == nullptr) {
				break;
			}
			info = *info.Next;
		}

		{
			auto b1 = connectionsEthernet != connectionsEthernetOld;
			auto b2 = connectionsWifi != connectionsWifiOld;
			if(b1 || b2) {
				Bits b{};
				b.Set(0, b1);
				b.Set(1, b2);

				connectionsEthernetOld = clone(connectionsEthernet);
				connectionsWifiOld = clone(connectionsWifi);

				WhenNetworkChanged(connectionsEthernet, connectionsWifi, b);
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

#ifndef _TunnelStructs_hpp_
#define _TunnelStructs_hpp_

#include <stduuid/uuid.h>

class TunnelInterface : Moveable<TunnelInterface>, DeepCopyOption<TunnelInterface> {
public:
	SortedIndex<String> Address{};
	SortedIndex<String> DNS{};
	Value MTU = 0;
	String PrivateKey{};

	Id UUID = Id(Helper::GetVoidUuid()); // # UUID = 0000-0000-...
	String Name{};                       // # Name = tunnel1

	TunnelInterface(){};
	TunnelInterface(const TunnelInterface& x, int n)
	{
		Address = clone(x.Address);
		DNS = clone(x.DNS);
		MTU = x.MTU;
		PrivateKey = clone(x.PrivateKey);
		UUID = x.UUID;
		Name = x.Name;
	};

	String ToString() const
	{
		String str{};
		str << "[Interface]\n# Name = " << Name << "\n# UUID = " << UUID.ToString() << "\nAddress = ";
		for(int i = 0; i < Address.GetCount(); ++i) {
			str << Address[i] << (i == Address.GetCount() - 1 ? String::GetVoid() : ", ");
		}

		str << "\nDNS = ";
		for(int i = 0; i < DNS.GetCount(); ++i) {
			str << DNS[i] << (i == DNS.GetCount() - 1 ? String::GetVoid() : ", ");
		}

		str << "\nMTU = " << MTU.ToString() << "\nPrivateKey = " << PrivateKey << "\n";
		return str;
	};
};

class TunnelPeer : Moveable<TunnelPeer>, DeepCopyOption<TunnelPeer> {
public:
	SortedIndex<String> AllowedApps{};
	SortedIndex<String> AllowedIPs{};
	SortedIndex<String> DisallowedApps{};
	SortedIndex<String> DisallowedIPs{};
	String Endpoint{};
	Value PersistentKeepalive = 0;
	String PresharedKey{};
	String PublicKey{};

	String Name{}; // # Name = peer1

	TunnelPeer(){};
	TunnelPeer(const TunnelPeer& x, int n)
	{
		AllowedApps = clone(x.AllowedApps);
		AllowedIPs = clone(x.AllowedIPs);
		DisallowedApps = clone(x.DisallowedApps);
		DisallowedIPs = clone(x.DisallowedIPs);
		Endpoint = x.Endpoint;
		PersistentKeepalive = x.PersistentKeepalive;
		PresharedKey = x.PresharedKey;
		PublicKey = x.PublicKey;
	};

	String ToString() const
	{
		String str{};
		str << "[Peer]\n# Name = " << Name << "\nAllowedApps = ";
		for(int i = 0; i < AllowedApps.GetCount(); ++i) {
			str << AllowedApps[i] << (i == AllowedApps.GetCount() - 1 ? String::GetVoid() : ", ");
		}

		str << "\nAllowedIPs = ";
		for(int i = 0; i < AllowedIPs.GetCount(); ++i) {
			str << AllowedIPs[i] << (i == AllowedIPs.GetCount() - 1 ? String::GetVoid() : ", ");
		}

		str << "\nDisallowedApps = ";
		for(int i = 0; i < DisallowedApps.GetCount(); ++i) {
			str << DisallowedApps[i] << (i == DisallowedApps.GetCount() - 1 ? String::GetVoid() : ", ");
		}

		str << "\nDisallowedIPs = ";
		for(int i = 0; i < DisallowedIPs.GetCount(); ++i) {
			str << DisallowedIPs[i] << (i == DisallowedIPs.GetCount() - 1 ? String::GetVoid() : ", ");
		}

		str << "\nEndpoint = " << Endpoint << "\nPersistentKeepalive = " << PersistentKeepalive.ToString();
		str << "\nPresharedKey = " << PresharedKey << "\nPublicKey = " << PublicKey << "\n";
		return str;
	};
};

class TunnelConfig : Moveable<TunnelConfig>, DeepCopyOption<TunnelConfig> {
public:
	TunnelInterface Interface{};
	Array<TunnelPeer> Peers{};

	TunnelConfig(){};
	TunnelConfig(const TunnelConfig& x, int n)
		: Interface(x.Interface, n)
		, Peers(x.Peers, n){};

	String ToString() const
	{
		String str{};
		str << Interface.ToString() << "\n";
		for(const auto& p : Peers) {
			str << p.ToString() << "\n";
		}
		return str;
	};

	bool operator<(const TunnelConfig& rhs) const { return ToLower(this->Interface.Name) < ToLower(rhs.Interface.Name); }
};

#endif

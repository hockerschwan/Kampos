#ifndef _TunnelStructs_hpp_
#define _TunnelStructs_hpp_

#include "Helper.hpp"
#include <Core/Core.h>

using namespace Upp;

class TunnelInterface : Moveable<TunnelInterface>, DeepCopyOption<TunnelInterface> {
public:
	Array<String> Address{};
	Array<String> DNS{};
	Value MTU = 0;
	String PrivateKey{};
	Array<String> PreUp{};
	Array<String> PostUp{};
	Array<String> PreDown{};
	Array<String> PostDown{};

	Id UUID = Id(Helper::GetVoidUuid()); // # UUID = 00000000-0000-...
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
		PreUp = clone(x.PreUp);
		PostUp = clone(x.PostUp);
		PreDown = clone(x.PreDown);
		PostDown = clone(x.PostDown);
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

		for(const auto& cmd : PreUp) {
			str << "PreUp = " << cmd << "\n";
		}

		for(const auto& cmd : PostUp) {
			str << "PostUp = " << cmd << "\n";
		}

		for(const auto& cmd : PreDown) {
			str << "PreDown = " << cmd << "\n";
		}

		for(const auto& cmd : PostDown) {
			str << "PostDown = " << cmd << "\n";
		}

		return str;
	};

	bool operator==(const TunnelInterface& rhs) const
	{
		return Address == rhs.Address && DNS == rhs.DNS && MTU == rhs.MTU && PrivateKey == rhs.PrivateKey;
	};
};

class TunnelPeer : Moveable<TunnelPeer>, DeepCopyOption<TunnelPeer> {
public:
	Array<String> AllowedApps{};
	Array<String> AllowedIPs{};
	Array<String> DisallowedApps{};
	Array<String> DisallowedIPs{};
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
		Name = x.Name;
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

	static TunnelPeer GetDefault()
	{
		TunnelPeer peer{};

		peer.AllowedIPs.Add("0.0.0.0/0");
		peer.AllowedIPs.Add("::/0");

		// https://en.wikipedia.org/wiki/IP_address#Private_addresses
		peer.DisallowedIPs.Add("10.0.0.0/8");
		peer.DisallowedIPs.Add("172.16.0.0/12");
		peer.DisallowedIPs.Add("192.168.0.0/16");
		peer.DisallowedIPs.Add("fc00::/7");

		peer.Name = "New peer";
		peer.PersistentKeepalive = 30;

		return pick(peer);
	};

	bool operator==(const TunnelPeer& rhs) const
	{
		return AllowedApps == rhs.AllowedApps && AllowedIPs == rhs.AllowedIPs && DisallowedApps == rhs.DisallowedApps &&
		       DisallowedIPs == rhs.DisallowedIPs && Endpoint == rhs.Endpoint && PersistentKeepalive == rhs.PersistentKeepalive &&
		       PresharedKey == rhs.PresharedKey && PublicKey == rhs.PublicKey;
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

	static TunnelConfig GetVoid() { return pick(TunnelConfig()); };

	bool operator<(const TunnelConfig& rhs) const { return ToLower(this->Interface.Name) < ToLower(rhs.Interface.Name); }

	bool operator==(const TunnelConfig& rhs) const { return Interface == rhs.Interface && Peers == rhs.Peers; };

	bool operator!=(const TunnelConfig& rhs) { return !(*this == rhs); };
};

#endif

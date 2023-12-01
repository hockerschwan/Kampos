#ifndef _TunnelStructs_hpp_
#define _TunnelStructs_hpp_

#include <stduuid/uuid.h>

class TunnelInterface : DeepCopyOption<TunnelInterface> {
public:
	SortedIndex<String> Address{};
	SortedIndex<String> DNS{};
	Value MTU = 0;
	String PrivateKey{};

	Id UUID = Id(Helper::GetVoidUuid()); // # UUID = 0000-0000-...
	String Name{};                       // # Name = tunnel1

	TunnelInterface(){};
	TunnelInterface(const TunnelInterface& x, int)
		: Address(x.Address, 1)
		, DNS(x.DNS, 1)
		, MTU(x.MTU)
		, PrivateKey(x.PrivateKey, 1)
		, UUID(x.UUID)
		, Name(x.Name){};

	String ToString() const
	{
		String str{};
		str << "{Address:" << Address.ToString() << ", DNS:" << DNS.ToString() << ", MTU:" << MTU;
		str << ", PrivateKey:" << PrivateKey << ", UUID:" << UUID << ", Name:" << Name << "}";
		return str;
	};
};

class TunnelPeer : DeepCopyOption<TunnelPeer> {
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
	TunnelPeer(const TunnelPeer& x, int)
		: AllowedApps(x.AllowedApps, 1)
		, AllowedIPs(x.AllowedIPs, 1)
		, DisallowedApps(x.DisallowedApps, 1)
		, DisallowedIPs(x.DisallowedIPs, 1)
		, Endpoint(x.Endpoint)
		, PersistentKeepalive(x.PersistentKeepalive)
		, PresharedKey(x.PresharedKey)
		, PublicKey(x.PublicKey){};

	String ToString() const
	{
		String str{};
		str << "{AllowedApps:" << AllowedApps.ToString() << ", AllowedIPs:" << AllowedIPs.ToString();
		str << ", DisallowedApps:" << DisallowedApps.ToString() << ", DisallowedIPs:" << DisallowedIPs.ToString();
		str << ", Endpoint:" << Endpoint << ", PersistentKeepalive:" << PersistentKeepalive;
		str << ", PresharedKey:" << PresharedKey << ", PublicKey:" << PublicKey << ", Name:" << Name << "}";
		return str;
	};
};

class TunnelConfig : DeepCopyOption<TunnelConfig> {
public:
	TunnelInterface Interface{};
	Array<TunnelPeer> Peers{};

	TunnelConfig(){};
	TunnelConfig(const TunnelConfig& x, int)
		: Interface(x.Interface, 1)
		, Peers(x.Peers, 1){};

	String ToString() const
	{
		String str{};
		str << "Interface:" << Interface.ToString() << ", Peers:" << Peers.ToString();
		return str;
	};
};

#endif

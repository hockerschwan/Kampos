#include "NetworkMonitor.hpp"
#include "RuleStructs.hpp"

extern std::unique_ptr<NetworkMonitor> gNetworkMonitor;

RuleConditionSSID::RuleConditionSSID()
	: RuleConditionBase()
{
	Type = RULE_SSID;
	gNetworkMonitor->WhenNetworkChanged <<
		[&](Index<String>& eth, Index<String>& wifi, Bits& bits) { IsSatisfied = eth.Find(SSID) >= 0 || wifi.Find(SSID) >= 0; };
}

RuleConditionSSID::RuleConditionSSID(const RuleConditionSSID& x, int n)
	: RuleConditionSSID()
{
	IsSatisfied = x.IsSatisfied;
	Negative = x.Negative;
	SSID = x.SSID;
}

String RuleConditionSSID::ToString() const
{
	String str{};
	str << "{\n\t\"Type\": \"" << (int)Type << "\",\n\t\"Negative\": \"" << (int)Negative;
	str << "\"\n\t\"SSID\": \"" << SSID << "\"\n}";

	return pick(str);
}

String RuleConditionSSID::Text() const
{
	String str{};
	str << "Connected to " << SSID;

	return pick(str);
}

void RuleConditionSSID::Check()
{
	// todo
}

RuleConditionAnyNetwork::RuleConditionAnyNetwork()
	: RuleConditionBase()
{
	Type = RULE_ANYNETWORK;
	gNetworkMonitor->WhenNetworkChanged << [&](Index<String>& eth, Index<String>& wifi, Bits& bits) {
		switch(NetworkType) {
		case RULE_ETHERNET:
			IsSatisfied = eth.GetCount() > 0;
			break;
		case RULE_WIFI:
			IsSatisfied = wifi.GetCount() > 0;
			break;
		}
	};
}

RuleConditionAnyNetwork::RuleConditionAnyNetwork(const RuleConditionAnyNetwork& x, int n)
	: RuleConditionAnyNetwork()
{
	IsSatisfied = x.IsSatisfied;
	Negative = x.Negative;
	NetworkType = x.NetworkType;
}

String RuleConditionAnyNetwork::Text() const
{
	String str{};
	str << "Connected to ";
	switch(NetworkType) {
	case RULE_ETHERNET:
		str << "Ethernet";
		break;
	case RULE_WIFI:
		str << "Wi-Fi";
		break;
	}

	return pick(str);
}

String RuleConditionAnyNetwork::ToString() const
{
	String str{};
	str << "{\n\t\"Type\": \"" << (int)Type << "\",\n\t\"Negative\": \"" << (int)Negative;
	str << "\"\n\t\"NetworkType\": \"" << (int)NetworkType << "\"\n}";

	return pick(str);
}

void RuleConditionAnyNetwork::Check()
{
	// todo
}

Rule::Rule(const Rule& x, int n)
{
	UUID = x.UUID;
	Name = x.Name;
	TunnelId = x.TunnelId;
	Type = x.Type;

	for(auto& cond : x.Conditions) {
		if(cond.Is<RuleConditionSSID>()) {
			Any any{};
			any.Create<RuleConditionSSID>();
			any.Get<RuleConditionSSID>() = clone(cond.Get<RuleConditionSSID>());
			Conditions.Add(pick(any));
		}
		else if(cond.Is<RuleConditionAnyNetwork>()) {
			Any any{};
			any.Create<RuleConditionAnyNetwork>();
			any.Get<RuleConditionAnyNetwork>() = clone(cond.Get<RuleConditionAnyNetwork>());
			Conditions.Add(pick(any));
		}
	}
}

bool Rule::IsSatisFied() const
{
	for(const auto& cond : Conditions) {
		if(cond.Is<RuleConditionSSID>() && !cond.Get<RuleConditionSSID>().IsSatisfied) {
			return false;
		}
		else if(cond.Is<RuleConditionAnyNetwork>() && !cond.Get<RuleConditionAnyNetwork>().IsSatisfied) {
			return false;
		}
	}
	return true;
}

String Rule::ToString() const
{
	String str{};
	str << "{\n\t\"UUID\": \"" << UUID.ToString() << "\",\n\t\"Name\": \"" << Name << "\",\n\t";
	str << "\"TunnelId\": \"" << TunnelId.ToString() << "\",\n\t\"Type\": \"" << (int)Type << "\",\n\t";
	str << "\"Conditions\": [";
	str << (Conditions.GetCount() > 0 ? "\t" : "") << "]\n}\n";
	return str;
}

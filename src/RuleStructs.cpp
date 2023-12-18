#include "NetworkMonitor.hpp"
#include "RuleManager.hpp"
#include "RuleStructs.hpp"

extern std::unique_ptr<NetworkMonitor> gNetworkMonitor;
extern std::unique_ptr<RuleManager> gRuleManager;

RuleConditionSSID::RuleConditionSSID()
	: RuleConditionBase()
{
	Type = RULE_SSID;
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

void RuleConditionSSID::Check(const Index<String>& eth, const Index<String>& wifi)
{
	IsSatisfied = Negative ^ (eth.Find(SSID) >= 0 || wifi.Find(SSID) >= 0);
}

RuleConditionAnyNetwork::RuleConditionAnyNetwork()
	: RuleConditionBase()
{
	Type = RULE_ANYNETWORK;
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

void RuleConditionAnyNetwork::Check(const Index<String>& eth, const Index<String>& wifi)
{
	switch(NetworkType) {
	case RULE_ETHERNET:
		IsSatisfied = eth.GetCount() > 0;
		break;
	case RULE_WIFI:
		IsSatisfied = wifi.GetCount() > 0;
		break;
	}
	IsSatisfied ^= Negative;
}

Rule::Rule(const Rule& x, int n)
	: Rule()
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

void Rule::Check(const Index<String>& eth, const Index<String>& wifi)
{
	for(auto& condAny : Conditions) {
		if(condAny.Is<RuleConditionSSID>()) {
			auto& cond = condAny.Get<RuleConditionSSID>();
			cond.Check(eth, wifi);
		}
		else if(condAny.Is<RuleConditionAnyNetwork>()) {
			auto& cond = condAny.Get<RuleConditionAnyNetwork>();
			cond.Check(eth, wifi);
		}
	}

	if(Conditions.GetCount() == 0) {
		isSatisfied_ = Type == RULE_ANY;
		return;
	}

	switch(Type) {
	case RULE_ALL: {
		for(const auto& cond : Conditions) {
			if(cond.Is<RuleConditionSSID>() && !cond.Get<RuleConditionSSID>().IsSatisfied) {
				isSatisfied_ = false;
				return;
			}
			else if(cond.Is<RuleConditionAnyNetwork>() && !cond.Get<RuleConditionAnyNetwork>().IsSatisfied) {
				isSatisfied_ = false;
				return;
			}
		}
		isSatisfied_ = true;
		return;
	}
	case RULE_ANY: {
		for(const auto& cond : Conditions) {
			if(cond.Is<RuleConditionSSID>() && cond.Get<RuleConditionSSID>().IsSatisfied) {
				isSatisfied_ = true;
				return;
			}
			else if(cond.Is<RuleConditionAnyNetwork>() && cond.Get<RuleConditionAnyNetwork>().IsSatisfied) {
				isSatisfied_ = true;
				return;
			}
		}
		isSatisfied_ = false;
		return;
	}
	}

	isSatisfied_ = false;
}

String Rule::ToString() const
{
	String str{};
	str << "{\n\t\"UUID\": \"" << UUID.ToString() << "\",\n\t\"Name\": \"" << Name << "\",\n\t";
	str << "\"TunnelId\": \"" << TunnelId.ToString() << "\",\n\t\"Type\": \"" << (int)Type << "\",\n\t";
	str << "\"Conditions\": [";
	for(int i = 0; i < Conditions.GetCount(); ++i) {
		auto& condAny = Conditions[i];
		if(condAny.Is<RuleConditionSSID>()) {
			auto& cond = condAny.Get<RuleConditionSSID>();
			str << "\n\t\t{\n\t\t\t\"Type\": \"" << (int)cond.Type << "\",\n\t\t\t";
			str << "\"Negative\": \"" << (int)cond.Negative << "\",\n\t\t\t";
			str << "\"SSID\": \"" << cond.SSID << "\"\n\t\t}";
		}
		else if(condAny.Is<RuleConditionAnyNetwork>()) {
			auto& cond = condAny.Get<RuleConditionAnyNetwork>();
			str << "\n\t\t{\n\t\t\t\"Type\": \"" << (int)cond.Type << "\",\n\t\t\t";
			str << "\"Negative\": \"" << (int)cond.Negative << "\",\n\t\t\t";
			str << "\"NetworkType\": \"" << (int)cond.NetworkType << "\"\n\t\t}";
		}

		if(i != Conditions.GetCount() - 1) {
			str << ",";
		}
	}
	str << (Conditions.GetCount() > 0 ? "\n\t" : String::GetVoid()) << "]\n}\n";
	return str;
}

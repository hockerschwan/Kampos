#include "Logger.hpp"
#include "NetworkMonitor.hpp"
#include "ProcessManager.hpp"
#include "RuleManager.hpp"
#include "TunnelsManager.hpp"

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<ProcessManager> gProcessManager;
extern std::unique_ptr<NetworkMonitor> gNetworkMonitor;
extern std::unique_ptr<TunnelsManager> gTunnelsManager;

RuleManager::RuleManager()
{
	LoadFile();

	gNetworkMonitor->NetworkDetected << [&](const Index<String>& eth, const Index<String>& wifi) {
		ethernet_ = clone(eth);
		wifi_ = clone(wifi);
		CheckConditions(ethernet_, wifi_);
	};

	gTunnelsManager->WhenListChanged << [&] { CheckTunnelId(); };
}

bool RuleManager::GetRule(const Id& uuid, Rule& out) const
{
	auto i = rules_.Find(uuid);
	if(i < 0) {
		return false;
	}

	out = pick(clone(rules_.Get(uuid)));
	return true;
}

Id RuleManager::Add()
{
	auto id = Id(ToLower(Uuid::Create().ToStringWithDashes()));
	Rule rule{};
	rule.UUID = id;
	rule.Name = "New Rule";

	rules_.Add(id, pick(rule));
	if(!Save()) {
		rules_.Remove(rules_.GetCount() - 1);
		return Id();
	}

	return id;
}

bool RuleManager::Remove(const Id& uuid)
{
	auto i = rules_.Find(uuid);
	if(i < 0) {
		return false;
	}

	rules_.Remove(i);

	return Save();
}

void RuleManager::Move(const Id& uuid, int i)
{
	if(i < 0) {
		return;
	}

	int iOld = rules_.Find(uuid);
	if(iOld < 0) {
		return;
	}

	if(i > iOld) {
		--i;
	}

	Rule rule{};
	GetRule(uuid, rule);

	rules_.Remove(iOld);
	rules_.Insert(i, uuid, pick(rule));

	Save();
}

bool RuleManager::Save()
{
	String str{};

	str << "{\n";
	str << "\t\"Rules\": [";
	if(rules_.GetCount() > 0) {
		str << "\n";
	}

	for(int i = 0; i < rules_.GetCount(); ++i) {
		auto lines = Split(rules_[i].ToString(), "\n");
		for(int j = 0; j < lines.GetCount(); ++j) {
			str << "\t\t" << lines[j];
			if(j != lines.GetCount() - 1) {
				str << "\n";
			}
		}

		if(i != rules_.GetCount() - 1) {
			str << ",";
		}
		str << "\n";
	}
	str << (rules_.GetCount() > 0 ? "\t" : "") << "]\n}\n";
	auto res = SaveFile(path_, str);
	CheckConditions();
	return res;
}

bool RuleManager::Save(const Rule& rule)
{
	auto& r = rules_.Get(rule.UUID);
	r = clone(rule);
	return Save();
}

void RuleManager::LoadFile()
{
	if(!FileExists(path_)) {
		Save();
		Sleep(100);
	}

	auto str = Replace(Upp::LoadFile(path_), {{"\r", ""}, {"\n", ""}, {"\t", ""}});
	auto js = ParseJSON(str);

	rules_.Clear();
	for(const auto& r : js("Rules")) {
		Rule rule{};
		rule.UUID = Id(r["UUID"]);
		rule.Name = r["Name"];
		rule.TunnelId = Id(r["TunnelId"]);
		rule.Type = (RuleType)(StrInt(r["Type"].ToString()));

		for(auto& c : r["Conditions"]) {
			switch(StrInt(c["Type"].ToString())) {
			case RULE_SSID: {
				Any cond{};
				cond.Create<RuleConditionSSID>();
				cond.Get<RuleConditionSSID>().Negative = (bool)StrInt(c["Negative"].ToString());
				cond.Get<RuleConditionSSID>().SSID = c["SSID"].ToString();

				rule.Conditions.Add(pick(cond));
				break;
			}
			case RULE_ANYNETWORK: {
				Any cond{};
				cond.Create<RuleConditionAnyNetwork>();
				cond.Get<RuleConditionAnyNetwork>().Negative = (bool)StrInt(c["Negative"].ToString());
				cond.Get<RuleConditionAnyNetwork>().NetworkType = (RuleNetworkType)StrInt(c["NetworkType"].ToString());

				rule.Conditions.Add(pick(cond));
				break;
			}
			}
		}

		rules_.Add(rule.UUID, pick(rule));
	}
}

void RuleManager::CheckTunnelId()
{
	bool save = false;
	for(const auto& item : ~(rules_)) {
		auto& rule = item.value;
		TunnelConfig _{};
		if(!gTunnelsManager->GetConfig(rule.TunnelId, _)) {
			rule.TunnelId = Id();
			save = true;
		}
	}

	if(save) {
		Save();
	}
}

void RuleManager::CheckConditions(const Index<String>& eth, const Index<String>& wifi)
{
	Id uuid{}, tunnelId{};
	for(const auto& item : ~(rules_)) {
		auto& rule = item.value;
		rule.Check(eth, wifi);
		if(uuid.IsNull() && rule.IsSatisfied()) {
			uuid = rule.UUID;
			tunnelId = rule.TunnelId;
		}
	}

	currentRule_ = uuid;

	if(uuid.IsNull()) {
		gProcessManager->Stop();
		WhenRuleChanged(uuid);
		return;
	}

	gProcessManager->Start(tunnelId, false);
	WhenRuleChanged(uuid);
}

#ifndef _RuleManager_hpp_
#define _RuleManager_hpp_

#include "Helper.hpp"
#include "RuleStructs.hpp"
#include <Core/Core.h>

using namespace Upp;

struct RuleManager {
public:
	RuleManager();

	const ArrayMap<Id, Rule> GetRules() const { return pick(clone(rules_)); };
	bool GetRule(const Id& uuid, Rule& out) const;
	const Id GetCurrentId() const { return pick(clone(currentRule_)); };

	const Index<String> GetEthernet() const { return pick(clone(ethernet_)); };
	const Index<String> GetWifi() const { return pick(clone(wifi_)); };

	Id Add();
	bool Remove(const Id& uuid);
	void Move(const Id& uuid, int i);

	void LoadFile();
	bool Save();
	bool Save(const Rule& rule);

	void CheckConditions() { CheckConditions(ethernet_, wifi_); };

	Event<Id> WhenRuleChanged;

private:
	void CheckTunnelId();
	void CheckConditions(const Index<String>& eth, const Index<String>& wifi);

	const String path_ = Helper::RoamingPath() << "rules.json";

	ArrayMap<Id, Rule> rules_{};
	Id currentRule_{};

	Index<String> ethernet_{};
	Index<String> wifi_{};
};

#endif

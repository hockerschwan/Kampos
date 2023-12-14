#ifndef _RuleManager_hpp_
#define _RuleManager_hpp_

#include "Helper.hpp"
#include <Core/Core.h>

using namespace Upp;

struct RuleCondition : Moveable<RuleCondition> {
public:
	bool IsSatisfied = false;
};

struct RuleConditionSSID : public RuleCondition {};

enum RuleType { RULE_ALL, RULE_ANY };

struct Rule : MoveableAndDeepCopyOption<Rule> {
public:
	Id UUID{};
	String Name{};
	Id TunnelId{};
	RuleType Type{};
	Array<RuleCondition> Conditions{};

	Rule(){};
	Rule(const Rule& x, int n)
	{
		UUID = x.UUID;
		Name = x.Name;
		TunnelId = x.TunnelId;
		Type = x.Type;
		Conditions = clone(x.Conditions);
	};

	bool IsSatisFied()
	{
		for(const auto& cond : Conditions) {
			if(!cond.IsSatisfied) {
				return false;
			}
		}
		return true;
	};

	String ToString() const
	{
		String str{};
		str << "{\n\t\"UUID\": \"" << UUID.ToString() << "\",\n\t\"Name\": \"" << Name << "\",\n\t";
		str << "\"TunnelId\": \"" << TunnelId.ToString() << "\",\n\t\"Type\": \"" << (int)Type << "\",\n\t";
		str << "\"Conditions\": [";
		str << (Conditions.GetCount() > 0 ? "\t" : "") << "]\n}\n";
		return str;
	};
};

struct RuleManager {
public:
	RuleManager() { LoadFile(); };

	Id Add();
	bool Remove(const Id& uuid);

	const ArrayMap<Id, Rule> GetRules() const { return pick(clone(rules_)); };
	bool GetRule(const Id& uuid, Rule& out) const;

	bool Save();

private:
	void LoadFile();

	const String path_ = Helper::RoamingPath() << "rules.json";

	ArrayMap<Id, Rule> rules_{};
	Id defaultRule_{};
};

#endif

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

	Id Add();
	bool Remove(const Id& uuid);
	void Move(const Id& uuid, int i);

	bool Save();
	bool Save(const Rule& rule);

private:
	void LoadFile();
	void CheckTunnelId();

	const String path_ = Helper::RoamingPath() << "rules.json";

	ArrayMap<Id, Rule> rules_{};
	Id currentRule_{};
};

#endif

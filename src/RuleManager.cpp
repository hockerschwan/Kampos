#include "RuleManager.hpp"

bool RuleManager::GetRule(const Id& uuid, Rule& out) const
{
	auto i = rules_.Find(uuid);
	if(i < 0) {
		return false;
	}

	out = pick(clone(rules_.Get(uuid)));
	return true;
}

bool RuleManager::Save()
{
	String str{};

	str << "{\n";
	str << "\t\"Default\": \"" << defaultRule_.ToString() << "\",\n";
	str << "\t\"Rules\": [";
	for(const auto& r : rules_) {
		for(const auto& line : Split(r.ToString(), "\n")) {
			str << "\t\t" << line << "\n";
		}
	}
	str << (rules_.GetCount() > 0 ? "\t" : "") << "]\n}\n";
	return SaveFile(path_, str);
}

void RuleManager::LoadFile()
{
	if(!FileExists(path_)) {
		Save();
		Sleep(100);
	}

	auto str = Replace(Upp::LoadFile(path_), {{"\r", ""}, {"\n", ""}, {"\t", ""}});
	auto js = ParseJSON(str);

	defaultRule_ = Id(js("Default").ToString());

	rules_.Clear();
	for(const auto& r : js("Rules")) {
		Rule rule{};
		rule.UUID = Id(r["UUID"]);
		rule.Name = r["Name"];
		rule.TunnelId = Id(r["TunnelId"]);
		rule.Type = (RuleType)((int)r["Type"]);
		// conditions
		rules_.Add(rule.UUID, pick(rule));
	}
}

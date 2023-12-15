#include "RuleManager.hpp"
#include "TunnelsManager.hpp"

extern std::unique_ptr<TunnelsManager> gTunnelsManager;

RuleManager::RuleManager()
{
	LoadFile();

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
	return SaveFile(path_, str);
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
		// todo: conditions
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

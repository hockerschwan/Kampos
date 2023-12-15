#include "RuleEditor.hpp"
#include "TunnelsManager.hpp"

extern std::unique_ptr<TunnelsManager> gTunnelsManager;
extern std::unique_ptr<RuleManager> gRuleManager;

RuleEditor::RuleEditor()
{
	CtrlLayout(*this);

	editName_.WhenAction = [&] { WhenAction(); };
	dropTunnel_.WhenAction = [&] { WhenAction(); };

	swType_.Add(RULE_ALL, "All", 2).Add(RULE_ANY, "Any", 2);
	swType_.WhenAction = [&] { WhenAction(); };

	gTunnelsManager->WhenListChanged << [&] { RefreshTunnels(); };
}

RuleEditor& RuleEditor::SetId(const Id& uuid)
{
	Rule rule{};
	if(!gRuleManager->GetRule(uuid, rule)) {
		uuid_ = Id();
		return *this;
	}

	uuid_ = uuid;

	GuiLock __;

	editName_.SetText(rule.Name);

	RefreshTunnels();

	swType_.SetData(rule.Type);

	return *this;
}

Rule RuleEditor::Get() const
{
	Rule rule{};
	rule.UUID = uuid_;
	rule.Name = editName_.GetText().ToString();
	rule.TunnelId = dropTunnel_.GetIndex() >= 0 ? Id(dropTunnel_.GetKey(dropTunnel_.GetIndex())) : Id();
	rule.Type = (RuleType)((int)swType_);
	// todo: conditions

	return pick(rule);
}

void RuleEditor::RefreshTunnels()
{
	GuiLock __;

	dropTunnel_.Clear();
	auto tunnels = gTunnelsManager->GetTunnels();
	for(const auto& item : ~(tunnels)) {
		auto id = item.key;
		auto& tunnel = item.value;
		dropTunnel_.Add(id.ToString(), tunnel.Interface.Name);
	}

	Rule rule{};
	if(!gRuleManager->GetRule(uuid_, rule)) {
		return;
	}

	auto pos = tunnels.Find(rule.TunnelId);
	if(pos >= 0) {
		dropTunnel_.SetIndex(pos);
	}
}

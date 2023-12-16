#include "Logger.hpp"
#include "RuleEditor.hpp"
#include "TunnelsManager.hpp"

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<TunnelsManager> gTunnelsManager;
extern std::unique_ptr<RuleManager> gRuleManager;

RuleEditor::RuleEditor()
{
	CtrlLayout(*this);

	editName_.WhenAction = [&] { WhenAction(); };
	dropTunnel_.WhenAction = [&] { WhenAction(); };

	swType_.Add(RULE_ALL, "All", 2).Add(RULE_ANY, "Any", 2);
	swType_.WhenAction = [&] { WhenAction(); };

	scroll_.scroll.y.AutoHide(false);
	scroll_.scroll.x.AutoHide(false).AutoDisable(false).Hide();

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

	for(auto& any : editors_) {
		if(any.Is<RuleConditionEditorSSID>()) {
			auto& e = any.Get<RuleConditionEditorSSID>();
			conditions_.RemoveChild((Ctrl*)&e);
		}
		else if(any.Is<RuleConditionEditorAnyNetwork>()) {
			auto& e = any.Get<RuleConditionEditorAnyNetwork>();
			conditions_.RemoveChild((Ctrl*)&e);
		}
	}
	editors_.Clear();

	auto h = 0;
	for(const auto& cond : rule.Conditions) {
		if(cond.Is<RuleConditionSSID>()) {
			Any any{};
			auto& editor = any.Create<RuleConditionEditorSSID>();
			conditions_.Add(editor.HSizePosZ().TopPos(h));
			h += editor.GetSize().cy;

			editors_.Add(pick(any));
		}
		else if(cond.Is<RuleConditionAnyNetwork>()) {
			Any any{};
			auto& editor = any.Create<RuleConditionEditorAnyNetwork>();
			conditions_.Add(editor.HSizePosZ().TopPos(h));
			h += editor.GetSize().cy;

			editors_.Add(pick(any));
		}
	}

	scroll_.RemoveChild(&conditions_);
	scroll_.Add(conditions_.HSizePosZ().TopPos(0, h));
	scroll_.AddPane(conditions_);

	return *this;
}

Rule RuleEditor::Get() const
{
	Rule rule{};
	rule.UUID = uuid_;
	rule.Name = editName_.GetText().ToString();
	rule.TunnelId = dropTunnel_.GetIndex() >= 0 ? Id(dropTunnel_.GetKey(dropTunnel_.GetIndex())) : Id();
	rule.Type = (RuleType)((int)swType_);

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

void RuleEditor::ShowConditionsMenu(Bar& bar)
{
	int iconSize = Zx(15); // 16 @100%

	bar.Add("Add SSID", Rescale(AppIcons::Tunnels(), iconSize, iconSize), [] {});
	bar.Add("Add \"Any Ethernet/WiFi\"", Rescale(AppIcons::Tunnels(), iconSize, iconSize), [] {});
}

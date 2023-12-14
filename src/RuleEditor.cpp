#include "RuleEditor.hpp"
#include "RuleManager.hpp"
#include "TunnelsManager.hpp"

extern std::unique_ptr<TunnelsManager> gTunnelsManager;
extern std::unique_ptr<RuleManager> gRuleManager;

RuleEditor::RuleEditor()
{
	CtrlLayout(*this);

	left_.Add(editName_.HSizePosZ(0, 8).TopPosZ(0, 20));

	left_.Add(dropTunnel_.HSizePosZ(0, 8).TopPosZ(28, 20));

	swType_.SetLabel(RULE_ALL, "All").SetLabel(RULE_ANY, "Any", 4);
	left_.Add(swType_.HSizePosZ().TopPosZ(52, 20));

	left_.Add(textUUID_.HSizePosZ().BottomPosZ(0, 20));

	grid_.SetColumns(2);
	grid_.Add(left_);
	grid_.Add(array_);

	array_.Header(false);
}

RuleEditor& RuleEditor::SetId(const Id& uuid)
{
	Rule rule{};
	if(gRuleManager->GetRule(uuid, rule)) {
		uuid_ = uuid;

		GuiLock __;

		editName_.SetText(rule.Name);

		auto tunnels = gTunnelsManager->GetTunnels();
		for(const auto& item : ~(tunnels)) {
			auto id = item.key;
			auto& tunnel = item.value;
			dropTunnel_.Add(id.ToString(), tunnel.Interface.Name);
		}
		auto pos = tunnels.Find(rule.TunnelId);
		if(pos >= 0) {
			dropTunnel_.SetIndex(pos);
		}

		swType_.SetData(rule.Type);

		textUUID_.SetText(uuid.ToString());
	}

	return *this;
}

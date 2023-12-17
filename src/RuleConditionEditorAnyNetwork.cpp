#include "RuleConditionEditorAnyNetwork.hpp"

RuleConditionEditorAnyNetwork::RuleConditionEditorAnyNetwork()
{
	CtrlLayout(*this);

	sw_.Add(0, "Ethernet").Add(1, "Wi-Fi", 2);
	sw_.WhenAction = [&] { Save(); };

	opt_.WhenAction = [&] { Save(); };

	btn_.SetImage(Rescale(AppIcons::Bin(), Zx(15), Zx(15))).Tip("Delete");
	btn_.WhenAction = [&] {
		if(PromptYesNo("Are you sure you want to delete?") == 1) {
			WhenDelete();
		}
	};
}

RuleConditionAnyNetwork RuleConditionEditorAnyNetwork::Get() const
{
	RuleConditionAnyNetwork cond{};
	cond.Negative = (bool)opt_.Get();
	cond.NetworkType = (RuleNetworkType)(int)sw_;

	return pick(cond);
}

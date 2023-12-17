#include "RuleConditionEditorSSID.hpp"

RuleConditionEditorSSID::RuleConditionEditorSSID()
{
	CtrlLayout(*this);

	edit_.WhenAction = [&] { Save(); };

	opt_.WhenAction = [&] { Save(); };

	btn_.SetImage(Rescale(AppIcons::Bin(), Zx(15), Zx(15))).Tip("Delete");
	btn_.WhenAction = [&] {
		if(PromptYesNo("Are you sure you want to delete?") == 1) {
			WhenDelete();
		}
	};
}

RuleConditionSSID RuleConditionEditorSSID::Get() const
{
	RuleConditionSSID cond{};
	cond.Negative = (bool)opt_.Get();
	cond.SSID = edit_.GetText().ToString();

	return pick(cond);
}

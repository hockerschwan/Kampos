#ifndef _RuleEditor_hpp_
#define _RuleEditor_hpp_

#include "RuleConditionEditorAnyNetwork.hpp"
#include "RuleConditionEditorSSID.hpp"
#include "RuleManager.hpp"
#include "common.hpp"

struct RuleEditor : WithRuleLayout<ParentCtrl> {
public:
	RuleEditor();

	RuleEditor& SetId(const Id& uuid);

	Rule Get() const;

private:
	void RefreshTunnels();

	void ShowConditionsMenu(Bar& bar);

	Id uuid_{};
	ParentCtrl conditions_{};
	Array<Any> editors_{};
};

#endif

#ifndef _RuleConditionEditorAnyNetwork_hpp_
#define _RuleConditionEditorAnyNetwork_hpp_

#include "RuleStructs.hpp"
#include "common.hpp"

struct RuleConditionEditorAnyNetwork : public WithRuleConditionAnyNetworkLayout<ParentCtrl>,
									   Moveable<RuleConditionEditorAnyNetwork> {
public:
	RuleConditionEditorAnyNetwork();

	RuleConditionAnyNetwork Get() const;

	void ToggleImage(bool show) { img_.Show(show); };

	Event<> WhenDelete;

private:
	void Save() { WhenAction(); };
};

#endif

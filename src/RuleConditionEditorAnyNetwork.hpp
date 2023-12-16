#ifndef _RuleConditionEditorAnyNetwork_hpp_
#define _RuleConditionEditorAnyNetwork_hpp_

#include "common.hpp"

struct RuleConditionEditorAnyNetwork : public WithRuleConditionAnyNetworkLayout<ParentCtrl>,
									   Moveable<RuleConditionEditorAnyNetwork> {
public:
	RuleConditionEditorAnyNetwork() { CtrlLayout(*this); };
};

#endif

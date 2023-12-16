#ifndef _RuleConditionEditorSSID_hpp_
#define _RuleConditionEditorSSID_hpp_

#include "common.hpp"

struct RuleConditionEditorSSID : public WithRuleConditionSSIDLayout<ParentCtrl>, Moveable<RuleConditionEditorSSID> {
public:
	RuleConditionEditorSSID() { CtrlLayout(*this); };
};

#endif

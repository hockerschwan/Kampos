#ifndef _RuleConditionEditorSSID_hpp_
#define _RuleConditionEditorSSID_hpp_

#include "RuleStructs.hpp"
#include "common.hpp"

struct RuleConditionEditorSSID : public WithRuleConditionSSIDLayout<ParentCtrl>, Moveable<RuleConditionEditorSSID> {
public:
	RuleConditionEditorSSID();

	RuleConditionSSID Get() const;

	Event<> WhenDelete;

private:
	void Save() { WhenAction(); };
};

#endif

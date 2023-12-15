#ifndef _RuleEditor_hpp_
#define _RuleEditor_hpp_

#include "RuleManager.hpp"
#include "common.hpp"

struct RuleEditor : WithRuleLayout<ParentCtrl> {
public:
	RuleEditor();

	RuleEditor& SetId(const Id& uuid);

	Rule Get() const;

private:
	void RefreshTunnels();

	Id uuid_{};
};

#endif

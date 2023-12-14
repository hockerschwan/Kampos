#ifndef _RuleEditor_hpp_
#define _RuleEditor_hpp_

#include "common.hpp"

struct RuleEditor : WithRuleLayout<ParentCtrl> {
public:
	RuleEditor();

	RuleEditor& SetId(const Id& uuid);

private:
	Id uuid_{};

	ParentCtrl left_{};
	EditField editName_{};
	DropList dropTunnel_{};
	Switch swType_;
	StaticText textUUID_{};
	ArrayCtrl array_{};
};

#endif

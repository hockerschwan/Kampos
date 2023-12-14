#include "Logger.hpp"
#include "RuleEditor.hpp"
#include "RuleManager.hpp"
#include "RulesPage.hpp"

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<RuleManager> gRuleManager;

RulesPage::RulesPage()
{
	CtrlLayout(*this);

	{
		auto rules = gRuleManager->GetRules();
		array_.AddColumn("Col1").WithLined([&rules](int line, One<Ctrl>& x) {
			auto id = rules.GetKey(line);
			gLogger->Log(id.ToString());
			x.Create<RuleEditor>().SetId(id).SizePos();
		});

		for(int i = 0; i < rules.GetCount(); ++i) {
			array_.Add();
		}

		array_.SetLineCy(Zy(130));
		array_.RefreshLayoutDeep();
	}
}

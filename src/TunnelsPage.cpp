#include "TunnelsManager.hpp"
#include "TunnelsPage.hpp"

extern std::unique_ptr<TunnelsManager> gTunnelsManager;

TunnelsPage::TunnelsPage()
{
	CtrlLayout(*this);

	clTunnels_.RowMode().NoRoundSize().AutoHideSb(false).ItemHeight(28).ItemWidth(153);
	auto tunnels = gTunnelsManager->GetTunnels();
	for(const auto& item : ~(tunnels)) {
		auto& id = item.key;
		auto name = gTunnelsManager->GetName(id);
		clTunnels_.Add(id.ToString(), name, true);
	}
}

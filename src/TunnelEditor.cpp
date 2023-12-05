#include "Logger.hpp"
#include "TunnelEditor.hpp"
#include "TunnelsManager.hpp"

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<TunnelsManager> gTunnelsManager;

TunnelEditor::TunnelEditor()
	: ifc_(MakeOne<TunnelInterfaceEditor>())
{
	this->Add(ifc_->HSizePosZ().TopPosZ(0));
}

void TunnelEditor::SetId(const Id& uuid)
{
	uuid_ = uuid;

	peers_.Clear();

	auto tunnels = gTunnelsManager->GetTunnels();
	for(const auto& item : ~(tunnels)) {
		auto& id = item.key;
		if(id == uuid) {
			auto& cfg = item.value;

			ifc_->Set(cfg.Interface);

			for(auto& peer : cfg.Peers) {
				peers_.Add().Set(peer);
			}
			break;
		}
	}

	auto h = ifc_->GetSize().cy;
	for(auto& p : peers_) {
		this->Add(p.HSizePosZ().TopPos(h));
		h += p.GetStdSize().cy;
	}
	this->HSizePosZ().TopPos(0, h);
}

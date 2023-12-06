#include "Logger.hpp"
#include "TunnelEditor.hpp"
#include "TunnelsManager.hpp"
#include "TunnelsPage.hpp"

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<TunnelsManager> gTunnelsManager;

TunnelEditor::TunnelEditor()
	: ifc_(MakeOne<TunnelInterfaceEditor>())
{
	ifc_->SetParent(this);
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

			int i = 0;
			for(auto& peer : cfg.Peers) {
				peers_.Add().SetParent(this).Set(peer, i);
				++i;
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

void TunnelEditor::AddPeer()
{
	auto tunnels = gTunnelsManager->GetTunnels();
	for(const auto& item : ~(tunnels)) {
		auto& id = item.key;
		if(id == uuid_) {
			auto cfg = clone(item.value);
			cfg.Peers.Add(TunnelPeer::GetDefault());

			gTunnelsManager->Save(uuid_, cfg, true);

			while(timerRunning_ > 0) {
				AtomicDec(timerRunning_);
			}

			parent_->SetContent(uuid_);

			break;
		}
	}
}

void TunnelEditor::DeletePeer(int i)
{
	if(i >= peers_.GetCount()) {
		return;
	}

	auto tunnels = gTunnelsManager->GetTunnels();
	for(const auto& item : ~(tunnels)) {
		auto& id = item.key;
		if(id == uuid_) {
			auto cfg = clone(item.value);
			cfg.Peers.Remove(i);

			gTunnelsManager->Save(uuid_, cfg, true);

			while(timerRunning_ > 0) {
				AtomicDec(timerRunning_);
			}

			parent_->SetContent(uuid_);

			break;
		}
	}
}

void TunnelEditor::Save(bool immediate)
{
	if(timerRunning_ > 0) {
		return;
	}

	AtomicInc(timerRunning_);
	Thread t{};
	t.Run([&] {
		auto id = clone(uuid_);
		auto& cfg = GetConfig();

		if(!immediate) {
			t.Sleep(200);
		}

		gTunnelsManager->Save(id, cfg, true);

		if(timerRunning_ > 0) {
			AtomicDec(timerRunning_);
		}
	});
}

const TunnelConfig TunnelEditor::GetConfig() const
{
	TunnelConfig cfg{};

	cfg.Interface = clone(ifc_->Get());
	for(const auto& pe : peers_) {
		cfg.Peers.Add(clone(pe.Get()));
	}

	return pick(cfg);
}

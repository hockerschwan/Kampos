#include "Logger.hpp"
#include "TunnelEditor.hpp"
#include "TunnelsManager.hpp"
#include "TunnelsPage.hpp"

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<TunnelsManager> gTunnelsManager;

TunnelEditor::TunnelEditor()
	: ifc_(MakeOne<TunnelInterfaceEditor>())
{
	ifc_->WhenAction = [&] { Save(); };
	this->Add(ifc_->HSizePosZ().TopPosZ(0));
}

void TunnelEditor::SetId(const Id& uuid)
{
	if(!uuid_.IsNull() && timerRunning_ > 0) {
		Save(true);
	}

	uuid_ = uuid;
	peers_.Clear();

	TunnelConfig cfg{};
	if(!gTunnelsManager->GetConfig(uuid_, cfg)) {
		return;
	}

	ifc_->Set(cfg.Interface);

	int i = 0;
	for(auto& peer : cfg.Peers) {
		auto& pe = peers_.Add();
		pe.WhenAction = [&] { Save(); };
		pe.WhenDelete = [&](int j) { DeletePeer(j); };
		pe.Set(peer, i);
		++i;
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
	TunnelConfig cfg{};
	if(!gTunnelsManager->GetConfig(uuid_, cfg)) {
		return;
	}

	cfg.Peers.Add(TunnelPeer::GetDefault());

	gTunnelsManager->Save(uuid_, cfg, true);
	ResetTimer();
	Refresh();
}

void TunnelEditor::DeletePeer(int i)
{
	if(i >= peers_.GetCount()) {
		return;
	}

	TunnelConfig cfg{};
	if(!gTunnelsManager->GetConfig(uuid_, cfg)) {
		return;
	}

	cfg.Peers.Remove(i);

	gTunnelsManager->Save(uuid_, cfg, true);
	ResetTimer();
	Refresh();
}

void TunnelEditor::Save(bool immediate)
{
	if(!immediate && timerRunning_ > 0) {
		return;
	}

	AtomicInc(timerRunning_);
	Thread t{};
	t.Run([&] {
		if(!immediate) {
			t.Sleep(1000);
		}

		if(timerRunning_ == 0) {
			return;
		}

		auto id = clone(uuid_);
		auto& cfg = GetConfig();

		gTunnelsManager->Save(id, cfg, true);
		ResetTimer();
	});

	if(immediate) {
		t.Wait();
	}
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

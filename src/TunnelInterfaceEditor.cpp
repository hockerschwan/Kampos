#include "Logger.hpp"
#include "TunnelEditor.hpp"
#include "TunnelInterfaceEditor.hpp"
#include "TunnelStructs.hpp"

extern std::unique_ptr<Logger> gLogger;

TunnelInterfaceEditor::TunnelInterfaceEditor()
	: addresses_(MakeOne<TunnelAddressEditor>())
	, dns_(MakeOne<TunnelAddressEditor>())
	, preup_(MakeOne<TunnelCommandEditor>())
	, postup_(MakeOne<TunnelCommandEditor>())
	, predown_(MakeOne<TunnelCommandEditor>())
	, postdown_(MakeOne<TunnelCommandEditor>())
{
	CtrlLayout(*this);

	addresses_->SetText("Address");
	dns_->SetText("DNS");
	preup_->SetText("PreUp");
	postup_->SetText("PostUp");
	predown_->SetText("PreDown");
	postdown_->SetText("PostDown");

	grid_.SetColumns(2);
	grid_.Add(*addresses_);
	grid_.Add(*dns_);

	grid2_.SetColumns(2);
	grid2_.Add(*preup_);
	grid2_.Add(*postup_);

	grid3_.SetColumns(2);
	grid3_.Add(*predown_);
	grid3_.Add(*postdown_);

	textName_.Hide();

	spinMTU_.WhenAction = [&] {
		if(StrIntValue(spinMTU_.GetText().ToString()).IsNull()) {
			spinMTU_.SetText("0");
		}
		Save();
	};
	editPrivateKey_.WhenAction = [&] { Save(); };
	addresses_->WhenArrayAction = [&] { Save(); };
	dns_->WhenArrayAction = [&] { Save(); };
	preup_->WhenArrayAction = [&] { Save(); };
	postup_->WhenArrayAction = [&] { Save(); };
	predown_->WhenArrayAction = [&] { Save(); };
	postdown_->WhenArrayAction = [&] { Save(); };
}

const TunnelInterface TunnelInterfaceEditor::Get() const
{
	TunnelInterface ifc{};

	ifc.Name = textName_.GetText();
	ifc.UUID = clone(uuid_);
	ifc.Address = clone(addresses_->Get());
	ifc.DNS = clone(dns_->Get());
	ifc.MTU = StrIntValue(spinMTU_.GetText().ToString());
	ifc.PrivateKey = editPrivateKey_.GetText().ToString();
	ifc.PreUp = clone(preup_->Get());
	ifc.PostUp = clone(postup_->Get());
	ifc.PreDown = clone(predown_->Get());
	ifc.PostDown = clone(postdown_->Get());

	return pick(ifc);
}

void TunnelInterfaceEditor::Set(TunnelInterface& ifc)
{
	textName_.SetText(ifc.Name);
	uuid_ = clone(ifc.UUID);
	spinMTU_.SetText(ifc.MTU.ToString());
	editPrivateKey_.SetText(ifc.PrivateKey);

	addresses_->Clear();
	for(const auto& addr : ifc.Address) {
		addresses_->Add(addr);
	}

	dns_->Clear();
	for(const auto& addr : ifc.DNS) {
		dns_->Add(addr);
	}

	preup_->Clear();
	for(const auto& cmd : ifc.PreUp) {
		preup_->Add(cmd);
	}

	postup_->Clear();
	for(const auto& cmd : ifc.PostUp) {
		postup_->Add(cmd);
	}

	predown_->Clear();
	for(const auto& cmd : ifc.PreDown) {
		predown_->Add(cmd);
	}

	postdown_->Clear();
	for(const auto& cmd : ifc.PostDown) {
		postdown_->Add(cmd);
	}
}

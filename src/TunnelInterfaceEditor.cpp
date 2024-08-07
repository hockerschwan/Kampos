#include "Logger.hpp"
#include "TunnelEditor.hpp"
#include "TunnelInterfaceEditor.hpp"
#include "TunnelStructs.hpp"

extern std::unique_ptr<Logger> gLogger;

TunnelInterfaceEditor::TunnelInterfaceEditor()
	: addresses_(MakeOne<TunnelAddressEditor>())
	, dns_(MakeOne<TunnelAddressEditor>())
{
	CtrlLayout(*this);

	addresses_->SetText("Address");
	dns_->SetText("DNS");

	grid_.SetColumns(2);
	grid_.Add(*addresses_);
	grid_.Add(*dns_);

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
	editPreUp_.WhenAction = [&] { Save(); };
	editPostUp_.WhenAction = [&] { Save(); };
	editPreDown_.WhenAction = [&] { Save(); };
	editPostDown_.WhenAction = [&] { Save(); };
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
	ifc.PreUp = editPreUp_.GetText().ToString();
	ifc.PostUp = editPostUp_.GetText().ToString();
	ifc.PreDown = editPreDown_.GetText().ToString();
	ifc.PostDown = editPostDown_.GetText().ToString();

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

	editPreUp_.SetText(ifc.PreUp);
	editPostUp_.SetText(ifc.PostUp);
	editPreDown_.SetText(ifc.PreDown);
	editPostDown_.SetText(ifc.PostDown);
}

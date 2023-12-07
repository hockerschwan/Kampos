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
}

const TunnelInterface TunnelInterfaceEditor::Get() const
{
	TunnelInterface ifc{};

	ifc.Name = textName_.GetText();
	ifc.UUID = Id(textUUID_.GetText());
	ifc.Address = clone(addresses_->Get());
	ifc.DNS = clone(dns_->Get());
	ifc.MTU = StrIntValue(spinMTU_.GetText().ToString());
	ifc.PrivateKey = editPrivateKey_.GetText().ToString();

	return pick(ifc);
}

void TunnelInterfaceEditor::Set(TunnelInterface& ifc)
{
	textName_.SetText(ifc.Name);
	textUUID_.SetText(ifc.UUID.ToString());
	spinMTU_.SetText(ifc.MTU.ToString());
	editPrivateKey_.SetText(ifc.PrivateKey);

	addresses_->Clear();
	for(auto& addr : ifc.Address) {
		addresses_->Add(addr);
	}

	dns_->Clear();
	for(auto& addr : ifc.DNS) {
		dns_->Add(addr);
	}
}

#include "TunnelInterfaceEditor.hpp"
#include "TunnelStructs.hpp"

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
}

void TunnelInterfaceEditor::Set(TunnelInterface& ifc)
{
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

#include "Logger.hpp"
#include "TunnelAddressEditor.hpp"

extern std::unique_ptr<Logger> gLogger;

TunnelAddressEditor::TunnelAddressEditor()
{
	CtrlLayout(*this);
	Clear();
	array_.AddColumn("Address").Edit(edit_);
	array_.WhenAcceptEdit = [&] {
		auto n1 = Addresses_.GetCount();
		auto n2 = array_.GetCount();

		// remove empty
		for(int i = n2 - 1; i >= 0; --i) {
			if(array_.Get(i, 0).ToString().IsEmpty()) {
				array_.Remove(i);
			}
		}

		if(n1 < n2) { // added
			for(int i = 0; i < n2; ++i) {
				auto m = array_.Get(i, 0).ToString();
				if(Addresses_.Find(m) < 0) {
					Addresses_.Add(m);
					array_.Sort();
					ArrayAction();
					break;
				}
			}
		}
		else if(n1 == n2) { // modified
			Addresses_.Clear();
			for(int i = 0; i < n2; ++i) {
				Addresses_.Add(array_.Get(i, 0));
			}
			array_.Sort();
			ArrayAction();
		}
	};
	array_.WhenArrayAction = [&] {
		if(Addresses_.GetCount() > array_.GetCount()) { // removed
			for(auto& addr : Addresses_) {
				if(array_.Find(addr, 0) < 0) {
					Addresses_.RemoveKey(addr);
					ArrayAction();
					break;
				}
			}
		}
	};
}

const String TunnelAddressEditor::ToString() const
{
	String str{};
	for(int i = 0; i < Addresses_.GetCount(); ++i) {
		str << Addresses_[i];
		if(i != Addresses_.GetCount() - 1) {
			str << ", ";
		}
	}
	return pick(str);
}

void TunnelAddressEditor::Add(const String& address)
{
	Addresses_.Add(address);
	array_.Add(address);
	array_.Sort();
}

void TunnelAddressEditor::Clear()
{
	Addresses_.Clear();
	array_.Clear();
}

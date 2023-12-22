#include "Logger.hpp"
#include "TunnelAddressEditor.hpp"

extern std::unique_ptr<Logger> gLogger;

const char* AddressDnDName = "AddressDragAndDrop";

TunnelAddressEditor::TunnelAddressEditor()
{
	CtrlLayout(*this);
	Clear();
	array_.AddColumn("Address").Edit(edit_);
	array_.WhenAcceptEdit = [&] { AcceptEdit(); };
	array_.WhenArrayAction = [&] {
		Addresses_.Clear();
		for(int i = 0; i < array_.GetCount(); ++i) {
			Addresses_.Add(array_.Get(i, 0).ToString());
		}
		WhenArrayAction();
	};
}

void TunnelAddressEditor::Add(const String& address)
{
	Addresses_.Add(address);
	array_.Add(address);
}

void TunnelAddressEditor::Clear()
{
	Addresses_.Clear();
	array_.Clear();
}

void TunnelAddressEditor::AcceptEdit()
{
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
			if(Addresses_.GetIndex(m) < 0) {
				Addresses_.Add(m);
				WhenArrayAction();
				break;
			}
		}
	}
	else if(n1 == n2) { // modified
		Addresses_.Clear();
		for(int i = 0; i < n2; ++i) {
			Addresses_.Add(array_.Get(i, 0));
		}
		WhenArrayAction();
	}
}

void TunnelAddressEditor::Drag()
{
	auto i = array_.GetCursor();
	String addr = array_.Get(i, 0).ToString();

	auto clip = InternalClip(array_, AddressDnDName);
	Append(clip, addr);

	array_.DoDragAndDrop(clip);
}

void TunnelAddressEditor::Drop(PasteClip& clip)
{
	if(AcceptText(clip)) {
		auto addr = GetString(clip);
		array_.Add(addr);
		array_.SetFocus();
		Addresses_.Add(addr);
		WhenArrayAction();
	}
}

void TunnelAddressEditor::DropInsert(int i, PasteClip& clip)
{
	if(AcceptInternal<ArrayCtrl>(clip, AddressDnDName)) {
		array_.InsertDrop(i, clip);
		array_.SetFocus();

		auto addr = GetString(clip);
		if(i >= Addresses_.GetCount()) {
			Addresses_.Add(addr);
		}
		else {
			Addresses_.Insert(i, addr);
		}
		WhenArrayAction();
	}
}

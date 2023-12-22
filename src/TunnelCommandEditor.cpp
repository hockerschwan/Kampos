#include "TunnelCommandEditor.hpp"

const char* CommandDnDName = "CommandDragAndDrop";

TunnelCommandEditor::TunnelCommandEditor()
{
	CtrlLayout(*this);
	Clear();
	array_.AddColumn("Command").Edit(edit_);
	array_.WhenAcceptEdit = [&] { AcceptEdit(); };
	array_.WhenArrayAction = [&] {
		Commands_.Clear();
		for(int i = 0; i < array_.GetCount(); ++i) {
			Commands_.Add(array_.Get(i, 0).ToString());
		}
		WhenArrayAction();
	};
}

void TunnelCommandEditor::Add(const String& command)
{
	Commands_.Add(command);
	array_.Add(command);
}

void TunnelCommandEditor::Clear()
{
	Commands_.Clear();
	array_.Clear();
}

void TunnelCommandEditor::AcceptEdit()
{
	auto n1 = Commands_.GetCount();
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
			if(Commands_.GetIndex(m) < 0) {
				Commands_.Add(m);
				WhenArrayAction();
				break;
			}
		}
	}
	else if(n1 == n2) { // modified
		Commands_.Clear();
		for(int i = 0; i < n2; ++i) {
			Commands_.Add(array_.Get(i, 0));
		}
		WhenArrayAction();
	}
}

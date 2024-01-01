#include "TunnelCommandEditor.hpp"

const char* CommandDnDName = "CommandDragAndDrop";

TunnelCommandEditor::TunnelCommandEditor()
{
	CtrlLayout(*this);

	Clear();
	array_.AddColumn("Command").Edit(edit_);
	array_.WhenAcceptEdit = [&] { AcceptEdit(); };
	array_.WhenArrayAction = [&] {
		if(Commands_.GetCount() != array_.GetCount()) {
			Commands_.Clear();
			for(int i = 0; i < array_.GetCount(); ++i) {
				Commands_.Add(array_.Get(i, 0).ToString());
			}
			WhenArrayAction();
		}
	};
	array_.WhenUpdateRow = [&] { AcceptEdit(); };

	btnCopy_.SetImage(Rescale(AppIcons::Copy(), Zx(15), Zx(15)));
	btnCopy_.WhenAction = [&] {
		String str{};
		for(int i = 0; i < Commands_.GetCount(); ++i) {
			str << Commands_[i];
			if(i != Commands_.GetCount() - 1) {
				str << ", ";
			}
		}
		AppendClipboardText(pick(str));
	};

	btnPaste_.SetImage(Rescale(AppIcons::Paste(), Zx(15), Zx(15)));
	btnPaste_.WhenAction = [&] {
		auto str = ReadClipboardText();
		if(str.IsEmpty()) {
			return;
		}

		for(auto& app : Split(str, ",")) {
			app = TrimBoth(app);
			if(app.IsEmpty() || array_.Find(app) >= 0) {
				continue;
			}
			Commands_.Add(app);
			array_.Add(app);
		}
		if(Commands_.GetCount() > 0) {
			WhenArrayAction();
		}
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

	if(n1 < n2) {
		for(int i = n2 - 1; i >= 0; --i) { // remove empty
			if(array_.Get(i, 0).ToString().IsEmpty()) {
				array_.Remove(i);
			}
		}

		for(int i = 0; i < n2; ++i) {
			auto m = array_.Get(i, 0).ToString();
			if(Commands_.GetIndex(m) < 0) {
				Commands_.Add(m);
				WhenArrayAction();
				break;
			}
		}
	}
	else if(n1 == n2) {
		Commands_.Clear();
		for(int i = 0; i < n2; ++i) {
			Commands_.Add(array_.Get(i, 0));
		}
		WhenArrayAction();
	}
}

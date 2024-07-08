#include "TunnelAppEditor.hpp"

TunnelAppEditor::TunnelAppEditor()
{
	CtrlLayout(*this);

	Clear();
	array_.AddColumn("Address").Edit(edit_);
	array_.WhenAcceptEdit = [&] { AcceptEdit(); };
	array_.WhenArrayAction = [&] {
		if(Apps_.GetCount() != array_.GetCount()) {
			Apps_.Clear();
			for(int i = 0; i < array_.GetCount(); ++i) {
				auto app = array_.Get(i, 0).ToString();
				if(!app.IsEmpty()) {
					Apps_.Add(app);
				}
			}
			Sort(Apps_);
			WhenArrayAction();
		}
	};

	btnCopy_.SetImage(Rescale(AppIcons::Copy(), Zx(15), Zx(15)));
	btnCopy_.WhenAction = [&] {
		String str{};
		for(int i = 0; i < Apps_.GetCount(); ++i) {
			str << Apps_[i];
			if(i != Apps_.GetCount() - 1) {
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
			Apps_.Add(app);
			array_.Add(app);
		}
		if(Apps_.GetCount() > 0) {
			Sort(Apps_);
			array_.Sort();
			WhenArrayAction();
		}
	};
}

void TunnelAppEditor::Add(const String& app)
{
	Apps_.Add(app);
	Sort(Apps_);
	array_.Add(app);
	array_.Sort();
}

void TunnelAppEditor::Clear()
{
	Apps_.Clear();
	array_.Clear();
}

void TunnelAppEditor::AcceptEdit()
{
	auto n1 = Apps_.GetCount();
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
			if(Apps_.GetIndex(m) < 0) {
				Apps_.Add(m);
				Sort(Apps_);
				array_.Sort();
				WhenArrayAction();
				break;
			}
		}
	}
	else if(n1 == n2) { // modified
		Apps_.Clear();
		for(int i = 0; i < n2; ++i) {
			Apps_.Add(array_.Get(i, 0));
		}
		Sort(Apps_);
		array_.Sort();
		WhenArrayAction();
	}
}

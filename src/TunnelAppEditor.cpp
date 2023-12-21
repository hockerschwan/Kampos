#include "TunnelAppEditor.hpp"

TunnelAppEditor::TunnelAppEditor()
{
	CtrlLayout(*this);
	Clear();
	array_.AddColumn("Address").Edit(edit_);
	array_.WhenAcceptEdit = [&] {
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
				if(Apps_.Find(m) < 0) {
					Apps_.Add(m);
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
			array_.Sort();
			WhenArrayAction();
		}
	};
	array_.WhenArrayAction = [&] {
		if(Apps_.GetCount() > array_.GetCount()) { // removed
			for(auto& app : Apps_) {
				if(array_.Find(app, 0) < 0) {
					Apps_.RemoveKey(app);
					WhenArrayAction();
					break;
				}
			}
		}
	};
}

const String TunnelAppEditor::ToString() const
{
	String str{};
	for(int i = 0; i < Apps_.GetCount(); ++i) {
		str << Apps_[i];
		if(i != Apps_.GetCount() - 1) {
			str << ", ";
		}
	}
	return pick(str);
}

void TunnelAppEditor::Add(const String& app)
{
	Apps_.Add(app);
	array_.Add(app);
	array_.Sort();
}

void TunnelAppEditor::Clear()
{
	Apps_.Clear();
	array_.Clear();
}

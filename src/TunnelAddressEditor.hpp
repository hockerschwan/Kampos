#ifndef _TunnelAddressEditor_hpp_
#define _TunnelAddressEditor_hpp_

#include "common.hpp"

struct TunnelAddressEditor : public WithTunnelAddressEditorLayout<ParentCtrl>, Moveable<TunnelAddressEditor> {
public:
	TunnelAddressEditor();

	void SetText(const String& text) { textTitle_.SetText(text); };
	Array<String> Get() const { return pick(clone(Addresses_)); };

	void Add(const String& address);
	void Clear();

	Event<> WhenArrayAction;

private:
	void AcceptEdit();
	void Drag();
	void Drop(PasteClip& clip);
	void DropInsert(int i, PasteClip& clip);

	Array<String> Addresses_{};

	EditField edit_;
};

#endif

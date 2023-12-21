#ifndef _TunnelAddressEditor_hpp_
#define _TunnelAddressEditor_hpp_

#include "common.hpp"

struct TunnelAddressEditor : public WithTunnelAddressEditorLayout<ParentCtrl>, Moveable<TunnelAddressEditor> {
public:
	TunnelAddressEditor();

	void SetText(const String& text) { textTitle_.SetText(text); };
	Index<String> Get() const { return pick(clone(Addresses_)); };

	const String ToString() const;

	void Add(const String& address);
	void Clear();

	Event<> WhenArrayAction;

private:
	void AcceptEdit();
	void Drag();
	void Drop(PasteClip& clip);
	void DropInsert(int i, PasteClip& clip);

	Index<String> Addresses_{};

	EditField edit_;
};

#endif

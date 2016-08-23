#ifndef _Gui_H_
#define _Gui_H_

#include "UIComponent.h"
#include "UIButton.h"
#include "UIImage.h"

#include <list>

class CGui
{
public:
	CGui();
	~CGui();
	void addCtrl(CUIComponent* component);
	void removeCtrl(CUIComponent* component);
	void removeAll();
	void update();
	void render();
private:
	HGE *m_hge;
	CUIComponent* m_curCtrl;
	std::list<CUIComponent*> m_list;
};
#endif _Gui_H_
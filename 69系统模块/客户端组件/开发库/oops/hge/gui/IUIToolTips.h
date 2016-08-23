#ifndef _IUIToolTips_H_
#define _IUIToolTips_H_

class CUIComponent;

class IUIToolTips
{
public:
	virtual ~IUIToolTips() {}

	virtual void Show(CUIComponent* owner, float x, float y) = 0;
	virtual void Hide(CUIComponent* owner) = 0;
	virtual void MouseMove(CUIComponent* owner, float x, float y) {};
	virtual void Render() = 0;
}; // IUIToolTips

#endif	// _IUIToolTips_H_
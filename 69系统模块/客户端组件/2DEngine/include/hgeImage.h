//{@ add by Lotus 2014/3/19 C2DImage
#include "hgesprite.h"

class C2DImage
{
public:
	C2DImage();
	~C2DImage();
public:
	HTEXTURE			m_Texture;
	hgeSprite*			m_pSprite;
public:
	//º”‘ÿŒ∆¿Ì
	void LoadTexture(HGE *pHge,CHAR szFileName[]);
	//
	void LoadPackTexture(HGE *pHge,CHAR szFileName[]);
	//ªÊª≠ÕºœÒ
	void DrawImage(HGE *pHge,float x,float y);
	//
	void DrawImage(HGE *pHge,float x,float y,float rot, float hscale=1.0f, float vscale=0.0f);
};
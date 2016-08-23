#include "..\..\include\hgeImage.h"
C2DImage::C2DImage()
{

}

C2DImage::~C2DImage()
{

}

void C2DImage::LoadTexture(HGE *pHge,CHAR szFileName[])
{
	m_Texture = pHge->Texture_Load(szFileName);
	///资源
	int nImageWidth, nImageHeight;
	/// 背景
	nImageWidth = pHge->Texture_GetWidth(m_Texture, true);
	nImageHeight = pHge->Texture_GetHeight(m_Texture, true);
	m_pSprite = new hgeSprite(m_Texture, 0.0f, 0.0f, (float)nImageWidth, (float)nImageHeight);

}

void C2DImage::LoadPackTexture(HGE *pHge,CHAR szFileName[])
{
	DWORD dwSize = 0;
	void* data = pHge->Resource_Load(szFileName,&dwSize);//加载资源

	m_Texture = pHge->Texture_Load((const char*)data,dwSize);

	///资源
	int nImageWidth, nImageHeight;
	/// 背景
	nImageWidth = pHge->Texture_GetWidth(m_Texture, true);
	nImageHeight = pHge->Texture_GetHeight(m_Texture, true);
	m_pSprite = new hgeSprite(m_Texture, 0.0f, 0.0f, (float)nImageWidth, (float)nImageHeight);
	m_pSprite->SetHotSpot(nImageHeight/2,nImageHeight/2);
}

void C2DImage::DrawImage(HGE *pHge,float x,float y)
{
	m_pSprite->Render(x,y);
}

void C2DImage::DrawImage(HGE *pHge,float x,float y,float rot, float hscale/*=1.0f*/, float vscale/*=0.0f*/)
{

	m_pSprite->RenderEx(x,y,rot,hscale,vscale);
}

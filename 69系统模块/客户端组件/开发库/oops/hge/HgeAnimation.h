/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeAnimation helper class header
*/
#ifndef _HgeAnimation_H_
#define _HgeAnimation_H_

#include "HgeHeader.h"

#define HGEANIM_FWD			0
#define HGEANIM_REV			1
#define HGEANIM_PINGPONG	2
#define HGEANIM_NOPINGPONG	0
#define HGEANIM_LOOP		4
#define HGEANIM_NOLOOP		0


class CHgeAnimation : public hgeSprite
{
public:
	CHgeAnimation(HTEXTURE tex, int nframes, float FPS, float x, float y, float w, float h);
	CHgeAnimation(const CHgeAnimation &anim);

	void		Play();
	void		Stop() { bPlaying=false; }
	void		Resume() { bPlaying=true; }
	void		Update(float fDeltaTime);
	bool		IsPlaying() const { return bPlaying; }

	void		SetTextureInfo(HTEXTURE tex, int nframes, float w, float h);
	void		SetTextureInfo(HTEXTURE tex, int rows, int cols);
	void		SetTexture(HTEXTURE tex) { hgeSprite::SetTexture(tex); orig_width = hge->Texture_GetWidth(tex, true); }
	void		SetTextureRect(float x1, float y1, float x2, float y2) { hgeSprite::SetTextureRect(x1,y1,x2,y2); SetFrame(nCurFrame); }
	void		SetMode(int mode);
	void		SetSpeed(float FPS) { fSpeed=1.0f/FPS; }
	void		SetFrame(int n);
	void		SetFrames(int n) { nFrames=n; }

	int			GetMode() const { return Mode; }
	float		GetSpeed() const { return 1.0f/fSpeed; }
	int			GetFrame() const { return nCurFrame; }
	int			GetFrames() const { return nFrames; }

	void		RenderExEx(float x, float y, float rot = 0, float hscale = 1, float vscale = 0, float xExt[] = NULL, float yExt[] = NULL);

private:
	CHgeAnimation();

	int			orig_width;

	bool		bPlaying;

	float		fSpeed;
	float		fSinceLastFrame;

	int			Mode;
	int			nDelta;
	int			nFrames;
	int			nCurFrame;
};


#endif

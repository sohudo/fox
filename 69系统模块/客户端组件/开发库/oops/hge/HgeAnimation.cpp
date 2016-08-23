/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeAnimation helper class implementation
*/

#include "Stdafx.h"
#include "HgeAnimation.h"


CHgeAnimation::CHgeAnimation(HTEXTURE tex, int nframes, float FPS, float x, float y, float w, float h)
: hgeSprite(tex, x, y, w, h)
{
	orig_width = hge->Texture_GetWidth(tex, true);

	fSinceLastFrame=-1.0f;
	fSpeed=1.0f/FPS;
	bPlaying=false;
	nFrames=nframes;

	Mode=HGEANIM_FWD | HGEANIM_LOOP;
	nDelta=1;
	SetFrame(0);
}

CHgeAnimation::CHgeAnimation(const CHgeAnimation & anim)
: hgeSprite(anim)
{ 
	// Copy CHgeAnimation parameters: 
	this->orig_width	  = anim.orig_width;
	this->bPlaying        = anim.bPlaying; 
	this->fSpeed          = anim.fSpeed; 
	this->fSinceLastFrame = anim.fSinceLastFrame;
	this->Mode            = anim.Mode;
	this->nDelta          = anim.nDelta;
	this->nFrames         = anim.nFrames;
	this->nCurFrame       = anim.nCurFrame;
}

void CHgeAnimation::SetTextureInfo(HTEXTURE tex, int nframes, float w, float h)
{
	nFrames=nframes;
	SetTexture(tex);
	SetTextureRect(0, 0, w, h);
}

void CHgeAnimation::SetTextureInfo(HTEXTURE tex, int rows, int cols)
{
	nFrames=rows*cols;
	SetTexture(tex);
	SetTextureRect(0, 0, (float)hge->Texture_GetWidth(tex, true) / cols, (float)hge->Texture_GetHeight(tex, true) / rows);
}

void CHgeAnimation::SetMode(int mode)
{
	Mode=mode;

	if(mode & HGEANIM_REV)
	{
		nDelta = -1;
		SetFrame(nFrames-1);
	}
	else
	{
		nDelta = 1;
		SetFrame(0);
	}
}


void CHgeAnimation::Play()
{
	bPlaying=true;
	fSinceLastFrame=-1.0f;
	if(Mode & HGEANIM_REV)
	{
		nDelta = -1;
		SetFrame(nFrames-1);
	}
	else
	{
		nDelta = 1;
		SetFrame(0);
	}
}


void CHgeAnimation::Update(float fDeltaTime)
{
	if(!bPlaying) return;

	if(fSinceLastFrame == -1.0f)
		fSinceLastFrame=0.0f;
	else
		fSinceLastFrame += fDeltaTime;

	while(fSinceLastFrame >= fSpeed)
	{
		fSinceLastFrame -= fSpeed;

		if(nCurFrame + nDelta == nFrames)
		{
			switch(Mode)
			{
			case HGEANIM_FWD:
			case HGEANIM_REV | HGEANIM_PINGPONG:
				bPlaying = false;
				break;

			case HGEANIM_FWD | HGEANIM_PINGPONG:
			case HGEANIM_FWD | HGEANIM_PINGPONG | HGEANIM_LOOP:
			case HGEANIM_REV | HGEANIM_PINGPONG | HGEANIM_LOOP:
				nDelta = -nDelta;
				break;
			}
		}
		else if(nCurFrame + nDelta < 0)
		{
			switch(Mode)
			{
			case HGEANIM_REV:
			case HGEANIM_FWD | HGEANIM_PINGPONG:
				bPlaying = false;
				break;

			case HGEANIM_REV | HGEANIM_PINGPONG:
			case HGEANIM_REV | HGEANIM_PINGPONG | HGEANIM_LOOP:
			case HGEANIM_FWD | HGEANIM_PINGPONG | HGEANIM_LOOP:
				nDelta = -nDelta;
				break;
			}
		}

		if(bPlaying) SetFrame(nCurFrame+nDelta);
	}
}

void CHgeAnimation::SetFrame(int n)
{
	if (width == 0)
	{
		return;
	}

	float tx1, ty1, tx2, ty2;
	bool bX, bY, bHS;
	int ncols = int(orig_width) / int(width);


	n = n % nFrames;
	if(n < 0) n = nFrames + n;
	nCurFrame = n;

	// calculate texture coords for frame n
	ty1 = ty;
	tx1 = tx + n*width;

	if(tx1 > orig_width-width)
	{
		n -= int(orig_width-tx) / int(width);
		tx1 = width * (n%ncols);
		ty1 += height * (1 + n/ncols);
	}

	tx2 = tx1 + width;
	ty2 = ty1 + height;

	tx1 /= tex_width;
	ty1 /= tex_height;
	tx2 /= tex_width;
	ty2 /= tex_height;

	quad.v[0].tx=tx1; quad.v[0].ty=ty1;
	quad.v[1].tx=tx2; quad.v[1].ty=ty1;
	quad.v[2].tx=tx2; quad.v[2].ty=ty2;
	quad.v[3].tx=tx1; quad.v[3].ty=ty2;

	bX=bXFlip; bY=bYFlip; bHS=bHSFlip;
	bXFlip=false; bYFlip=false;
	SetFlip(bX,bY,bHS);
}

void CHgeAnimation::RenderExEx(float x, float y, float rot, float hscale, float vscale, float xExt[], float yExt[])
{
	float tx1, ty1, tx2, ty2;
	float sint, cost;

	if(vscale==0) vscale=hscale;

	tx1 = -hotX*hscale;
	ty1 = -hotY*vscale;
	tx2 = (width-hotX)*hscale;
	ty2 = (height-hotY)*vscale;

	float xDefExt[4] = {0};
	float yDefExt[4] = {0};

	if (xExt == NULL) xExt = xDefExt;
	if (yExt == NULL) yExt = yDefExt;

	if (rot != 0.0f)
	{
		cost = cosf(rot);
		sint = sinf(rot);

		quad.v[0].x  = (tx1 + xExt[0])*cost - (ty1 + yExt[0]) *sint + x;
		quad.v[0].y  = (tx1 + xExt[0])*sint + (ty1 + yExt[0])*cost + y;	

		quad.v[1].x  = (tx2 + xExt[1])*cost - (ty1 + yExt[1])*sint + x;
		quad.v[1].y  = (tx2 + xExt[1])*sint + (ty1 + yExt[1])*cost + y;	

		quad.v[2].x  = (tx2 + xExt[2])*cost - (ty2 + yExt[2])*sint + x;
		quad.v[2].y  = (tx2 + xExt[2])*sint + (ty2 + yExt[2])*cost + y;	

		quad.v[3].x  = (tx1 + xExt[3])*cost - (ty2 + yExt[3])*sint + x;
		quad.v[3].y  = (tx1 + xExt[3])*sint + (ty2 + yExt[3])*cost + y;	
	}
	else
	{
		quad.v[0].x = (tx1 + xExt[0]) + x; quad.v[0].y = (ty1 + yExt[0]) + y;
		quad.v[1].x = (tx2 + xExt[1]) + x; quad.v[1].y = (ty1 + yExt[1]) + y;
		quad.v[2].x = (tx2 + xExt[2]) + x; quad.v[2].y = (ty2 + yExt[2]) + y;
		quad.v[3].x = (tx1 + xExt[3]) + x; quad.v[3].y = (ty2 + yExt[3]) + y;
	}

	hge->Gfx_RenderQuad(&quad);
}

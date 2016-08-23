#ifndef _HgeHeader_H_
#define _HgeHeader_H_

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
#include <string>
#include <map>
#include <list>
#include <vector>
#include <algorithm>

#ifdef _UNICODE

typedef std::wstring tstring;

#else

typedef std::string tstring;

#endif // _UNICODE

//////////////////////////////////////////////////////////////////////////
// hge 库
//////////////////////////////////////////////////////////////////////////
#define HGE_LIBPATH(p,f) p##".dir/../"##f
// HGE
#undef MAX_PARTICLES

#include "include/hge.h"
#include "include/hgeanim.h"
#include "include/hgeresource.h"
#include "include/hgesprite.h"
#include "include/hgedistort.h"
#include "include/hgeanim.h"
#include "include/hgevector.h"
#include "include/hgeparticle.h"
#include "include/hgegui.h"

#pragma comment(lib,HGE_LIBPATH(__FILE__,   "lib/vc/hge.lib")) 
#pragma comment(lib,HGE_LIBPATH(__FILE__,   "lib/vc/hgehelp.lib")) 

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

#include "GameFrameViewHGE.h"
#include "GfxFont.h"
#include "HgeAnimation.h"
#include "hgesound.h"

// component
#include "gui/Gui.h"


//////////////////////////////////////////////////////////////////////////
// 功能函数
//////////////////////////////////////////////////////////////////////////
/**
 * 对齐方式
 */
#define ALIGN_LEFT     0
#define ALIGN_TOP      0
#define ALIGN_RIGHT    0x1
#define ALIGN_BOTTOM   0x2
#define ALIGN_VCENTER  0x4
#define ALIGN_HCENTER  0x8
#define ALIGN_CENTER   (ALIGN_HCENTER | ALIGN_VCENTER)

template<typename T>
inline void AlignResetPosition(T &x, T &y, T w, T h, unsigned int align)
{
	if (align & ALIGN_HCENTER)
	{
		x -= w / 2;
	}
	else if (align & ALIGN_RIGHT)
	{
		x -= w;
	}

	if (align & ALIGN_VCENTER)
	{
		y -= h / 2;
	}
	else if (align & ALIGN_BOTTOM)
	{
		y -= h;
	}

	x = (T)(int)x;
	y = (T)(int)y;
} // AlignResetPosition

template<typename T>
inline void AlignInsideResetPosition(T &x, T &y, T w, T h, unsigned int align)
{
	if (align & ALIGN_HCENTER)
	{
		x += w / 2;
	}
	else if (align & ALIGN_RIGHT)
	{
		x += w;
	}

	if (align & ALIGN_VCENTER)
	{
		y += h / 2;
	}
	else if (align & ALIGN_BOTTOM)
	{
		y += h;
	}

	x = (T)(int)x;
	y = (T)(int)y;
} // AlignInsideResetPosition

#endif // _HgeHeader_H_

//////////////////////////////////////////////////////////////////////////
//
// 中文字体绘制类
//
// 作者：微妙的平衡(BOGY)
//
// 版本：For HGE v1.7
//
//////////////////////////////////////////////////////////////////////////
#ifndef GDIFONT_H
#define GDIFONT_H

class GfxFont
{
public:
	GfxFont(const char* lpsFontName, int nFaceSize, BOOL bBold = FALSE, BOOL bItalic = FALSE, BOOL bAntialias = TRUE);
	~GfxFont(void);

public:
	void Print( float x, float y, const wchar_t* format, ...);
	// 渲染文本
	void RenderEx( float x, float y, unsigned int align, const wchar_t* format, ...);
	void Render(float x, float y, unsigned int align, const wchar_t* text);

	//描边  dwColor文本颜色   DwStrokeColor描边颜色
	void Print( float x, float y,DWORD dwColor,DWORD DwStrokeColor, const wchar_t* format, ...);

	void Render(float x, float y, unsigned int align, const wchar_t* text,DWORD dwColor,DWORD DwStrokeColor);

	// 设置与获取颜色
	virtual void	SetColor( DWORD dwColor, int i = -1 );
	virtual DWORD	GetColor( int i = 0 );
	virtual void	SetZ(float z);
	virtual float	GetZ( int i = 0 ) const;

	// 获取文本宽高
	virtual SIZE	GetTextSize( const wchar_t* text, int iFrame = 0);

	// 根据坐标获取字符
	virtual wchar_t	GetCharacterFromPos( const wchar_t* text, float pixel_x, float pixel_y );

	// 设置字间距
	virtual void	SetKerningWidth( float kerning );
	virtual void	SetKerningHeight( float kerning );

	// 获取字间距
	virtual float	GetKerningWidth();
	virtual float	GetKerningHeight();

	// 字体大小
	virtual float	GetFontSize();
	
	// 获取字符宽度
	inline float	GetWidthFromCharacter( wchar_t c, bool original = false );

private:
	virtual void	RealRender(float x, float y, const wchar_t* text);
	//描边
	virtual void	RealRender(float x, float y, const wchar_t* text,DWORD dwColor,DWORD DwStrokeColor);
	// 根据字符获取轮廓
	unsigned int	GetGlyphByCharacter( wchar_t c );
	inline void		CacheCharacter(unsigned int idx, wchar_t c);

	typedef struct tagEngineFontGlyph
	{
		HTEXTURE	t;
		float		w;
		float		h;
		float		x;
		float		y;
		float		c;
	}TENGINEFONTGLYPH;

	static const unsigned int font_count = 0xFFFF;// = sizeof(wchar_t);
	TENGINEFONTGLYPH	m_Glyphs[font_count];
	UINT				m_nAntialias;//反锯齿
	LONG				m_nAscent;//基线
	DWORD				m_dwFontColor;
	float				m_nFontSize;
	float				m_nKerningWidth;
	float				m_nKerningHeight;

	class HGE*			m_pHGE;
	class hgeSprite*	m_pSprite;

	// GDI设备
	HDC					m_hMemDC;
	HFONT				m_hFont;
};
#endif//GDIFONT_H

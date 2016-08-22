#ifndef _utils_Utils_H_
#define _utils_Utils_H_

#include <cassert>
#include <cstdarg>
#include <vector>

namespace Utils
{
	//////////////////////////////////////////////////////////////////////////
	// 设置高低位
	//////////////////////////////////////////////////////////////////////////
	inline unsigned int MakeInt(unsigned int high, unsigned int low)
	{
		return (((high & 0xffff) << 16) | (low & 0xffff));
	} // MakeInt

	inline unsigned int HighInt(unsigned int val)
	{
		return ((val >> 16) & 0xffff);
	} // HighInt

	inline unsigned int LowInt(unsigned int val)
	{
		return (val & 0xffff);
	} // LowInt

	inline unsigned short MakeShort(unsigned short high, unsigned short low)
	{
		return (((high & 0xff) << 8 ) | (low & 0xff));
	} // MakeShort

	inline unsigned short HighShort(unsigned short val)
	{
		return ((val >> 8) & 0xff);
	} // HighShort

	inline unsigned short LowShort(unsigned short val)
	{
		return (val & 0xff);
	} // LowShort

	//////////////////////////////////////////////////////////////////////////
	// 掩码操作
	//////////////////////////////////////////////////////////////////////////
	/**
	 * 设置掩码位
	 */
	template<typename T>
	inline void SetMask(T &val, BYTE pos, bool isOne)
	{
		assert((sizeof(T) * 8 > pos) && "超出位数长度");

		if (isOne)
		{
			T bT  = (1 << pos);
			val |= bT;
		}
		else
		{
			T bT  = ~(1 << pos);
			val &= bT;
		}
	} // SetMask

	/**
	 * 获取掩码位
	 */
	template<typename T>
	inline bool GetMask(T val, BYTE pos)
	{
		assert((sizeof(T) * 8 > pos) && "超出位数长度");
		return (val & (1 << pos)) != 0;
	} // GetMask

	//////////////////////////////////////////////////////////////////////////
	// 字符串操作
	//////////////////////////////////////////////////////////////////////////
	/**
	 * 去掉字符前后空白字符
	 * @param[inout]	str: 字符串
	 * @param[in]		mode: 模式 [0:前后, 1:只去前 2:只去后 
	 */
	inline void trim(TCHAR* str, int mode = 0)
	{
		if (str == 0)
		{
			return;
		}

		int nLen = lstrlen(str);

		if (mode == 0 || mode == 2)
		{
			while (nLen > 0 && str[nLen - 1] == TEXT(' '))
			{
				str[nLen - 1] = TEXT('\0');
				--nLen;
			}
		}
		
		if (mode == 0 || mode == 1)
		{
			int index = 0;

			while (index < nLen && str[index] == TEXT(' '))
			{
				++index;
			}

			if (index != 0)
			{
			#ifdef _UNICODE
				wmemmove(&str[0], &str[index], nLen - index);;
			#else
				memmove(&str[0], &str[index], nLen - index);
			#endif
				str[nLen - index] = TEXT('\0');
			}
		}
	} // trim

	//////////////////////////////////////////////////////////////////////////
	// 资源操作
	//////////////////////////////////////////////////////////////////////////
	// 资源处理函数原型
	typedef bool (*DATA_HANDLE)(void* pBuffer, DWORD dwSize, void* pUserData);
	// 资源处理函数
	inline bool HandleResData(LPCTSTR type, LPCTSTR name, LPCTSTR dll, DATA_HANDLE fnDataHandle, void* pUserData)
	{
		try
		{
			HMODULE hModule = GetModuleHandle(dll);
			if (hModule == NULL) return false;

			HRSRC hRsrc;// = FindResource(hModule, name, type);

			if (lstrcmp(type, TEXT("BMP")) != 0)
			{
				hRsrc = FindResource(hModule, name, type);
			}
			else
			{
				hRsrc = FindResource(hModule, name, RT_BITMAP);
			}

			if (hRsrc == NULL) 
			{
				DWORD err = GetLastError();
				_tprintf(TEXT("FindResource type:%s name:%s failed![err:%d]\n"), type, name, err);
				return false;
			}

			HGLOBAL hGlobal = LoadResource(hModule, hRsrc);
			if (hGlobal==NULL) return false;
			
			void* pBuffer = LockResource(hGlobal);
			DWORD dwSize  = SizeofResource(hModule, hRsrc); 

			// 处理资源
			bool bOk = fnDataHandle(pBuffer, dwSize, pUserData);

			UnlockResource(hGlobal);
			FreeResource(hGlobal);
			return bOk;
		}
		catch (...)
		{

		}

		return false;
	} // HandleResData

	//////////////////////////////////////////////////////////////////////////
	// 页数控制器
	//////////////////////////////////////////////////////////////////////////
	class CPagesController
	{
	public:
		/**
		 * @param[in] iPageItems:int	一页的数目
		 * @param[in] iBasePage:int		页数最小下标
		 */
		CPagesController(int iPageItems, int iBasePage = 1)
			: m_iPageItems(iPageItems)
			, m_iCurPage(iBasePage)
			, m_iTotalPage(iBasePage)
			, m_iBasePage(iBasePage)
		{}

		/**
		 * 设置当前项数目
		 * @param[in] iItems:int 当前项数目
		 */
		void SetItems(int iItems)
		{
			m_iItems		= iItems;
			m_iTotalPage	= m_iItems / m_iPageItems;
			m_iTotalPage   += (m_iItems % m_iPageItems != 0) ? 1 : 0;

			if (m_iTotalPage == 0)
			{
				m_iTotalPage = m_iBasePage;
			}

			CorrectCurPage();
		} // SetItems

		/**
		 * 设置当前页
		 */
		void SetCurPage(int iCurPage)
		{
			m_iCurPage = iCurPage;
			CorrectCurPage();
		} // SetCurPage

		int GetCurPage() const
		{
			return m_iCurPage;
		} // GetCurPage

		int GetTotalPage() const
		{
			return m_iTotalPage;
		} // GetTotalPage

		/**
		 * 获取当前页项下标和个数
		 * @param[out] startIndex:int	当前页项下标
		 * @param[out] count:int		当前页的数量
		 */
		void GetCurPageItems(int &startIndex, int &count)
		{
			int iPage  = m_iCurPage - m_iBasePage;
			startIndex = iPage * m_iPageItems;
			count	   = max(0, min(m_iItems - startIndex, m_iPageItems));
		} // GetCurPageItems

		/** 上一页 */
		void PrevPage()
		{
			m_iCurPage -= 1;
			CorrectCurPage();
		} // PrevPage

		/** 下一页 */
		void NextPage()
		{
			m_iCurPage += 1;
			CorrectCurPage();
		} // NextPage

		/** 是否有上一页 */
		bool hasPrevPage() const
		{
			return m_iCurPage > m_iBasePage;
		} // hasPrevPage

		/** 是否有下一页 */
		bool hasNextPage() const
		{
			return m_iCurPage < (m_iTotalPage + m_iBasePage - 1);
		} // hasNextPage
	private:
		void CorrectCurPage()
		{
			if (m_iCurPage >= m_iTotalPage)
			{
				m_iCurPage = m_iTotalPage + m_iBasePage - 1;
			}

			if (m_iCurPage < m_iBasePage)
			{
				m_iCurPage = m_iBasePage;
			}
		} // CorrectCurPage
	private:
		// 最小页下标
		int m_iBasePage;
		// 当前页
		int m_iCurPage;
		// 所有页
		int m_iTotalPage;
		// 页项数目
		int m_iPageItems;
		// 当前数目
		int m_iItems;
	}; // CPagesController
	//////////////////////////////////////////////////////////////////////////
	// 随机
	//////////////////////////////////////////////////////////////////////////
	inline int RandomArea(int nLen, ... )
	{
		using namespace std;

		if ( nLen <= 0 )
			return 0;

		va_list argptr;
		va_start(argptr, nLen);
		vector<int> Array;
		int nIndex = 0;
		int nACTotal = 0;

		for ( BYTE i = 0; i < nLen; ++i )
		{
			int nTemp = va_arg( argptr, int );
			if( nTemp >= 0 && nTemp < 100000 )
			{
				Array.push_back(nTemp);
				nACTotal += nTemp;
			}
			else
			{
				ASSERT(FALSE);
				break;
			}
		}
		va_end(argptr);

		if ( nACTotal > 0 )
		{
			static UINT nRandAuxiliary = 0;
			nRandAuxiliary += rand()%10;
			int nRandNum = ((UINT)(rand() + GetTickCount() + nRandAuxiliary)) % nACTotal;
			for (int i = 0, l = (int)Array.size(); i < l; i++)
			{
				nRandNum -= Array[i];
				if (nRandNum < 0)
				{
					nIndex = i;
					break;
				}
			}
		}
		else
		{
			nIndex = rand()%nLen;
		}
	
		return nIndex;
	}

	template<typename T>
	inline void RandomArray(const T arrIn[], T arrOut[], int iAryCount)
	{
		if (iAryCount <= 1)
		{
			return;
		}

		if (arrOut != arrIn)
		{
			CopyMemory(arrOut, arrIn, sizeof(T) * iAryCount);
		}

		for (int i = 0; i < iAryCount - 1; ++i)
		{
			int iRandIndex		= rand() % (iAryCount - i - 1) + i + 1;
			T tmp				= arrOut[i];
			arrOut[i]			= arrOut[iRandIndex];
			arrOut[iRandIndex]  = tmp;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 
	//////////////////////////////////////////////////////////////////////////
	inline bool GetPrivateProfileIntEx(int &iOut, const TCHAR* data_name, const TCHAR* key_name, int iDefault, const TCHAR* config_name)
	{
		iOut = GetPrivateProfileInt(data_name, key_name, INT_MIN, config_name); 

		if (iOut == INT_MIN)
		{
			iOut = iDefault;
			return false;
		}

		return true;
	} // GetPrivateProfileIntEx

	inline bool GetPrivateProfileStringEx(TCHAR* szOut, DWORD nSize, const TCHAR* data_name, const TCHAR* key_name, const TCHAR* szDefault, const TCHAR* config_name)
	{
		GetPrivateProfileString(data_name, key_name,TEXT("*"), szOut, nSize, config_name);

		if (lstrcmp(szOut, TEXT("*")) == 0)
		{
			lstrcpy(szOut, szDefault);
			return false;
		}

		return true;
	} // GetPrivateProfileStringEx

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	class CElapsed
	{
	public:
		void init(DWORD dwDelay)
		{
			m_dwStart = GetTickCount();
			m_dwDelay = dwDelay;
		}

		bool isOver2(DWORD dwDelay) const
		{
			return GetTickCount() - m_dwStart >= dwDelay;
		}

		bool isOver() const
		{
			return isOver2(m_dwDelay);
		}
	private:
		DWORD m_dwStart;
		DWORD m_dwDelay;
	};
} // Utils
#endif // _utils_Utils_H_
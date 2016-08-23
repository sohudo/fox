#include "Stdafx.h"
#include <Windows.h>
#include <ctime>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include "Log.h"
using namespace std;
//////////////////////////////////////////////////////////////////////////
// Log File
//////////////////////////////////////////////////////////////////////////
#ifdef LOG_FILE

#define MAX_LOGFILE_SIZE (1024*1024*512)

#ifdef _UNICODE
#define _tfprintf fwprintf
#else
#define _tfprintf fprintf
#endif

typedef struct
{
	INT32 m_nSec;       ///< Seconds.     [0-60]
	INT32 m_nMin;       ///< Minutes.     [0-59]
	INT32 m_nHour;      ///< Hours.       [0-23]
	INT32 m_nMday;      ///< Day.         [1-31]
	INT32 m_nMon;       ///< Month.       [0-11]
	INT32 m_nYear;      ///< Year
	INT32 m_nWday;      ///< Day of week. [0-6]
}SLocalTime;

void  SGSYS_GetLocalTime(UINT32 dwTime,SLocalTime* pstLocalTime)
{
	if (dwTime == 0)
	{
		SYSTEMTIME  stTime;
		GetLocalTime(&stTime);

		pstLocalTime->m_nYear   = stTime.wYear;
		pstLocalTime->m_nMon    = stTime.wMonth;
		pstLocalTime->m_nMday   = stTime.wDay;
		pstLocalTime->m_nHour   = stTime.wHour;
		pstLocalTime->m_nMin    = stTime.wMinute;
		pstLocalTime->m_nSec    = stTime.wSecond;
		pstLocalTime->m_nWday   = stTime.wDayOfWeek;
	}
	else
	{
		struct tm* pstTime = localtime((time_t*)&dwTime);

		pstLocalTime->m_nSec	= pstTime->tm_sec;
		pstLocalTime->m_nMin	= pstTime->tm_min;
		pstLocalTime->m_nHour	= pstTime->tm_hour;
		pstLocalTime->m_nMday	= pstTime->tm_mday;
		pstLocalTime->m_nMon	= pstTime->tm_mon + 1;
		pstLocalTime->m_nYear	= pstTime->tm_year + 1900;
		pstLocalTime->m_nWday	= pstTime->tm_wday; 
	}
} // SGSYS_GetLocalTime

bool makedirs(const TCHAR* root)
{
	if (_taccess(root, 0) == -1)
	{
		int nLen	= lstrlen(root);
		int nStart	= 0;

		for(int i = 0; i < nLen; ++i)
		{
			if(root[i]==TEXT('\\') || root[i]==TEXT('/'))
			{
				TCHAR szPath[MAX_PATH]={0};
				lstrcpyn(szPath, root, i + 1);

				if (_tmkdir(szPath) == -1)
				{
					continue;
				}

				nStart = i;
			}
		}

		if((nLen - nStart)> 0)
		{
			_tmkdir(root);
		}

		if (_taccess(root, 0) == -1)
		{
			return false;
		}
	}

	return true;
} // makedirs

//--------------------------------------------------------------
static int   g_iFileCnt = 0;
static FILE* g_File		= 0;
static INT32 g_curYear  = 0;
static INT32 g_curMon   = 0;
static INT32 g_curDay   = 0;

void gFileLogOpen()
{
	_tsetlocale(LC_ALL, TEXT("chs"));	// 中文支持
} // gFileLogOpen

void gFileLogClose()
{
	if (g_File != NULL)
	{
		fflush(g_File);
		fclose(g_File);
	}

	g_File = NULL;
} // gFileLogClose

void gFileLogWrite(const TCHAR* root, const TCHAR* info)
{
	if (!makedirs(root))
	{
		// 创建失败
		return;
	}

	SLocalTime t;
	SGSYS_GetLocalTime(0, &t);
	bool isSameTime = (t.m_nYear == g_curYear && t.m_nMon == g_curYear && t.m_nMday == g_curDay);

	while (g_File == NULL || !isSameTime|| (ftell(g_File) > MAX_LOGFILE_SIZE))
	{

		gFileLogClose();

		if (!isSameTime)
		{
			g_iFileCnt	= 0;
			g_curYear	= t.m_nYear;
			g_curMon	= t.m_nMon;
			g_curDay	= t.m_nMday;
			isSameTime  = true;
		}

		TCHAR szPath[MAX_PATH] ={0};
		_stprintf(szPath, TEXT("%s/%4d_%02d_%02d_file_%d.txt"), root, g_curYear, g_curMon, g_curDay, ++g_iFileCnt);
		g_File = _tfopen(szPath, TEXT("a+"));

		if (g_File == NULL)
		{
			break;
		}

		fseek(g_File, 0, SEEK_END);
	}

	if (g_File != NULL)
	{

		_tfprintf(g_File, 
			TEXT("[%02d:%02d:%02d] %s\n"),
			t.m_nHour, t.m_nMin, t.m_nSec,
			info);
		fflush(g_File);
	}
} // gFileLogWrite

#else

void gFileLogOpen() {}
void gFileLogWrite(const TCHAR* root, const TCHAR* info) {}
void gFileLogClose(){}
#endif // LOG_FILE



//////////////////////////////////////////////////////////////////////////
// Log
//////////////////////////////////////////////////////////////////////////
CLog g_Log;

#ifdef LOG_OPEN


#include <Windows.h>

using namespace std;
static int g_iRef   = 0;
static bool g_bInit = false;

CLog::CLog()
{
	if (g_iRef == 0)
	{
		gFileLogOpen();
		_tsetlocale(LC_ALL, TEXT("chs"));	// 中文支持
		g_bInit = AllocConsole() != 0;
		freopen("CONOUT$", "w+t", stdout);  // 申请写
	}

	++g_iRef;
} // CLog

CLog::~CLog()
{
	--g_iRef;
	if (g_iRef == 0)
	{
		if (g_bInit)
		{
			g_bInit = false;
			FreeConsole();
		}

		gFileLogClose();
	}
} // ~CLog

void CLog::Clear()
{
	system("cls");
} // Clear

void CLog::Print(const TCHAR* format, ...)
{
	TCHAR szOutput[1024];
	va_list ap;
	va_start(ap, format);
	int nLen = _vsntprintf(szOutput, 1024, format, ap);
	va_end(ap); 

	_tprintf(TEXT("====================\n%s\n--------------------\n"), szOutput);
} // Print

void CLog::LogToFile(const TCHAR* root, const TCHAR* format,...)
{
#ifdef LOG_FILE
	TCHAR szOutput[1024];
	va_list ap;
	va_start(ap, format);
	int nLen = _vsntprintf(szOutput, 1024, format, ap);
	va_end(ap); 

	gFileLogWrite(root, szOutput);
#endif // LOG_FILE
} // LogToFile

#else // 

CLog::CLog()
{
	gFileLogOpen();
} // CLog

CLog::~CLog()
{
	gFileLogClose();
} // ~CLog

void CLog::Clear()
{

}

void CLog::Print(const TCHAR*format, ...)
{
} // Print

void CLog::LogToFile(const TCHAR* root, const TCHAR* format,...)
{
#ifdef LOG_FILE
	TCHAR szOutput[1024];
	va_list ap;
	va_start(ap, format);
	int nLen = _vsntprintf(szOutput, 1024, format, ap);
	va_end(ap); 

	gFileLogWrite(root, szOutput);
#endif // LOG_FILE
} // LogToFile

#endif

//////////////////////////////////////////////////////////////////////////
// FileLog
//////////////////////////////////////////////////////////////////////////

#ifdef LOG_FILE
CFileLog::CFileLog()
{
	m_szRoot[0]	= 0;
	m_iFileCnt	= 0;
	m_File		= 0;
	m_curYear	= 0;
	m_curMon	= 0;
	m_curDay	= 0;
	_tsetlocale(LC_ALL, TEXT("chs"));	// 中文支持
} // CFileLog

CFileLog::~CFileLog()
{
	if (m_File != NULL)
	{
		fflush(m_File);
		fclose(m_File);
	}

	m_File = NULL;
}

void CFileLog::SetRoot(const TCHAR* format,...)
{
	va_list ap;
	va_start(ap, format);
	int nLen = _vsntprintf(m_szRoot, MAX_PATH, format, ap);
	va_end(ap); 
} // SetRoot

void CFileLog::Close()
{
	if (m_File != NULL)
	{
		fflush(m_File);
		fclose(m_File);
	}

	m_File = NULL;
} // Close

void CFileLog::Log(const TCHAR* format,...)
{
	if (m_szRoot[0] == 0)
	{
		return;
	}

	TCHAR szOutput[1024];
	va_list ap;
	va_start(ap, format);
	int nLen = _vsntprintf(szOutput, 1024, format, ap);
	va_end(ap); 

	if (!makedirs(m_szRoot))
	{
		// 创建失败
		return;
	}

	SLocalTime t;
	SGSYS_GetLocalTime(0, &t);
	bool isSameTime = (t.m_nYear == m_curYear && t.m_nMon == m_curYear && t.m_nMday == m_curDay);

	while (m_File == NULL || !isSameTime|| (ftell(m_File) > MAX_LOGFILE_SIZE))
	{

		Close();

		if (!isSameTime)
		{
			m_iFileCnt	= 0;
			m_curYear	= t.m_nYear;
			m_curMon	= t.m_nMon;
			m_curDay	= t.m_nMday;
			isSameTime  = true;
		}

		TCHAR szPath[MAX_PATH] ={0};
		_stprintf(szPath, TEXT("%s/%4d_%02d_%02d_file_%d.txt"), m_szRoot, m_curYear, m_curMon, m_curDay, ++m_iFileCnt);
		m_File = _tfopen(szPath, TEXT("a+"));

		if (m_File == NULL)
		{
			break;
		}

		fseek(m_File, 0, SEEK_END);
	}

	if (m_File != NULL)
	{

		_tfprintf(m_File, 
			TEXT("[%02d:%02d:%02d] %s\n"),
			t.m_nHour, t.m_nMin, t.m_nSec,
			szOutput);
		fflush(m_File);
	}
} // Log
#else

CFileLog::CFileLog()
{
} // CFileLog

CFileLog::~CFileLog()
{
}


void CFileLog::SetRoot(const TCHAR* format,...)
{
}

void CFileLog::Close()
{
} // Close

void CFileLog::Log(const TCHAR* format,...)
{
} // Log
#endif



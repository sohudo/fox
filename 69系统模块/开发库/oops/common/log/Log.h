#ifndef _log_Log_H_
#define _log_Log_H_

class CFileLog
{
public:
	CFileLog();
	~CFileLog();
	void SetRoot(const TCHAR* format,...);
	void Close();
	void Log(const TCHAR* format,...);
private:

#ifdef LOG_FILE
	int		m_iFileCnt;
	FILE*	m_File;
	int		m_curYear;
	int		m_curMon;
	int		m_curDay;
	TCHAR	m_szRoot[MAX_PATH];
#endif
};

class CLog
{
public:
	CLog();
	~CLog();
	void Clear();
	void Print(const TCHAR* format,...);
	void LogToFile(const TCHAR* root, const TCHAR* format,...);
}; // Log

extern CLog g_Log;
#endif //_log_Log_H_

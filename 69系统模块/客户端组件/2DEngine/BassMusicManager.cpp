
#include "StdAfx.h"
#include "BassMusicManager.h"

#include "bass/bass.h"
#pragma comment(lib, "./bass/bass.lib")

// 单件模式类指针
CBassMusicManager* CBassMusicManager::ms_Instance = NULL;

std::map<int, tagMusicStream> CBassMusicManager::m_mapPlayingStream;
std::map<int, tagMusicStream> CBassMusicManager::m_mapNeedDeleteStream;

CBassMusicManager::CBassMusicManager()
{
	m_hWnd = NULL;
}

CBassMusicManager::~CBassMusicManager()
{
}

// 获取声音管理类的指针
CBassMusicManager* CBassMusicManager::GetInstance()
{
	if(ms_Instance == NULL)
	{
		ms_Instance = new CBassMusicManager();
	}

	return ms_Instance;		
}

// 释放资源
void CBassMusicManager::ReleaseAll()
{
	if(ms_Instance != NULL)
	{
		delete ms_Instance;
		ms_Instance = NULL;
	}
}

// 设置窗口句柄
bool CBassMusicManager::Init(HWND hWnd)
{
	if(ms_Instance == NULL)
		return false;

	if (m_hWnd != NULL) return false;

	m_hWnd = hWnd;
	return (BASS_Init(-1, 44100, 0, m_hWnd, NULL) == TRUE); 
}

// 从文件中加载声音
bool CBassMusicManager::LoadFromFile(const int nID, LPCTSTR lpszFileName, bool bLoop)
{
	if(ms_Instance == NULL)
	{
		return false;
	}

	HSTREAM hStream;

	if(bLoop)
	{
#ifdef _UNICODE
		hStream = BASS_StreamCreateFile(FALSE, (LPTSTR)lpszFileName, 0, 0, BASS_SAMPLE_LOOP|BASS_SAMPLE_FX|BASS_UNICODE);
#else
		hStream = BASS_StreamCreateFile(FALSE, (LPTSTR)lpszFileName, 0, 0, BASS_SAMPLE_LOOP|BASS_SAMPLE_FX);
#endif
	}
	else
	{
#ifdef _UNICODE
		hStream = BASS_StreamCreateFile(FALSE, (LPTSTR)lpszFileName, 0, 0, BASS_UNICODE);
#else
		hStream = BASS_StreamCreateFile(FALSE, (LPTSTR)lpszFileName, 0, 0, 0);
#endif
	}

	ASSERT(hStream != NULL);
	if(hStream == NULL)
	{
		return false;
	}

	tagMusicStream tagMs;
	tagMs.hStream = hStream;
	tagMs.bLoop = bLoop;

	m_mapPlayingStream.insert(std::pair<int, tagMusicStream>(nID, tagMs));
	
	return true;
}

// 从内存中加载声音
bool CBassMusicManager::LoadFromMemory(const int nID, void* pBuffer, DWORD dwLength, bool bLoop)
{
	if(ms_Instance == NULL)
	{
		return false;
	}

	HSTREAM hStream;

	if(bLoop)
	{
		hStream = BASS_StreamCreateFile(TRUE, pBuffer, 0, dwLength, BASS_SAMPLE_LOOP|BASS_SAMPLE_FX);
	}
	else
	{
		hStream = BASS_StreamCreateFile(TRUE, pBuffer, 0, dwLength, 0);
	}

	ASSERT(hStream != NULL);
	if(hStream == NULL)
	{
		return false;
	}

	tagMusicStream tagMs;
	tagMs.hStream = hStream;
	tagMs.bLoop = bLoop;

	m_mapPlayingStream.insert(std::pair<int, tagMusicStream>(nID, tagMs));

	return true;
}

// 从资源中加载声音
bool CBassMusicManager::LoadFromResource(const int nID, LPCTSTR lpszResourceName, HINSTANCE hInst, bool bLoop, LPCTSTR lpszResourceType)
{
	if(ms_Instance == NULL)
	{
		return false;
	}

	HRSRC hResInfo = FindResource(hInst, lpszResourceName, lpszResourceType);
	if (!hResInfo) return false;

	HGLOBAL hResDat = LoadResource(hInst, hResInfo);
	if (!hResDat) return false;

	PVOID pResBuffer = LockResource(hResDat);
	if (!pResBuffer) return false;

	DWORD dwResBuffer = SizeofResource(hInst, hResInfo);
	
	return LoadFromMemory(nID, pResBuffer, dwResBuffer, bLoop);
}

// 播放制定标志声音
bool CBassMusicManager::Play(const int nID, bool bDeleteWhenStop)
{
	return _Play(nID, false, bDeleteWhenStop);
}

//从头重新播放声音
bool CBassMusicManager::RePlay(const int nID, bool bDeleteWhenStop)
{
	return _Play(nID, true, bDeleteWhenStop);
}

// 播放声音
bool CBassMusicManager::_Play(const int nID, bool bReplay, bool bDeleteWhenStop)
{
	if(ms_Instance == NULL)
	{
		return false;
	}

	m_mapPlayingStream.find(nID);
	std::map<int, tagMusicStream>::iterator iter = m_mapPlayingStream.find(nID);

	if(iter == m_mapPlayingStream.end())
	{
		return false;
	}

	HSTREAM hStream = iter->second.hStream;
	bool bLoop = iter->second.bLoop;

	tagMusicStream tagMs;
	tagMs.bLoop = bLoop;
	tagMs.hStream = hStream;
	
	// 开始播放
	BASS_ChannelPlay(hStream, bReplay ? TRUE : FALSE);

	// 设置回调函数
  BASS_ChannelSetSync(hStream, BASS_SYNC_END, (QWORD)MAKELONG(10, 0), &CBassMusicManager::MySyncProc, 0);

	// 如果要求立即删除资源，则将添加到需要删除的容器中,同时从当前播放容器中删除
	if(bDeleteWhenStop)
	{
		m_mapNeedDeleteStream.insert(std::pair<int, tagMusicStream>(nID, tagMs));
		m_mapPlayingStream.erase(iter);
	}

	return true;
}

// 暂停
bool CBassMusicManager::Pause(const int nID)
{
	if(ms_Instance == NULL)
	{
		return false;
	}

	std::map<int, tagMusicStream>::iterator iter = m_mapNeedDeleteStream.find(nID);
	if(iter != m_mapNeedDeleteStream.end())
	{
		return (BASS_ChannelPause(iter->second.hStream) == TRUE);
	}
	else
	{
		iter = m_mapPlayingStream.find(nID);
		if(iter != m_mapPlayingStream.end())
		{
			return (BASS_ChannelPause(iter->second.hStream) == TRUE);
		}
	}

	return false;
	
}

// 停止
bool CBassMusicManager::Stop(const int nID)
{
	if(ms_Instance == NULL)
	{
		return false;
	}

	std::map<int, tagMusicStream>::iterator iter = m_mapNeedDeleteStream.find(nID);
	if(iter != m_mapNeedDeleteStream.end())
	{
		return (BASS_ChannelStop(iter->second.hStream) == TRUE);
	}
	else
	{
		iter = m_mapPlayingStream.find(nID);
		if(iter != m_mapPlayingStream.end())
		{
			return (BASS_ChannelStop(iter->second.hStream) == TRUE);
		}
	}

	return false;
}

// 是否在播放
bool CBassMusicManager::IsPlaying(int nUid)
{
	if(ms_Instance == NULL)
	{
		return false;
	}

	std::map<int, tagMusicStream>::iterator iter;
	iter = m_mapPlayingStream.find(nUid);
	if(iter != m_mapPlayingStream.end())
	{
		return (BASS_ChannelIsActive(iter->second.hStream) == BASS_ACTIVE_PLAYING);
	}
	else
	{
		iter = m_mapNeedDeleteStream.find(nUid);
		if(iter != m_mapNeedDeleteStream.end())
		{
			return (BASS_ChannelIsActive(iter->second.hStream)==BASS_ACTIVE_PLAYING);
		}
		else
		{
			return false;
		}
	}

	return false;
}

// 设置声音大小
bool CBassMusicManager::SetVolumn(int nVolumn)
{
	if(nVolumn < 0 || nVolumn > 100) return false;

	return (BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, nVolumn * 100) == TRUE);
}

//bool CBassMusicManager::SetVolumn(const int nID, int nVolumn)
//{
//	if(nVolumn < 0 || nVolumn > 100) return false;
//
//	std::map<int, tagMusicStream>::iterator iter = m_mapNeedDeleteStream.find(nID);
//	if(iter != m_mapNeedDeleteStream.end())
//	{
//		return (BASS_ChannelSetAttribute(iter->second.hStream, BASS_ATTRIB_VOL, (float)nVolumn / 100.f) == TRUE);
//	}
//	else
//	{
//		iter = m_mapPlayingStream.find(nID);
//		if(iter != m_mapPlayingStream.end())
//		{
//			return (BASS_ChannelSetAttribute(iter->second.hStream, BASS_ATTRIB_VOL, (float)nVolumn / 100.f) == TRUE);
//		}
//	}
//
//	return false;
//}

// 回调函数
void CALLBACK CBassMusicManager::MySyncProc(HSYNC handle, DWORD channel, DWORD data, DWORD user)
{
	std::map<int, tagMusicStream>::iterator iter = m_mapNeedDeleteStream.begin();
	for(; iter != m_mapNeedDeleteStream.end();)
	{
		if(BASS_ChannelIsActive(iter->second.hStream) == BASS_ACTIVE_STOPPED)
		{	
			if(BASS_StreamFree(iter->second.hStream) == TRUE)
			{
				m_mapNeedDeleteStream.erase(iter++);
			}
			else
			{
				++iter;
			}
		}
		else
		{
			++iter;
		}
	}
}
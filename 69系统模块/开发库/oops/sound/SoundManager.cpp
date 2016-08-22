#include "Stdafx.h"
#include <string>
#include <list>
#include <map>
#include <algorithm>
#include "./OpenAL/OpenAL.h"
#include "SndLoader.h"
#include "SndLoader.cpp"
#include "SoundManager.h"
#include <process.h>


using namespace std;

CSoundManager g_SoundManager;

//////////////////////////////////////////////////////////////////////////
class CLocker
{
public:
	CLocker(CRITICAL_SECTION* pSec)
		: m_pSec(pSec)
	{
		if (m_pSec)
		{
			EnterCriticalSection(m_pSec);
		}
	}
	~CLocker()
	{
		if (m_pSec)
		{
			LeaveCriticalSection(m_pSec);
			m_pSec = 0;
		}
	}
private:
	CRITICAL_SECTION* m_pSec;
};
//////////////////////////////////////////////////////////////////////////
struct CSoundManager::SData
{
	map<string, unsigned int>		mapSndBuffer;   // 文件对应的声音缓冲
	map<unsigned int, unsigned int>	mapSndSource;   // 声源对应播放次数
	list<unsigned int>              ltPlaying;		// 播放中的声音

	volatile bool			isRunning;		// 运行标志
	HANDLE					hThread;		// 线程句柄
	CRITICAL_SECTION        cs;				// 临界区

	SData():isRunning(false), hThread(NULL)
	{}
}; // CSoundManager::SData

CSoundManager::CSoundManager(void)
: m_pData(0)
{
	Init();
} // CSoundManager

CSoundManager::~CSoundManager(void)
{
	Deinit();
} // ~CSoundManager

void CSoundManager::Init()
{
	if (m_pData)
	{
		return;
	}

	if(!alutInit(NULL, 0))
	{
		printf("<CSoundManager::CSoundManager> 初始化失败: %s\n", alutGetErrorString(alutGetError()));
	}
	else
	{
		m_pData            = new SData();
		m_pData->isRunning = true;
		InitializeCriticalSection(&m_pData->cs);
		m_pData->hThread   = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunction, (void *)this, 0, 0);
	}
}

void CSoundManager::Deinit()
{
	if (m_pData == 0)
	{
		return;
	}

	// 等待线程退出
	m_pData->isRunning = false;

	if (m_pData->hThread)
	{
		Sleep(1000);
		WaitForSingleObject(m_pData->hThread, 1000);
		CloseHandle(m_pData->hThread);
		m_pData->hThread=NULL;
		DeleteCriticalSection(&m_pData->cs);	
	}

	m_pData->ltPlaying.clear();
	// 清理声源
	map<unsigned int, unsigned int>::iterator itSrc = m_pData->mapSndSource.begin();

	for (; itSrc != m_pData->mapSndSource.end(); ++itSrc)
	{
		alSourceStop(itSrc->first);
		alDeleteSources(1, &itSrc->first);
	}

	m_pData->mapSndSource.clear();

	// 清理缓冲
	map<string, unsigned int>::iterator itBuf = m_pData->mapSndBuffer.begin();

	for (; itBuf != m_pData->mapSndBuffer.end(); ++itBuf)
	{
		alDeleteBuffers(1, &itBuf->second);
	}

	m_pData->mapSndBuffer.clear();

	// 关闭al	
	if(!alutExit())
	{
		printf("<CSoundManager::CSoundManager> 退出失败: %s\n", alutGetErrorString(alutGetError()));
	}

	delete m_pData;
	m_pData = 0;
} // Release

unsigned int CSoundManager::CreateSound(const char* filename)
{
	ERR_RETURN_ZERO((m_pData == 0), "未初始化或初始化失败");
	CLocker locker(&m_pData->cs);

	unsigned int buffer = 0;

	map<string, unsigned int>::iterator it = m_pData->mapSndBuffer.find(filename);

	if (it != m_pData->mapSndBuffer.end())
	{
		buffer = it->second;
	}
	else
	{
		buffer = CSndLoader::Load(filename);

		if (buffer != 0)
		{
			m_pData->mapSndBuffer[filename] = buffer;
		}
	}

	if (buffer == 0)
	{
		return 0;
	}

	unsigned int source = 0;
	// 创建声音源
	alGenSources(1, &source);
	AL_ERR_RETURN_ZERO("创建声音源失败");
	// 绑定声音缓冲
	alSourcei(source, AL_BUFFER, buffer);

	ALenum eErr = alGetError();

	if (eErr != AL_NO_ERROR)
	{
		alDeleteSources(1, &source);
		source = 0;
		printf("<CSoundManager::CreateSound> 绑定声音缓冲失败 : %s\n", alutGetErrorString(eErr));
		return 0;
	}

	// 初始化声音相关, 默认播放一次
	m_pData->mapSndSource[source] = 1;
	return source;
} // CSoundManager

void CSoundManager::SetVolume(float volume)
{
	ERR_RETURN((m_pData == 0), "未初始化或初始化失败");

	if (volume > 1)
	{
		volume = 1;
	}
	else if (volume < 0)
	{
		volume = 0;
	}

	alListenerf(AL_GAIN, volume);
	AL_ERR_RETURN("设置全局音量失败");
} // SetVolume

void CSoundManager::SetListenerPosition(float x, float y, float z)
{
	ERR_RETURN((m_pData == 0), "未初始化或初始化失败");
	alListener3f(AL_POSITION, x, y, -z);
	AL_ERR_RETURN("设置监听者位置失败");
} // SetListenerPosition

void CSoundManager::SetListenerOrientation(float x, float y, float z)
{
	ERR_RETURN((m_pData == 0), "未初始化或初始化失败");
	alListener3f(AL_ORIENTATION, x, y, -z);
	AL_ERR_RETURN("设置监听者方向失败");
} // SetListenerOrientation

void CSoundManager::Update()
{
	if (!m_pData)
	{
		return;
	}

	CLocker locker(&m_pData->cs);
	list<unsigned int>::iterator itNext = m_pData->ltPlaying.begin();
	list<unsigned int>::iterator itCur;

	for (; itNext != m_pData->ltPlaying.end(); )
	{
		itCur = itNext++;
		unsigned int source = (unsigned int)*itCur;
		ALenum state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);

		//printf("state :%x\n", state);

		if (state == AL_STOPPED)
		{
			map<unsigned int, unsigned int>::iterator itSnd = m_pData->mapSndSource.find(source);

			if (itSnd->second > 0)
			{
				itSnd->second -= 1;

				if (itSnd->second == 0)
				{
					alDeleteSources(1, &itSnd->first);
					m_pData->mapSndSource.erase(itSnd);
					m_pData->ltPlaying.erase(itCur);
				}
				else
				{
					alSourcePlay(source);
				}
			}
		}
	}
} // Update

void CSoundManager::StopAll(int type)
{
	CLocker locker(&m_pData->cs);
	m_pData->ltPlaying.clear();
	// 清理声源
	map<unsigned int, unsigned int>::iterator itNext = m_pData->mapSndSource.begin();
	map<unsigned int, unsigned int>::iterator itCur;

	for (; itNext != m_pData->mapSndSource.end();)
	{
		itCur = itNext++;
		alSourceStop(itCur->first);

		if ((type == 0) || (itCur->second != 0))
		{
			alDeleteSources(1, &itCur->first);
			m_pData->mapSndSource.erase(itCur);
		}
	}
} // StopAll

void CSoundManager::SetVolume(unsigned int id, float volume)
{
	ERR_RETURN((m_pData == 0), "未初始化或初始化失败");
	ERR_RETURN((!alIsSource(id)), "找不到声音对象");

	if (volume > 1)
	{
		volume = 1;
	}
	else if (volume < 0)
	{
		volume = 0;
	}

	alSourcef(id, AL_GAIN, volume);
	AL_ERR_RETURN("设置音量失败");
} // SetVolume

void CSoundManager::SetPitch(unsigned int id, float pitch)
{
	ERR_RETURN((m_pData == 0), "未初始化或初始化失败");
	ERR_RETURN((!alIsSource(id)), "找不到声音对象");

	if (pitch > 1)
	{
		pitch = 1;
	}
	else if (pitch < 0)
	{
		pitch = 0;
	}

	pitch = pitch * 1.5f + 0.5f;
	alSourcef(id, AL_PITCH, pitch);
	AL_ERR_RETURN("设置音高失败");
} // SetPitch

void CSoundManager::SetRepeat(unsigned int id, unsigned int iRepeat)
{
	ERR_RETURN((m_pData == 0), "未初始化或初始化失败");
	CLocker locker(&m_pData->cs);
	map<unsigned int, unsigned int>::iterator it = m_pData->mapSndSource.find(id);
	ERR_RETURN((it == m_pData->mapSndSource.end()), "找不到声音对象");
	it->second = iRepeat;
	alSourcei(id, AL_LOOPING, iRepeat == 0 ? AL_TRUE : AL_FALSE);
	AL_ERR_RETURN("设置循环播放失败");
	UpdateToLoopList(id);
} // SetRepeat

void CSoundManager::SetPosition(unsigned int id, float x, float y, float z)
{
	ERR_RETURN((m_pData == 0), "未初始化或初始化失败");
	ERR_RETURN((!alIsSource(id)), "找不到声音对象");
	alSource3f(id, AL_POSITION, x, y, z);
	AL_ERR_RETURN("设置声源位置失败");
} // SetPosition

void CSoundManager::Play(unsigned int id)
{
	ERR_RETURN((m_pData == 0), "未初始化或初始化失败");
	ERR_RETURN((!alIsSource(id)), "找不到声音对象");
	alSourcePlay(id);
	AL_ERR_RETURN("播放失败");

	UpdateToLoopList(id);
} // Play

void CSoundManager::Pause(unsigned int id)
{
	ERR_RETURN((m_pData == 0), "未初始化或初始化失败");
	ERR_RETURN((!alIsSource(id)), "找不到声音对象");
	alSourcePause(id);
	UpdateToLoopList(id);
} // Pause

void CSoundManager::Stop(unsigned int id)
{
	ERR_RETURN((m_pData == 0), "未初始化或初始化失败");
	CLocker locker(&m_pData->cs);
	map<unsigned int, unsigned int>::iterator it = m_pData->mapSndSource.find(id);
	ERR_RETURN((it == m_pData->mapSndSource.end()), "找不到声音对象");
	alSourceStop(id);
	UpdateToLoopList(id);

	if (it->second !=0)
	{
		printf("删除声音对象 %d", it->first);
		alDeleteSources(1, &it->first);
		m_pData->mapSndSource.erase(it);
	}
} // Stop

/** 更新到循环列表中 */
void CSoundManager::UpdateToLoopList(unsigned int id)
{
	CLocker locker(&m_pData->cs);
	// 根据循环次数添加到次数循环列表中
	ALenum loop;
	ALenum state;
	alGetSourcei(id, AL_LOOPING, &loop);
	alGetSourcei(id, AL_SOURCE_STATE, &state);

	if (state == AL_PLAYING)
	{
		list<unsigned int>::iterator itP = find(m_pData->ltPlaying.begin(), m_pData->ltPlaying.end(), id);

		if (loop == AL_TRUE)
		{
			if (itP != m_pData->ltPlaying.end())
			{
				m_pData->ltPlaying.erase(itP);
			}
		}
		else
		{
			if (itP == m_pData->ltPlaying.end())
			{
				m_pData->ltPlaying.push_back(id);
			}
		}
	}

} // UpdateToLoopList


unsigned int __stdcall CSoundManager::ThreadFunction(void* pData)
{
	CSoundManager* pMgr = (CSoundManager*) pData;

	while (pMgr->m_pData->isRunning)
	{

		pMgr->Update();
		Sleep(300);
	}

	printf("<CSoundManager::ThreadFunction> 声音线程退出\n");
	return 0;
} // ThreadFunction
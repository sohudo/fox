#ifndef _SoundManager_H_
#define _SoundManager_H_

/**
 * 声音管理器
 */
class CSoundManager
	//: public CEventDispatcher
{
public:
	CSoundManager(void);
	~CSoundManager(void);
	/**
	 * 初始化
	 */
	void Init();
	/**
	 * 清除 
	 */
	void Deinit();
	/**
	 * 创建声音
	 * @param filename:文件路径
	 * @return 声音id 0:创建失败
	 */
	unsigned int CreateSound(const char* filename);
	/**
	 * 设置全局音量
	 * @param volume:音量值[0-1.0]
	 */
	void SetVolume(float volume);
	/**
	 * 设置听者位置(左手坐标系)
	 */
	void SetListenerPosition(float x, float y, float z);
	/**
	 * 设置听者方向(左手坐标系)
	 */
	void SetListenerOrientation(float x, float y, float z);
	/**
	 * 停止所有声音播放
	 * @param type: 0:并删除所有声音 1:只删除不是无限循环播放
	 */
	void StopAll(int type = 0);
	//////////////////////////////////////////////////////////////////////////
	/**
	 * 设置音量
	 * @param id:声音id
	 * @param volume:音量值[0-1.0]
	 */
	void SetVolume(unsigned int id, float volume);
	/**
	 * 设置音高
	 * @param pitch:音高值[0-1.0]
	 */
	void SetPitch(unsigned int id, float pitch);
	/**
	 * 设置是否循环播放
	 * @param id:声音id
	 * @param iRepeat:循环次数 0:表示无限循环
	 */
	void SetRepeat(unsigned int id, unsigned int iRepeat);
	/** 
	 * 设置位置(左手坐标系)
	 * @param id:声音id
	 * @param 
	 */
	void SetPosition(unsigned int id, float x, float y, float z);
	/** 
	 * 播放
	 * @param id:声音id
	 */
	void Play(unsigned int id);
	/**
	 * 暂停 
	 * @param id:声音id
	 */
	void Pause(unsigned int id);
	/** 
	 * 停止
	 * @param id:声音id
	 */
	void Stop(unsigned int id);
private:
	CSoundManager(const CSoundManager&);
	CSoundManager& operator=(const CSoundManager&);
	/** 更新到循环列表中 */
	void UpdateToLoopList(unsigned int id);
	/** 更新状态 */
	void Update();
	//线程函数
	static unsigned int __stdcall ThreadFunction(void* pData);
private:
	struct SData;
	SData* m_pData;
private:
	friend class CSoundChannel;
}; // CSoundManager

extern CSoundManager g_SoundManager;

#endif // _SoundManager_H_

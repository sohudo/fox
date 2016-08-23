
#ifndef BASS_MUSIC_MANAGER_H_
#define BASS_MUSIC_MANAGER_H_
#pragma once

#include <map>

//前置声明，避免包含bass.h
typedef DWORD HSTREAM;
typedef DWORD HSYNC;

// 声音资源结构体
typedef struct
{
	// HSTREAM
	HSTREAM hStream;
	// 是否循环播放, 注意：此参数不能与bDeleteWhenStop同时为true
	bool bLoop;
} tagMusicStream;

// 声音资源管理类
// 此类以单件模式实现的,可以管理程序中所有声音资源的播放、暂停等操作
class CBassMusicManager
{
public:
	// 析构函数
	~CBassMusicManager();

	// 获取声音管理类的指针
	static CBassMusicManager* GetInstance();

	// 设置窗口句柄
	bool Init(HWND hWnd);

	// 释放资源
	void ReleaseAll();
	
	// 从文件中加载声音
	// nID: 声音标志ID，根据此标志进行以后相应的操作，如播放，暂停等
	// lpszFileName: 文件路径
	// bLoop: 是否循环，默认:false
	bool LoadFromFile(const int nID, LPCTSTR lpszFileName, bool bLoop = false);

	// 从内存中加载声音
	// nID: 声音标志ID，根据此标志进行以后相应的操作，如播放，暂停等
	// pBuffer: 内存地址
	// dwLength: 长度
	// bLoop: 是否循环，默认:false
	bool LoadFromMemory(const int nID, void* pBuffer, DWORD dwLength, bool bLoop = false);

	// 从资源中加载声音
	// nID: 声音标志ID，根据此标志进行以后相应的操作，如播放，暂停等
	// lpszResourceName: 资源名
	// hInst: 资源句柄
	// lpszResourceType: 资源类型 默认:TEXT("Sound")
	// bLoop: 是否循环，默认:false
	bool LoadFromResource(const int nID, LPCTSTR lpszResourceName, HINSTANCE hInst, bool bLoop = false, LPCTSTR lpszResourceType = TEXT("Sound"));

	// 播放指定标志的声音
	// nID: 声音标志ID
	// bDeleteWhenStop: 播放完成后是否删除资源，默认false
	bool Play(const int nID, bool bDeleteWhenStop = false);

	// 从头重新播放指定标志的声音
	// nID: 声音标志ID
	// bDeleteWhenStop 播放完成后是否删除资源，默认false
	bool RePlay(const int nID, bool bDeleteWhenStop = false);

	// 暂停
	bool Pause(const int nID);

	// 停止
	bool Stop(const int nID);

	// 是否在播放
	bool IsPlaying(const int nID);

	// 设置声音大小
	// 声音大小，取值范围0~100
	bool SetVolumn(int nVolumn);
	//bool SetVolumn(const int nID, int nVolumn);

protected:
	// 回调函数
	static void CALLBACK MySyncProc(HSYNC handle, DWORD channel, DWORD data, DWORD user);

	bool _Play(const int nID, bool bReplay = false, bool bDeleteWhenStop = true);

protected:
	// 构造函数
	CBassMusicManager();

private:
	// HSTREAM容器
	static std::map<int, tagMusicStream> m_mapPlayingStream;	
	// 需要释放HSTREAM资源的容器
	static std::map<int, tagMusicStream> m_mapNeedDeleteStream;
	// 类指针
	static CBassMusicManager* ms_Instance;
	// 窗口指针
	HWND m_hWnd;
};

#endif // BASS_MUSIC_MANAGER_H_

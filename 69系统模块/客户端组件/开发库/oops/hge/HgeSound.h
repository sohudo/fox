#pragma once

#include "HgeHeader.h"

//HGE音效 支持的格式有WAV, MP3, MP2, MP1 和OGG。
class CHgeSound
{
	HEFFECT			m_Sound;			//声效句柄

	HCHANNEL		m_SoundChannel;		//活动音频通道句柄

	HGE				*m_hge;


 static	int			m_iVolume;			//当前音量

public:
	CHgeSound(void);
	~CHgeSound(void);

	//读取音乐路径
	void LoadSound(HGE *hge,char* sound);

	//播放声音
	void PlaySound();

	//播放声音EX   左右声到-100~100   播放速度默认1.0  是否循环播放
	void PlaySoundEX(int pan=0,float pitch=1.0f,bool loop=false);

	//设置音量
	void SetVolume(int volume);

	//获取音量
	int  GetVolume();

	//暂停
	void PauseSound();

	//恢复
	void ResumeSound();

	//停止
	void StopSound();

	//是否正在播放
	bool IsPlaying();

	//声音长度
	float GetLength();

	//设置播放位置
	void SetPos(float fPos);

	//当前播放位置
	float GetPos();

};

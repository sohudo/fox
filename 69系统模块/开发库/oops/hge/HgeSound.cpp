#include "Stdafx.h"
#include "hgesound.h"


int  CHgeSound::m_iVolume=100;

CHgeSound::CHgeSound(void)
{
}

CHgeSound::~CHgeSound(void)
{
}
//读取音乐路径
void CHgeSound::LoadSound(HGE *hge,char* sound)
{
	m_hge=hge;
	m_Sound=m_hge->Effect_Load(sound);
}

//播放声音
void CHgeSound::PlaySound()
{
	if (m_iVolume==0)
	{
		return;
	}

	m_SoundChannel=m_hge->Effect_Play(m_Sound);
	m_hge->Channel_SetVolume(m_SoundChannel,m_iVolume);
}

//播放声音EX     左右声到-100~100   播放速度默认1.0  是否循环播放
void CHgeSound::PlaySoundEX(int pan,float pitch,bool loop)
{
	m_SoundChannel=m_hge->Effect_PlayEx(m_Sound,m_iVolume,pan,pitch,loop);
}

//设置音量
void CHgeSound::SetVolume(int volume)
{
	m_iVolume=volume;

	m_hge->Channel_SetVolume(m_SoundChannel,volume);
}

//获取音量
int  CHgeSound::GetVolume()
{
	return m_iVolume;
}

//暂停
void CHgeSound::PauseSound()
{
	m_hge->Channel_Pause(m_SoundChannel);
}

//恢复
void CHgeSound::ResumeSound()
{
	m_hge->Channel_Resume(m_SoundChannel);
}

//停止
void CHgeSound::StopSound()
{
	m_hge->Channel_Stop(m_SoundChannel);
}

//是否正在播放
bool CHgeSound::IsPlaying()
{
	return m_hge->Channel_IsPlaying(m_SoundChannel);
}

//声音长度
float CHgeSound::GetLength()
{
	return m_hge->Channel_GetLength(m_SoundChannel);
}

//设置播放位置
void CHgeSound::SetPos(float fPos)
{
	return m_hge->Channel_SetPos(m_SoundChannel,fPos);
}

//当前播放位置
float CHgeSound::GetPos()
{
	return m_hge->Channel_GetPos(m_SoundChannel);
}
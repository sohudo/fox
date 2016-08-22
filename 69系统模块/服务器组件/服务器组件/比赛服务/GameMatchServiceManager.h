#ifndef GAME_MATCH_SERVICE_MANAGER_HEAD_FILE
#define GAME_MATCH_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "MatchServiceHead.h"
#include "ChampionshipMatch.h"

//////////////////////////////////////////////////////////////////////////
//定时器子项
struct tagMatchTimerItem
{
	//DWORD								dwTimerID;						//定时器 ID
	DWORD								dwRealID;						//实际ID
	DWORD								dwRepeatTimes;					//重复次数
	WPARAM								wBindParam;						//绑定参数
};

//比赛奖励
struct tagMatchReward
{
	DWORD								dwGold;							//金币奖励
	DWORD								dwMedal;						//奖牌奖励
	DWORD								dwExperience;					//经验奖励
};

class CChampionshipMatch;

//类说明
typedef CWHArray<IServerUserItem *> CMatchUserItemArray;
typedef CWHArray<BYTE> CMatchLoopTimerArray;
typedef CWHArray<CChampionshipMatch*>CMatchGroupArray;
typedef CWHArray<tagMatchReward*> CMatchRewardArray;
typedef CWHArray<tagMatchTimerItem *> CTimerItemPtr;

//游戏服务器管理类
class MATCH_SERVICE_CLASS CGameMatchServiceManager : public IGameMatchServiceManager, public IChampionshipMatchSink
{

	//变量定义
protected:
	CMatchUserItemArray					m_OnMatchUserItem;				//参赛用户
	tagGameServiceOption *				m_pGameServiceOption;			//服务配置
	const tagGameServiceAttrib *		m_pGameServiceAttrib;			//服务属性
	CMatchLoopTimerArray				m_LoopTimer;					//循环定时器 最多同时9组比赛
	//接口变量
protected:
	ITCPNetworkEngineEvent *			m_pITCPNetworkEngineEvent;		//网络接口
	IDataBaseEngine *					m_pIDataBaseEngine;				//数据引擎
	ITimerEngine *						m_pITimerEngine;				//时间引擎
	IMainServiceFrame *					m_pIGameServiceFrame;			//功能接口
	IServerUserItemSink *				m_pIServerUserItem;				//
	IServerUserManager *				m_pIServerUserManager;			//用户管理
	ITableFrame	**						m_ppITableFrame;				//框架接口
	IAndroidUserManager					*m_pAndroidUserManager;			//机器管理
	

	CMatchGroupArray					m_MatchGroup;
	CMatchGroupArray					m_OverMatchGroup;				//已经结束比赛组
	CChampionshipMatch *				m_pCurMatch;
	tagMatchRule						m_MatchRule;

	CMD_GR_MatchDesc					m_MatchDesc;					//信息描述
	CMatchRewardArray					m_RewardArr;					//奖励

	CTimerItemPtr						m_TimerItemFree;				//空闲数组
	CTimerItemPtr						m_TimerItemActive;				//活动数组
	DWORD								m_TimerMatchStartNO;			//定时賽的定时器是否开启 也是当前比赛场次

	
	
	//函数定义
public:
	//构造函数
	CGameMatchServiceManager(void);
	//析构函数
	virtual ~CGameMatchServiceManager(void);

	//基础接口
public:
	//释放对象
	virtual VOID  Release() { }
	//接口查询
	virtual VOID *  QueryInterface(const IID & Guid, DWORD dwQueryVer);
	//控制接口
public:
	//停止服务
	virtual bool  StopService();
	//启动服务
	virtual bool  StartService();
	//管理接口
public:
	//初始化接口
	virtual bool  InitMatchInterface(ITCPNetworkEngineEvent *pTCPNetworkEngine,IDataBaseEngine *pIDataBaseEngine, IServerUserManager *pIServerUserManager,
		IMainServiceFrame *pIMainServiceFrame,IServerUserItemSink *pIServerUserItem,ITimerEngine *pITimerEngine,IAndroidUserManager *pAndroidUserManager);
	//初始化桌子框架
	virtual bool  InitTableFrame(ITableFrame * pTableFrame,WORD wTableID);
	
	//系统事件
public:
	//时间事件
	virtual bool  OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//参数接口
public:
	//修改参数
	virtual bool  RectifyServiceOption(tagGameServiceOption * pGameServiceOption,tagGameServiceAttrib *pGameServiceAttrib);
	//信息接口
public:
	//用户参加比赛
	virtual bool  OnUserJoinGame(IServerUserItem * pIServerUserItem, BYTE cbReason);
	//用户退出比赛
	virtual bool  OnUserQuitGame(IServerUserItem * pIServerUserItem, BYTE cbReason,WORD *pBestRank=NULL, DWORD dwContextID=INVALID_WORD);
	//用户比赛成绩
	virtual bool  OnUserMatchResult(DWORD dwUserID, WORD wKindID, WORD wServerID, WORD wSortID);
	//获取比赛信息
	virtual DWORD  OnGetGameMatchNO(){return m_TimerMatchStartNO;};
	//创建接口
public:
	//建立比赛模式
	virtual VOID *  CreateGameMatchSink(REFGUID Guid, DWORD dwQueryVer);
	//网络接口
public:
	//发送数据
	virtual bool  SendDataToServer(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool SendMatchInfo(IServerUserItem * pIServerUserItem);
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);
	//命令消息
	virtual bool OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);
	//进入消息
	virtual bool OnEventEnterMatch(DWORD dwSocketID,VOID* pData,DWORD dwUserIP);

	//内部功能
private:
	//删除用户
	bool DeleteUserItem(DWORD dwUserIndex);
	//发送消息
	bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType);
	//读取比赛规则
	bool ReadMatchOption(LPCTSTR lpFileName);
	//检查是否已经逃跑的用户
	bool CheckRunWayUser(CChampionshipMatch *pMatch,DWORD dwUserID);

public:
	//报名人满，开始比赛
	virtual bool  OnMatchStart(CChampionshipMatch *pMatch);
	//等待超时
	virtual bool  OnWaitTimeOut(DWORD dwMatchNO);
	//用户淘汰
	virtual bool  OnUserOut(IServerUserItem *pUserItem,LPCTSTR szReason,WORD wLen,WORD wRank,CChampionshipMatch *pMatch);
	//发送数据
	virtual bool  SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//定时器
	virtual bool  SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter,CChampionshipMatch *pMatch);
	//定时器
	virtual bool  KillGameTimer(DWORD dwTimerID,CChampionshipMatch *pMatch);
	//发送消息
	virtual bool  SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType);
	//写入奖励
	virtual bool  WriteUserAward(IServerUserItem *pIServerUserItem, WORD wRank,DWORD dwMatchNO,LPCTSTR szDescribe,CChampionshipMatch *pMatch);
	//比赛结束
	virtual bool  OnEventMatchOver(CChampionshipMatch *pMatch);
	//发送用户分数
	virtual bool  SendGroupUserScore(IServerUserItem * pIServerUserItem,CChampionshipMatch *pMatch);
	//发送状态
	virtual bool  SendGroupUserStatus(IServerUserItem * pIServerUserItem,CChampionshipMatch *pMatch);
	//发送信息
	virtual bool  SendGroupUserMessage(LPCTSTR pStrMessage,CChampionshipMatch *pMatch);
	//发送数据到服务器 
	virtual bool  SendDataToGameServer(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据到一组用户 
	virtual bool  SendGroupData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize,CChampionshipMatch *pMatch);
	//获取一个空闲的机器人
	virtual IAndroidUserItem * GetFreeAndroidUserItem();
	//设置替补机器人
	virtual bool SetReplaceAndroidItem(IServerUserItem *pIServerUserItem);
	//插入空的桌子
	virtual void InsertNullTable();
	//获取空的桌子
	virtual WORD GetNullTable();


};

//////////////////////////////////////////////////////////////////////////
//组件创建

//////////////////////////////////////////////////////////////////////////

#endif
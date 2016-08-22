#ifndef CHAMPIONSHIP_MATCH_HEAD_FILE
#define CHAMPIONSHIP_MATCH_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameMatchSink.h"
#include "MatchServiceHead.h"

typedef CMap<DWORD,DWORD,IServerUserItem *, IServerUserItem *>CUserItemMap;
typedef CMap<IServerUserItem *, IServerUserItem *, DWORD, DWORD>CUserSeatMap;

#define IDI_TURN_MATCH_START		(IDI_MATCH_MODULE_START+1)
#define IDI_START_MATCH_GAME		(IDI_MATCH_MODULE_START+3)					//开始一盘比赛  
#define IDI_DELETE_OVER_MATCH		(IDI_MATCH_MODULE_START+5)					//删除结束的比赛组
#define IDI_NORMAL_TIMER			(IDI_MATCH_MODULE_START+6)					//常规定时器
#define IDI_START_MATCH_START		(IDI_MATCH_MODULE_START+7)					//开始一场比赛 
#define IDI_CHECK_OVER_MATCH		(IDI_MATCH_MODULE_START+8)					//检查一场比赛 
//#define IDI_START_MATCH_END			(IDI_MATCH_MODULE_START+9)					//开始一场比赛 
#define IDI_HALF_MINUTE				(IDI_MATCH_MODULE_START+9)					//30秒
#define IDI_GROUP_START				(IDI_MATCH_MODULE_START+10)					//比赛组内的定时器

//////////////////////////////////////////每组比赛定时器不超过9个///////////////////////////////////////
#define IDI_CHECK_MATCH_GROUP		(IDI_GROUP_START+1)							//轮巡所有开赛比赛组 
#define IDI_WAITTABLE_FIRST_ROUND	(IDI_GROUP_START+2)							//等待配桌 
#define IDI_WAITTABLE_RAND_TABLE	(IDI_GROUP_START+3)							//等待随机分配 
#define IDI_SWITCH_WAIT_TIME		(IDI_GROUP_START+4)							//换桌等待时间 
#define IDI_AllDismissGame			(IDI_GROUP_START+5)							//全部解散游戏
#define IDI_LASTMATCHRESULT			(IDI_GROUP_START+6)							//结束比赛
#define IDI_EnterNextRound			(IDI_GROUP_START+7)							//进入下一个循环
#define IDI_TabelRequestUser		(IDI_GROUP_START+8)							//自动分配桌子

/////////////////////////////////////////每组比赛专用定时器////////////////////////////////////////////
#define IDI_CONTINUE_GAME			1											//继续游戏
#define IDI_EXIT_TABLE				2											//离开桌子
#define IDI_CheakTabelStart			3											//检查分配桌子是否开始(解决有可能准备后不开始情况)

#define IDI_LAND_FIRST_END			10											//斗地主预赛结束
//第一轮规则
enum enFirstRoundRule
{
	FirstRoundRule_Score,					//打立出局
	FirstRoundRule_Time,					//定时排名
	TimerRoundRule							//定时賽 固定时间进入离开
};

struct tagMatchTimer
{
	DWORD					dwTimerID;						//定时器ID
	int						iElapse;						//执行间隔秒
	//DWORD					dwRepeat;						//执行次数
	WPARAM					wParam;							//附加参数
	LPARAM					lParam;							//附加参数
};

//////enum enMatchUserState
//////{
//////	MS_UserMatching,						//比赛中
//////	MS_UserPromotion,						//升级状态
//////	MS_UserMaybeOut							//不确定淘汰
//////};
//////struct tagMatchUser
//////{
//////	enMatchUserState		enUserState;			//比赛用户状态
//////	IServerUserItem *		pMahchUserItem;			//比赛的用户
//////};
//比赛时间模式
struct tagMatchTimeRule
{
	BYTE				cbStartHour;						//开始时间（小时）
	BYTE				cbStartMinute;						//开始时间（分）
	BYTE				cbEndHour;							//结束时间（小时）
	BYTE				cbEndMinute;						//结束时间（分）

	WORD				wMaxGameCount;						//最大游戏局数
	WORD				wMinGameCount;						//最小游戏局数
	BYTE				cbOutRoomCount;						//退出房间次数
};
struct tagMatchRule
{
	DWORD				dwMatchID;							//比赛ID
	TCHAR				szMatchName[LEN_SERVER];			//比赛名称
	BYTE				cbMatchEndTimer;					//一场比赛的结束时间
	WORD				wUserCount;							//开赛人数
	WORD				wAndroidUserCount;					//一场比赛中机器人报名的数量
	WORD				wWaitTime;							//达到淘汰人数后等待多久确定晋级人数，单位：秒
	DWORD				dwInitalBase;						//初始基数
	BYTE				cbFixedBase;						//是否固定基注 如果固定 初始基注不会自增
	BYTE				dwGrowthTime;						//增长时间
	BYTE				cbGrowthRange;						//增长幅度
	DWORD				dwFee;								//报名费
	DWORD				dwInitalScore;						//初始积分=0时 代表使用当前金币为比赛
	WORD				wEnterBase;							//初始积分=0时 必须多少倍金币才有报名条件
	WORD				wLeaveBase;							//淘汰时候低于多少倍被淘汰
	WORD				wRemainUserCount;					//剩余多少人停止淘汰
	DWORD				dwSecRoundUserCount;				//进入下一轮剩下的人数
	BYTE				cbLeastScore;						//最少积分
	BYTE				cbFirScoreRule;						//进入复赛的带分规则
	BYTE				cbFirGameCount;						//初赛N局
	BYTE				cbSecGameCount;						//复赛N局
	BYTE				cbSecSwitchTableCount;				//复赛N局自动换桌(0为不换桌，大于或者等于SecGameCount也相当于不换桌)
	WORD				wSecSwitchTableTime;				//自动换桌等待其他桌子时间单位秒 最低5秒 最高300秒 如果到达设置时间将不等待其他桌子
	BYTE				cbSecScoreRule;						//复赛带分规则(0为每轮每桌最后一局算分第1名晋级,1为所有桌混合排名晋级)
	BYTE				cbFirRoundCount;					//初赛阶段游戏局数
	BYTE				cbSecRoundCount;					//复赛阶段游戏轮次
	WORD				arrSecRoundUserCount[16];			//复赛阶段各轮游戏人数
	WORD				arrSecRoundExpand[16];				//复赛阶段各轮一个扩展值
	enFirstRoundRule	enFirstRule;						//第一轮规则
	DWORD				dwFirstRoundTime;					//第一轮时长，定时排名有效，单位：秒
	DWORD				dwAheadStatistics;					//提前统计成绩 单位：秒
	WORD				wPrecedeTimer;						//大于这个值可优先坐下
	tagMatchTimeRule	sTimerRule;							//时间模式规则
	tagMatchRule()
	{
		ZeroMemory(this, sizeof(tagMatchRule));
	}
};

#ifdef _UNICODE
#define VER_IChampionshipMatch INTERFACE_VERSION(1,1)
static const GUID IID_IChampionshipMatch={0x786c5501,0xa7a4,0x4531,0x0091,0xc5,0x3e,0x07,0x3e,0xa6,0x0a,0xf9};
#else
#define VER_IChampionshipMatch INTERFACE_VERSION(1,1)
static const GUID IID_IChampionshipMatch={0x1a4e0406,0x8ddc,0x4e34,0x0083,0x89,0xb5,0x5e,0x4e,0xfa,0x6b,0xb7};
#endif

interface IChampionshipMatch :public IUnknownEx
{
	//用户报名
	virtual bool OnUserSignUp(IServerUserItem *pUserItem)=NULL;
	//用户退赛
	virtual bool OnUserQuitMatch(IServerUserItem *pUserItem,bool bByUser=false)=NULL;
	//分配桌子
	virtual void SetMatchTable(ITableFrame *pTable[], WORD wTableCount)=NULL;
};

#ifdef _UNICODE
#define VER_IChampionshipMatchSink INTERFACE_VERSION(1,1)
static const GUID IID_IChampionshipMatchSink={0x30e4794a,0x610e,0x4873,0x00bd,0x23,0x9c,0xc7,0x2c,0xbe,0xd4,0xc5};
#else
#define VER_IChampionshipMatchSink INTERFACE_VERSION(1,1)
static const GUID IID_IChampionshipMatchSink={0xfc8dc7b5,0x2318,0x411f,0x00b9,0x09,0x6c,0xdd,0x39,0x38,0x0d,0x18};
#endif

class CChampionshipMatch;

interface IChampionshipMatchSink:public IUnknownEx
{
	//报名人满，开始比赛
	virtual bool  OnMatchStart(CChampionshipMatch *pMatch)=NULL;
	//等待超时
	virtual bool  OnWaitTimeOut(DWORD dwMatchNO)=NULL;
	//用户淘汰
	virtual bool  OnUserOut(IServerUserItem *pUserItem,LPCTSTR szReason,WORD wLen,WORD wRank,CChampionshipMatch *pMatch)=NULL;
	//发送数据
	virtual bool  SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//定时器
	virtual bool  SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter,CChampionshipMatch *pMatch)=NULL;
	//定时器
	virtual bool  KillGameTimer(DWORD dwTimerID,CChampionshipMatch *pMatch)=NULL;
	//发送消息
	virtual bool  SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;
	//写入奖励
	virtual bool  WriteUserAward(IServerUserItem *pIServerUserItem, WORD wRank,DWORD dwMatchNO,LPCTSTR szDescribe,CChampionshipMatch *pMatch)=NULL;
	//比赛结束
	virtual bool  OnEventMatchOver(CChampionshipMatch *pMatch)=NULL;
	//发送用户分数
	virtual bool  SendGroupUserScore(IServerUserItem * pIServerUserItem,CChampionshipMatch *pMatch)=NULL;
	//发送状态
	virtual bool  SendGroupUserStatus(IServerUserItem * pIServerUserItem,CChampionshipMatch *pMatch)=NULL;
	//发送信息
	virtual bool  SendGroupUserMessage(LPCTSTR pStrMessage,CChampionshipMatch *pMatch)=NULL;
	//发送数据到服务器 
	virtual bool  SendDataToGameServer(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//发送数据到一组用户
	virtual bool  SendGroupData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize,CChampionshipMatch *pMatch)=NULL;
	//获取一个空闲的机器人
	virtual IAndroidUserItem * GetFreeAndroidUserItem()=NULL;
	//设置替补机器人
	virtual bool SetReplaceAndroidItem(IServerUserItem *pIServerUserItem)=NULL;
	//发送数据
	virtual bool SendMatchInfo(IServerUserItem * pIServerUserItem)=NULL;
	//插入空的桌子
	virtual void InsertNullTable()=NULL;

};
typedef CWHArray<IServerUserItem *> CMatchUserItemArray;

struct ITableFrameEx 
{
	ITableFrame		*pTableFrame;
	DWORD			dwBase;
	WORD			wTableID;
	BYTE			cbStartTimes;
	BYTE			cbGameCount;
	BOOL			bRoundTableFinish;
	BOOL			bSwtichTableFinish;
	CMatchUserItemArray		m_FrontUserArray;				//上一次做过该桌的用户
};
typedef CWHArray<ITableFrameEx *>CTableFrameMananerArray;
typedef CWHArray<IServerUserItem *> CMatchUserItemArray;
typedef CWHArray<tagMatchTimer *> CUserMatchTimerArray;

struct tagMatchScore
{
	DWORD			dwUserID;
	IServerUserItem *pUserItem;
	SCORE			lScore;
};
struct tagMatchRanking
{
	DWORD			dwUserID;
	IServerUserItem *pUserItem;
	SCORE			lScore;
	LONG			lExpand;					//扩展值
};
typedef CWHArray<tagMatchScore *> CUserMatchScoreArray;
//////typedef CWHArray<tagMatchUser *> CMatchUserItemArrayEX;
//比赛状态
enum enMatchStatus
{
	MS_NULL_STATUS=0,		//无状态
	MS_SIGNUPING,			//报名中
	MS_WAITTABLE,			//没有桌子预赛 等待桌子
	MS_STOP_OUT,
	MS_FIRST_ROUND,
	MS_START_NEXT_ROUND,
	MS_SECOND_ROUND
};

class CChampionshipMatch:public IGameEventSink
{
	friend class CGameMatchServiceManager;
public:
	CWHArray<DWORD>						m_RunWayArray;					//逃跑ID 逃跑的时候记录 防止重复写分
protected:
	static WORD						m_wChairCount;					//椅子数目
	DWORD							m_dwMatchNO;
	CUserItemMap					m_OnMatchUserMap;				//参赛用户
	//////CMatchUserItemArrayEX			m_MatchUserArray;				//参数用户

	CMatchUserItemArray				m_FreeUserArray;				//空闲未淘汰的用户
	CMatchUserItemArray				m_ReOutUserArray;				//空闲即将淘汰的用户

	DWORD							m_dwCurBase;					// 当前的基数
	tagMatchRule					m_pMatchRule;
	IChampionshipMatchSink *		m_pMatchSink;

	enMatchStatus					m_enMatchStatus;				//比赛阶段
	
	CTableFrameMananerArray			m_MatchTableArray;				//预定比赛桌子
	CUserMatchTimerArray			m_MatchTimerArray;				//比赛专用定时器

	CTime							m_tMatchStart;					//开始时间
	DWORD							m_dwTimeCount;					//时间标记
	WORD							m_wCurRountSec;					//当前正处于复赛的哪阶段

	bool							m_bIsContinue;					//是否继续比赛

	BYTE							m_LoopTimer;					//循环定时器

	WORD							m_wAndroidUserCount;			//机器人个数
	DWORD							m_dwMatchTimeCount;				//比赛计时器(秒)
	
public:
	CChampionshipMatch(tagMatchRule &pMatchRule, DWORD dwMatchNO,IChampionshipMatchSink *pSink);
	virtual ~CChampionshipMatch(void);

public:
 	//释放对象
 	virtual VOID  Release(){}
 	//接口查询
	virtual VOID *  QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	//设置回调接口
	 bool SetMatchSink(IChampionshipMatchSink *pSink);
	//用户报名
	 bool OnUserSignUp(IServerUserItem *pUserItem);
	//用户退赛
	 BOOL OnUserQuitMatch(IServerUserItem *pUserItem,bool bByUser=false);
	//分配桌子
	 void SetMatchTable(ITableFrame *pTable[], WORD wTableCount);
	 //定时器
	 bool OnTimeMessage(DWORD dwTimerID, WPARAM dwBindParameter);

	 //游戏开始
	 virtual bool  OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount);
	 //游戏结束
	 virtual bool  OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	 //是否可以离开座位 
	 virtual bool  IsCanStanUp(IServerUserItem *pUserItem);
	 //玩家断线或重连 
	 virtual void  SetUserOffline(WORD wTableID,IServerUserItem *pUserItem, bool bOffline);
	 //用户坐下
	 virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //用户起来
	 virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //用户同意
	 virtual bool OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

protected:
	//选择晋级玩家
	void SelectPromotionUser();
	//玩家排序
	WORD SortMapUser(tagMatchScore score[]);
	//将玩家排序
	WORD SortUserRanking(tagMatchRanking MatchRanking[],bool bIsLand);
	//所有玩家起立
	void RearrangeUserSeat(bool bDismissGame=true);
	//获取名次
	WORD GetUserRank(IServerUserItem *pUserItem, ITableFrame *pITableFrame=NULL);
	//获取名次
	WORD GetUserRankMJ(IServerUserItem *pUserItem, ITableFrame *pITableFrame=NULL);
	//获取用户
	IServerUserItem * GetSeatUserItem(DWORD dwSeatID);
	//定时排名游戏结束
	void FirstRoundRuleTimeGameEnd(ITableFrameEx *pITableFrameEx);
	//打立出局游戏结束
	void FirstRoundRuleScoreGameEnd(ITableFrame *pITableFrame);
    //完成一轮
	bool OnTableFinishRound(ITableFrame *pITableFrame);
	//完成一轮
	bool OnTableFinishRoundMJ(ITableFrame *pITableFrame);
	//效验比赛开始
	void EfficacyStartMatch();
	//比赛结束
	void MatchOver();

	//发送提示
	VOID SendWaitTip(ITableFrame *pTableFrame);
	//发送提示
	VOID SendWaitTipMJ(ITableFrame *pTableFrame);
	//加入比赛桌子
	void AddMatchTable(ITableFrameEx* ITable){m_MatchTableArray.Add(ITable);}

	//获取桌子专用接口
	ITableFrameEx* GetTableInterface(ITableFrame* ITable);

	//空闲桌子拉空闲用户
	void TabelRequestUser(bool bPrecedeSit=true);
	//发送一组信息
	void SendGroupBox(LPCTSTR pStrMessage);
	//设置晋级用户和淘汰用户 每桌第一名优先排名
	void SetEnterUserOROutUser(ITableFrame *pITableFrame);
	//设置晋级用户和淘汰用户 所有桌混合排名
	void SetEnterUserOROutUser();
	//设置晋级用户 2人麻将比赛
	void SetEnterUserMJ();
	//设置晋级用户 斗地主比赛
	void SetEnterUserLand();
	//设置桌子完成标记
	void SetRoundTableFinish(ITableFrame *pITableFrame);
	//检测桌子完成标记
	bool ChackRoundTableFinish();
	//设置桌子完成标记
	void SetSwitchTableFinish(ITableFrame *pITableFrame);
	//设置桌子完成标记
	bool CheckSwitchTableFinish();
	//设置所有用户离开
	void AllUserExitTable(ITableFrame *pITableFrame);
	//本桌继续游戏
	void ContinueGame(ITableFrame *pITableFrame);
	//检查桌子是否开始游戏
	void CheakTableStart(ITableFrame *pITableFrame);
	//继续下一轮
	void ContinueRound();
	//最后结算
	void LastMatchResult();
	//检测参赛人数
	bool CheckMatchUser();
	//检查停止淘汰
	void CheckMatchStopOut(ITableFrame *pITableFrame);
	//检测退賽
	void CheckExitMatch();
	//获取优先坐下用户
	WORD GetPrecedeSitUser(DWORD dwCurTime);
	//实现该函数可保证Free组唯一
	void InsertFreeGroup(IServerUserItem *pUserItem);

	//比赛定时器 带参数定时器 必须用专用比赛定时器 不然多桌同时结束会抵消上一个定时器 出现偶然性BUG
protected:
	//捕获比赛专用定时器
	void CaptureMatchTimer();
	//投递比赛专用定时器
	void PostMatchTimer(DWORD dwTimerID, int iElapse, WPARAM wParam=0, LPARAM lParam=0);
	//杀死比赛专用定时器
	bool KillMatchTimer(DWORD dwTimerID, WPARAM wParam);
	//杀死比赛专用定时器
	bool KillMatchTimer(INT_PTR dwIndexID);
	//全部杀死比赛专用定时器
	void AllKillMatchTimer();

	//设定比赛定时器
	void SetMatchGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
	//杀死比赛定时器
	void KillMatchGameTimer(DWORD dwTimerID);
};


#endif
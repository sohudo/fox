#include "StdAfx.h"
#include "AfxTempl.h"
#include "GameMatchSink.h"
#include "GameMatchServiceManager.h"

#include "..\游戏服务器\DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameMatchServiceManager::CGameMatchServiceManager(void)
{
	m_pIGameServiceFrame=NULL;
	m_pITCPNetworkEngineEvent=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITimerEngine=NULL;
	m_pIServerUserManager=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;
	m_ppITableFrame=NULL;
	m_pAndroidUserManager=NULL;
	m_RewardArr.RemoveAll();
	//////m_wMatchIDCount=0;
	m_pCurMatch=NULL;
	m_TimerMatchStartNO=0;
	//////m_dwStartTimeID=0;
	ZeroMemory(&m_MatchDesc,sizeof(m_MatchDesc));
	ZeroMemory(&m_MatchRule,sizeof(m_MatchRule));
	return;
}

//析构函数
CGameMatchServiceManager::~CGameMatchServiceManager(void)
{
	m_RewardArr.RemoveAll();
	SafeDeleteArray(m_ppITableFrame);


	INT_PTR nCount=m_TimerItemFree.GetCount();
	for (INT_PTR i=0;i<nCount;i++)
		SafeDelete(m_TimerItemFree[i]);
	m_TimerItemFree.RemoveAll();

	nCount=m_TimerItemActive.GetCount();
	for (INT_PTR i=0;i<nCount;i++)
		SafeDelete(m_TimerItemActive[i]);
	m_TimerItemActive.RemoveAll();

}
//停止服务
bool  CGameMatchServiceManager::StopService()
{
	INT_PTR nCount=m_RewardArr.GetCount();
	for (INT_PTR i=0;i<nCount;i++)
		SafeDelete(m_RewardArr[i]);
	m_RewardArr.RemoveAll();

	if(m_MatchRule.enFirstRule==TimerRoundRule)return true;

	SafeDeleteArray(m_ppITableFrame);
	
	m_LoopTimer.RemoveAll();

	nCount=m_TimerItemFree.GetCount();
	for (INT_PTR i=0;i<nCount;i++)
		SafeDelete(m_TimerItemFree[i]);
	m_TimerItemFree.RemoveAll();

	nCount=m_TimerItemActive.GetCount();
	for (INT_PTR i=0;i<nCount;i++)
		SafeDelete(m_TimerItemActive[i]);
	m_TimerItemActive.RemoveAll();
	return true;
}
//启动服务
bool  CGameMatchServiceManager::StartService()
{
	if(m_MatchRule.enFirstRule==TimerRoundRule)return true;
	//最多同时9组编号
	for (int i=0;i<79;i++)
	{
		m_LoopTimer.Add(i);
	}
	m_pITimerEngine->SetTimer(IDI_CHECK_OVER_MATCH,60000L,TIMES_INFINITY,0);
	m_pITimerEngine->SetTimer(IDI_DELETE_OVER_MATCH, 6000,TIMES_INFINITY,0);
#ifdef _DEBUG
	m_pITimerEngine->SetTimer(IDI_HALF_MINUTE,100,TIMES_INFINITY,0);
#else
	m_pITimerEngine->SetTimer(IDI_HALF_MINUTE,3000,TIMES_INFINITY,0);
#endif


	InsertNullTable();
	return true;
}
//接口查询
void *  CGameMatchServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameMatchServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IChampionshipMatchSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameMatchServiceManager,Guid,dwQueryVer);
	return NULL;
}

//初始化接口
bool  CGameMatchServiceManager::InitMatchInterface(ITCPNetworkEngineEvent *pTCPNetworkEngine,IDataBaseEngine *pIDataBaseEngine, IServerUserManager *pIServerUserManager,
								 IMainServiceFrame *pIMainServiceFrame,IServerUserItemSink *pIServerUserItem,ITimerEngine *pITimerEngine,IAndroidUserManager *pAndroidUserManager)
{
	ASSERT(pTCPNetworkEngine!=NULL && pIDataBaseEngine!=NULL && pIServerUserManager!=NULL && pIMainServiceFrame!=NULL && pITimerEngine!=NULL);
	if(pTCPNetworkEngine==NULL || pIDataBaseEngine==NULL || pIServerUserManager==NULL || pIMainServiceFrame==NULL || pITimerEngine==NULL) return false;

	m_pITCPNetworkEngineEvent=pTCPNetworkEngine;
	m_pIDataBaseEngine=pIDataBaseEngine;
	m_pIServerUserManager=pIServerUserManager;
	m_pIGameServiceFrame=(IMainServiceFrame *)pIMainServiceFrame;
	m_pIServerUserItem=(IServerUserItemSink *)pIServerUserItem;
	m_pITimerEngine=pITimerEngine;
	m_pAndroidUserManager=pAndroidUserManager;

	return true;
}

//用户参加比赛
bool  CGameMatchServiceManager::OnUserJoinGame(IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	if(m_MatchRule.enFirstRule==TimerRoundRule) return false;

	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL) return true;


	if(CServerRule::IsForfendRoomEnter(m_pGameServiceOption->dwServerRule))
	{
		SendRoomMessage(pIServerUserItem, TEXT("抱歉，由于系统维护，该房间禁止用户报名比赛！。"),SMT_CHAT|SMT_EJECT);
		return true;
	}
	WORD wTableCount=GetNullTable();
	if(wTableCount<m_pCurMatch->m_pMatchRule.wUserCount/CChampionshipMatch::m_wChairCount)
	{
		CString str;
		str.Format( TEXT("抱歉，当前桌子不够请等待。还差 %d 张桌子可以报名"),m_pCurMatch->m_pMatchRule.wUserCount/CChampionshipMatch::m_wChairCount-wTableCount);
		SendRoomMessage(pIServerUserItem,str,SMT_CHAT);
		return true;
	}
	if(m_pCurMatch==NULL || m_pCurMatch->m_enMatchStatus==MS_WAITTABLE)
	{
		SendRoomMessage(pIServerUserItem, TEXT("抱歉，该房间参赛人数已满，请稍后再报名或者进入另一房间比赛。"),SMT_CHAT);
		return true;
	}
	INT_PTR dwUserCount=m_OnMatchUserItem.GetCount();
	for (INT_PTR i=0;i<dwUserCount;i++)
	{
		if(m_OnMatchUserItem[i]==pIServerUserItem)
		{
			SendRoomMessage(pIServerUserItem, TEXT("您已经成功报名，不能重复报名！"),SMT_CHAT);
			return true;
		}
	}
	if(m_LoopTimer.GetCount()==0)
	{
		SendRoomMessage(pIServerUserItem, TEXT("抱歉，比赛编号不够请稍等。"),SMT_CHAT);
		return true;
	}


	if(pIServerUserItem->GetUserStatus()!=US_FREE || pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		SendRoomMessage(pIServerUserItem, TEXT("对不起，您当前的状态不允许参加比赛！"),SMT_CHAT);
		return false;
	}
	if (m_MatchRule.dwInitalScore==0&&pIServerUserItem->GetUserScore()<m_MatchRule.dwInitalBase*m_MatchRule.wEnterBase+m_MatchRule.dwFee)
	{
		TCHAR szMsg[256]=TEXT("");
		_sntprintf(szMsg,CountArray(szMsg),TEXT("对不起，您当前的金币低于了%d，不能参加比赛！"),
			m_MatchRule.dwInitalBase*m_MatchRule.wEnterBase+m_MatchRule.dwFee);
		SendRoomMessage(pIServerUserItem, szMsg,SMT_CHAT|SMT_EJECT);
		return true;
	}

	if(m_MatchRule.dwFee>0 && !pIServerUserItem->IsAndroidUser())
	{
		ASSERT(m_pIGameServiceFrame!=NULL);
		if(m_pIGameServiceFrame==NULL) return false;

		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_FEE,&m_MatchRule.dwFee, sizeof(DWORD));
		return true;
	}

	//WB下面应该是机器人才会执行
	if(!m_pCurMatch->OnUserSignUp(pIServerUserItem))
	{
		SendRoomMessage(pIServerUserItem, TEXT("报名失败"),SMT_CHAT);
		return true;
	}
	//设置机器人用户比赛报名
	pIServerUserItem->SetUserEnlist(MS_SIGNUP);
	m_OnMatchUserItem.Add(pIServerUserItem);
	SendMatchInfo(NULL);


	if(m_pIGameServiceFrame!=NULL)
	{
		SCORE score=SCORE(m_MatchRule.dwFee)*-1;
		if(m_MatchRule.dwInitalScore==0)
		{
			//pIServerUserItem->ModifyUserInsure(score,0,0);
			pIServerUserItem->GetUserInfo()->lScore+=score;
			m_pIServerUserItem->OnEventUserItemScore(pIServerUserItem,SCORE_REASON_MATCH_FEE);
		}
		SendRoomMessage(pIServerUserItem, TEXT("恭喜，报名成功，请等待系统为您安排座位！"),SMT_CHAT);
		//m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("恭喜，报名成功，请等待系统为您安排座位！"),SMT_CHAT);
	}
	return true;
}

//用户退出比赛
bool  CGameMatchServiceManager::OnUserQuitGame(IServerUserItem * pIServerUserItem, BYTE cbReason,WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL) return false;

	bool bJoin=false;
	for (INT_PTR i=0;i<m_OnMatchUserItem.GetCount();i++)
	{
		//获取用户
		IServerUserItem * pOnLineUserItem=m_OnMatchUserItem[i];

		//用户判断
		if (pOnLineUserItem==pIServerUserItem)
		{
			ASSERT(pOnLineUserItem->GetUserID()==pIServerUserItem->GetUserID());
			////机器人不允许退出比赛 这个没用 实际上机器人已经跑掉了
			//if(pIServerUserItem->IsAndroidUser())return true;

			m_OnMatchUserItem.RemoveAt(i--);
			bJoin=true;
			break;
		}
	}

	if(bJoin)
	{
		INT_PTR nGroupCount=m_MatchGroup.GetCount();
		for (INT_PTR i=0;i<nGroupCount;i++)
		{
			CChampionshipMatch *pMatch=m_MatchGroup[i];
			if(pMatch->OnUserQuitMatch(pIServerUserItem,true))
				return true;
		}
		if(m_pCurMatch->OnUserQuitMatch(pIServerUserItem,true))
		{
			if(m_MatchRule.dwFee>0 && !pIServerUserItem->IsAndroidUser())
			{
				DBR_GR_MatchFee MatchFee;
				MatchFee.dwUserID=pIServerUserItem->GetUserID();
				MatchFee.dwMatchFee=m_MatchRule.dwFee;
				MatchFee.dwInoutIndex=pIServerUserItem->GetInoutIndex();
				MatchFee.dwClientAddr=pIServerUserItem->GetClientAddr();
				MatchFee.dwMatchID=m_MatchRule.dwMatchID;
				MatchFee.dwMatchNO=m_pCurMatch->m_dwMatchNO;
				lstrcpyn(MatchFee.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchFee.szMachineID));

				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MATCH_QUIT,dwContextID,&MatchFee,sizeof(MatchFee));
			}
			SendMatchInfo(NULL);
			return true;
		}
		SendMatchInfo(NULL);
	}
	return false;
}

//时间事件
bool  CGameMatchServiceManager::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	if(dwTimerID>=IDI_GROUP_START)
	{
		for (INT_PTR i=m_MatchGroup.GetCount()-1;i>=0;i--)
		{
			//查看是哪场比赛的定时器
			if (m_MatchGroup[i]->m_LoopTimer==((dwTimerID-IDI_GROUP_START)/10))
			{
				m_MatchGroup[i]->OnTimeMessage(dwTimerID-(m_MatchGroup[i]->m_LoopTimer*10),dwBindParameter);
			}
		}
		return true;
	}

	switch(dwTimerID)
	{
	case IDI_CHECK_OVER_MATCH:
		{
			//定时检测一下 那些比赛人数不够被卡的问题
			for (INT_PTR i=m_MatchGroup.GetCount()-1;i>=0;i--)
			{
				//参赛用户小于椅子数量 就要结束
				m_MatchGroup[i]->CheckMatchUser();
			}
			return true;
		}
	case IDI_DELETE_OVER_MATCH:
		{
			for (int i=0;m_OverMatchGroup.GetCount();i++)
			{
				CChampionshipMatch * pOverMatch=m_OverMatchGroup[i];
				m_OverMatchGroup.RemoveAt(i--);
				SafeDelete(pOverMatch);
			}
			return true;
		}
	case IDI_HALF_MINUTE:
		{			
			IAndroidUserItem *pAndroidUserItem=GetFreeAndroidUserItem();
			if(pAndroidUserItem!=NULL)
				OnUserJoinGame(pAndroidUserItem->GetMeUserItem(),0);
			return true;
		}
	}
	return true;
}

//参数修改
bool  CGameMatchServiceManager::RectifyServiceOption(tagGameServiceOption * pGameServiceOption,tagGameServiceAttrib *pGameServiceAttrib)
{
	//效验参数
	ASSERT(pGameServiceOption!=NULL);
	if (pGameServiceOption==NULL) return false;
	m_pGameServiceOption = pGameServiceOption;
	m_pGameServiceAttrib = pGameServiceAttrib;

	CChampionshipMatch::m_wChairCount=m_pGameServiceAttrib->wChairCount;
	if (m_ppITableFrame==NULL)
	{
		m_ppITableFrame=new ITableFrame*[pGameServiceOption->wTableCount];
	}
	
	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szPath,CountArray(szPath));

	//构造路径
	CString szFileName;
	szFileName.Format(TEXT("%s\\MatchOption%d.ini"),szPath,m_pGameServiceOption->wServerID);

	try
	{
		ReadMatchOption(szFileName);
	}
	catch(LPCTSTR pszString)
	{
		CTraceService::TraceString(pszString,TraceLevel_Exception);
		return false;
	}

	if (m_pCurMatch==NULL)
	{
		m_pCurMatch=new CChampionshipMatch(m_MatchRule, (DWORD)time(NULL), this);
	}

	return true;
}

//建立比赛模式
void *  CGameMatchServiceManager::CreateGameMatchSink(const IID & Guid, DWORD dwQueryVer)
{
	//变量定义
	CGameMatchSink *pGameMatchSink=NULL;
	//建立对象
	try
	{
		pGameMatchSink=new CGameMatchSink();
		if (pGameMatchSink==NULL) throw TEXT("创建失败");
		void * pObject=pGameMatchSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		
		return pObject;
	}
	catch (...) {}

	return NULL;
}

//发送数据
bool  CGameMatchServiceManager::SendDataToServer(DWORD dwUserIndex, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	ASSERT(FALSE);
	return true;
	//构造数据
	TCP_Command Command;
	Command.wSubCmdID=wSubCmdID;
	Command.wMainCmdID=wMainCmdID;

	//消息处理
	try
	{
		if (m_pITCPNetworkEngineEvent == NULL)
		{
			throw 0;
		}
		if (dwUserIndex>=INDEX_ANDROID)
		{
			
		}
		else
			if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(dwUserIndex,Command,pData,wDataSize)==false)
			{
				throw 0;
			}
	}
	catch (...) 
	{
		ASSERT(FALSE);
		DeleteUserItem(dwUserIndex);
		return false;
	}

	return true;
}

//报名人满，开始比赛
bool CGameMatchServiceManager::OnMatchStart(CChampionshipMatch *pMatch)
{
	ASSERT(pMatch==m_pCurMatch&&m_LoopTimer.GetCount()>0);
	//编号不够 不能报名的
	pMatch->m_LoopTimer=m_LoopTimer[0];
	m_LoopTimer.RemoveAt(0);
	m_MatchGroup.Add(pMatch);

	//准备新的一组比赛
	m_pCurMatch=new CChampionshipMatch(m_MatchRule, (DWORD)time(NULL), this);
	
	SendMatchInfo(NULL);
	BYTE cbMatchStatus=MS_MATCHING;
	SendGroupData( MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus),pMatch);

	return true;
}

//等待超时
bool CGameMatchServiceManager::OnWaitTimeOut(DWORD dwMatchNO)
{
	return true;
}

//用户淘汰
bool CGameMatchServiceManager::OnUserOut(IServerUserItem *pUserItem,LPCTSTR szReason,WORD wLen,WORD wRank,CChampionshipMatch *pMatch)
{
	if(pUserItem==NULL)
		return false;

	pUserItem->SetUserEnlist(MS_NULL);

	//清除比赛后的一些用户
	for (INT_PTR i=0;i<pMatch->m_FreeUserArray.GetCount();i++)
	{
		if(pMatch->m_FreeUserArray[i]->GetUserID()==pUserItem->GetUserID())
		{
			pMatch->m_FreeUserArray.RemoveAt(i);
			break;
		}
	}
	for (INT_PTR i=0;i<pMatch->m_ReOutUserArray.GetCount();i++)
	{
		if(pMatch->m_ReOutUserArray[i]->GetUserID()==pUserItem->GetUserID())
		{
			pMatch->m_ReOutUserArray.RemoveAt(i);
			break;
		}
	}

	if(wLen>0 && szReason!=NULL)
	{
		CString str(szReason, wLen);
		SendGameMessage(pUserItem,str,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);
	}

	for (INT_PTR i=0;i<m_OnMatchUserItem.GetCount();i++)
	{
		//获取用户
		IServerUserItem * pOnLineUserItem=m_OnMatchUserItem[i];

		//用户判断
		if (pOnLineUserItem==pUserItem)
		{
			m_OnMatchUserItem.RemoveAt(i);
			break;
		}
	}

	BYTE cbMatchStatus=MS_NULL;
	m_pIGameServiceFrame->SendData(pUserItem, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));

	if(pMatch==m_pCurMatch)
	{
		if(pUserItem!=NULL && !pUserItem->IsAndroidUser())
		{
			if(m_MatchRule.dwFee>0)
			{
				DBR_GR_MatchFee MatchFee;
				MatchFee.dwUserID=pUserItem->GetUserID();
				MatchFee.dwMatchFee=m_MatchRule.dwFee;
				MatchFee.dwInoutIndex=pUserItem->GetInoutIndex();
				MatchFee.dwClientAddr=pUserItem->GetClientAddr();
				MatchFee.dwMatchID=m_MatchRule.dwMatchID;
				MatchFee.dwMatchNO=m_pCurMatch->m_dwMatchNO;
				lstrcpyn(MatchFee.szMachineID,pUserItem->GetMachineID(),CountArray(MatchFee.szMachineID));

				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MATCH_QUIT,0,&MatchFee,sizeof(MatchFee));

				CString str;
				str.Format(TEXT("该房间即将关闭，请到其他房间报名参赛。退还报名费%d金币。"),m_MatchRule.dwFee);
				m_pIGameServiceFrame->SendRoomMessage(pUserItem,str,SMT_EJECT|SMT_CHAT|SMT_CLOSE_ROOM);
			}
		}
		CServerRule::SetForfendRoomEnter(static_cast<DWORD>(m_pGameServiceOption->dwServerRule),true);
	}
	else if(m_pIDataBaseEngine!=NULL && wRank > m_RewardArr.GetCount())
	{
		DBR_GR_MatchReward MatchReward;
		ZeroMemory(&MatchReward,sizeof(MatchReward));
		MatchReward.dwMatchID=m_MatchRule.dwMatchID;
		MatchReward.dwUserID=pUserItem->GetUserID();
		MatchReward.dwMatchNO=pMatch->m_dwMatchNO;
		MatchReward.wRank=wRank;
		MatchReward.lMatchScore=pUserItem->GetUserScore();
		MatchReward.dwClientAddr=pUserItem->GetClientAddr();
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MATCH_REWARD, 0, &MatchReward, sizeof(MatchReward));

	}

	////if(pMatch!=NULL)
	////{
	////	CMD_GR_Match_Num MatchNum;
	////	MatchNum.dwWaitting=(DWORD)pMatch->m_OnMatchUserMap.GetCount();
	////	MatchNum.dwTotal=m_MatchRule.wUserCount;
	////	MatchNum.dwMatchTotal=(DWORD)m_OnMatchUserItem.GetCount();

	////	SendGroupData(MDM_GR_MATCH,SUB_GR_MATCH_USER_COUNT,&MatchNum, sizeof(MatchNum),pMatch);
	////}
	return true;
}

//删除用户
bool CGameMatchServiceManager::DeleteUserItem(DWORD dwUserIndex)
{
	try
	{
		if (m_pITCPNetworkEngineEvent == NULL)
		{
			throw 0;
		}
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwUserIndex,0,0L);
	}
	catch (...)
	{
		//错误断言
		ASSERT(FALSE);
		return false;
	}
	return true;
}

//发送消息
bool CGameMatchServiceManager::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	if(pIServerUserItem==NULL)
		return false;
	//构造数据包
	CMD_CM_SystemMessage Message;
	Message.wType=wMessageType;

	lstrcpyn(Message.szString,lpszMessage,CountArray(Message.szString));
	Message.wLength=CountStringBuffer(Message.szString);

	//发送数据
	WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
	return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&Message,wSendSize);
}

//发送游戏消息
bool CGameMatchServiceManager::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	//排除在没有坐下的时候 关闭客户端
	if (!pIServerUserItem->IsClientReady()&&(wMessageType&SMT_CLOSE_GAME)==0)
	{
		return false;
	}
	//构造数据包
	CMD_CM_SystemMessage Message;
	Message.wType=wMessageType;
	lstrcpyn(Message.szString,lpszMessage,CountArray(Message.szString));
	Message.wLength=CountStringBuffer(Message.szString);

	//发送数据
	WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
	return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&Message,wSendSize);
}

//初始化桌子框架
bool CGameMatchServiceManager::InitTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	if(pTableFrame==NULL || m_ppITableFrame==NULL ||
		wTableID>m_pGameServiceOption->wTableCount)
	{
		ASSERT(false);
		return false;
	}
	m_ppITableFrame[wTableID]=pTableFrame;

	return true;
}

//发送数据
bool CGameMatchServiceManager::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	if(pIServerUserItem!=NULL)
		return m_pIGameServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
	else
		return m_pIGameServiceFrame->SendData(BG_COMPUTER, wMainCmdID,wSubCmdID, pData, wDataSize);
    
	return true;
}

//读取比赛规则
bool CGameMatchServiceManager::ReadMatchOption(LPCTSTR lpFileName)
{
	m_MatchRule.dwMatchID=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("MatchID"), (DWORD)-1, lpFileName);
	if(m_MatchRule.dwMatchID==(DWORD)-1)
		throw TEXT("MatchID配置错误……");
	GetPrivateProfileString(TEXT("MatchOption"), TEXT("MatchName"),TEXT(""), m_MatchRule.szMatchName, LEN_SERVER, lpFileName);

	m_MatchRule.enFirstRule=(enFirstRoundRule)GetPrivateProfileInt(TEXT("MatchOption"), TEXT("FirstRoundRule"), 0, lpFileName);
	if(m_MatchRule.enFirstRule==FirstRoundRule_Time)
	{
		throw TEXT("时间模式暂不能使用~~");
		m_MatchRule.dwFirstRoundTime=(DWORD)GetPrivateProfileInt(TEXT("MatchOption"), TEXT("FirstRoundTime"), 0, lpFileName);
		m_MatchRule.dwAheadStatistics=(DWORD)GetPrivateProfileInt(TEXT("MatchOption"), TEXT("AheadStatistics"), 180, lpFileName);
	}else if(m_MatchRule.enFirstRule==TimerRoundRule)//定时賽
	{
		m_MatchRule.sTimerRule.cbStartHour=(BYTE)GetPrivateProfileInt(TEXT("MatchOption"), TEXT("StartHour"), 0, lpFileName);
		m_MatchRule.sTimerRule.cbStartMinute=(BYTE)GetPrivateProfileInt(TEXT("MatchOption"), TEXT("StartMinute"), 0, lpFileName);
		m_MatchRule.sTimerRule.cbEndHour=(BYTE)GetPrivateProfileInt(TEXT("MatchOption"), TEXT("EndHour"), 0, lpFileName);
		m_MatchRule.sTimerRule.cbEndMinute=(BYTE)GetPrivateProfileInt(TEXT("MatchOption"), TEXT("EndMinute"), 0, lpFileName);

		m_MatchRule.sTimerRule.wMaxGameCount=(WORD)GetPrivateProfileInt(TEXT("MatchOption"), TEXT("MaxGameCount"), 255, lpFileName);
		m_MatchRule.sTimerRule.wMinGameCount=(WORD)GetPrivateProfileInt(TEXT("MatchOption"), TEXT("MinGameCount"), 10, lpFileName);
		m_MatchRule.sTimerRule.cbOutRoomCount=(BYTE)GetPrivateProfileInt(TEXT("MatchOption"), TEXT("OutRoomCount"), 3, lpFileName);
	}

	BYTE cbGameEndWriteGold=(BYTE)GetPrivateProfileInt(TEXT("MatchOption"), TEXT("GameEndWriteGold"), 0, lpFileName);
	CServerRule::SetRecordGameScore(m_pGameServiceOption->dwServerRule,cbGameEndWriteGold!=0?true:false);
	CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,cbGameEndWriteGold!=0?true:false);

	m_MatchRule.wUserCount=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("UserCount"), 0, lpFileName);
	if(m_MatchRule.enFirstRule!=TimerRoundRule && (m_MatchRule.wUserCount==0 || m_MatchRule.wUserCount%CChampionshipMatch::m_wChairCount!=0))
		throw TEXT("参数UserCount错误，开赛人数不能为0，并且开赛人数必须是单桌椅子数的倍数。");
	m_MatchRule.wAndroidUserCount=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("AndroidUserCount"), 0, lpFileName);
	m_MatchRule.wWaitTime=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("WaitTime"), 3, lpFileName);
	m_MatchRule.dwInitalBase=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("InitalBase"), 0, lpFileName);
	if(m_MatchRule.dwInitalBase==0)
		throw TEXT("参数InitalBase错误，初始积分必须大于0。");
	m_MatchRule.wRemainUserCount=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("RemainUserCount"), 0, lpFileName);
	m_MatchRule.cbFixedBase=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("FixedBase"), 0, lpFileName);
	m_MatchRule.wEnterBase=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("EnterBase"), 0, lpFileName);
	m_MatchRule.wLeaveBase=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("LeaveBase"), 0, lpFileName);
	m_MatchRule.dwGrowthTime=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("GrowthTime"), 0, lpFileName);
	m_MatchRule.cbGrowthRange=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("GrowthRange"), 0, lpFileName);
	m_MatchRule.dwFee=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("Fee"), 0, lpFileName);
	m_MatchRule.dwInitalScore=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("InitalScore"), 0, lpFileName);
	m_MatchRule.dwSecRoundUserCount=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("SecRoundUserCount"), 0, lpFileName);
	m_MatchRule.cbLeastScore=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("LeastScore"), 25, lpFileName);
	if(m_MatchRule.cbLeastScore<25)
		m_MatchRule.cbLeastScore=25;
	else if(m_MatchRule.cbLeastScore>100)
		m_MatchRule.cbLeastScore=100;
	m_MatchRule.cbFirScoreRule=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("FirScoreRule"), 0, lpFileName);
	m_MatchRule.cbFirGameCount=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("FirGameCount"), 1, lpFileName);
	m_MatchRule.cbSecGameCount=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("SecGameCount"), 1, lpFileName);
	m_MatchRule.cbSecSwitchTableCount=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("SecSwitchTableCount"), 0, lpFileName);
	if(m_MatchRule.cbFirGameCount==0 || m_MatchRule.cbSecGameCount==0)
		throw TEXT("参数FirGameCount或者SecGameCount错误，游戏局数不能为0");
	m_MatchRule.wSecSwitchTableTime=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("SecSwitchTableTime"), 30, lpFileName);
	if(m_MatchRule.wSecSwitchTableTime<5 || m_MatchRule.wSecSwitchTableTime>300)
		throw TEXT("参数SecSwitchTableTime错误，必须5-300之间");
	m_MatchRule.cbSecScoreRule=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("SecScoreRule"), 0, lpFileName);
	m_MatchRule.cbSecRoundCount=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("SecRoundCount"), 0, lpFileName);
	if(m_MatchRule.enFirstRule!=TimerRoundRule && (m_MatchRule.cbSecRoundCount==0 || m_MatchRule.cbSecRoundCount>CountArray(m_MatchRule.arrSecRoundUserCount)))
		throw TEXT("参数SecRoundCount错误，游戏轮数必须是大于0小于16");
	m_MatchRule.wPrecedeTimer=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("PrecedeTimer"), 60, lpFileName);
	if(m_MatchRule.wPrecedeTimer<60)m_MatchRule.wPrecedeTimer=60;
	m_MatchRule.cbMatchEndTimer=GetPrivateProfileInt(TEXT("MatchOption"), TEXT("MatchEndTimer"), 45, lpFileName);
	if(m_MatchRule.cbMatchEndTimer<5)
		throw TEXT("比赛时间错误，游戏时间必须大于5分钟");
	CString strKey;
	for(BYTE i=0;i<m_MatchRule.cbSecRoundCount;i++)
	{
		strKey.Format(TEXT("Round%d"),i+1);
		WORD wUserCount=GetPrivateProfileInt(TEXT("MatchOption"), strKey, 0, lpFileName);
		m_MatchRule.arrSecRoundUserCount[i]=wUserCount;
		strKey.Format(TEXT("Expand%d"),i+1);
		WORD wExpand=GetPrivateProfileInt(TEXT("MatchOption"), strKey, 0, lpFileName);
		m_MatchRule.arrSecRoundExpand[i]=wExpand;
	}

	for (INT_PTR i=0;i<m_RewardArr.GetCount();i++)
		SafeDelete(m_RewardArr[0]);
	m_RewardArr.RemoveAll();
	UINT nCount=GetPrivateProfileInt(TEXT("Award"), TEXT("Count"), 0, lpFileName);
	for (BYTE i=0;i<nCount;i++)
	{
		tagMatchReward *pMatchReward=new tagMatchReward;
		strKey.Format(TEXT("Rank%dGold"),i+1);
		pMatchReward->dwGold=GetPrivateProfileInt(TEXT("Award"), strKey, 0, lpFileName);
		strKey.Format(TEXT("Rank%dMedal"),i+1);
		pMatchReward->dwMedal=GetPrivateProfileInt(TEXT("Award"), strKey, 0, lpFileName);
		strKey.Format(TEXT("Rank%dExp"),i+1);
		pMatchReward->dwExperience=GetPrivateProfileInt(TEXT("Award"), strKey, 0, lpFileName);
		m_RewardArr.Add(pMatchReward);
	}

	for(BYTE i=0; i<MAX_MATCH_DESC; i++)
	{
		strKey.Format(TEXT("Title%d"), i+1);
		GetPrivateProfileString(TEXT("MatchDesc"), strKey, TEXT(""), m_MatchDesc.szTitle[i], CountArray(m_MatchDesc.szTitle[i]), lpFileName);
		strKey.Format(TEXT("Describe%d"), i+1);
		GetPrivateProfileString(TEXT("MatchDesc"), strKey, TEXT(""), m_MatchDesc.szDescribe[i], CountArray(m_MatchDesc.szDescribe[i]), lpFileName);
	}

	CWHIniData IniData;
	IniData.SetIniFilePath(lpFileName);
	IniData.ReadColor(m_MatchDesc.crTitleColor, TEXT("MatchDesc"), TEXT("TitleColor"));
	IniData.ReadColor(m_MatchDesc.crDescribeColor, TEXT("MatchDesc"), TEXT("DescribeColor"));
	return true;
}

//检查是否已经逃跑的用户
bool CGameMatchServiceManager::CheckRunWayUser(CChampionshipMatch *pMatch,DWORD dwUserID)
{
	if(pMatch==NULL)return false;
	for (int i=0;i<pMatch->m_RunWayArray.GetCount();i++)
	{
		if(pMatch->m_RunWayArray[i]==dwUserID)return true;
	}
	return false;
}

//定时器
bool CGameMatchServiceManager::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter,CChampionshipMatch *pMatch)
{
	//tagMatchTimerItem * pTimerItem=NULL;
	//INT_PTR nFreeCount=m_TimerItemFree.GetCount();
	//if (nFreeCount>0)
	//{
	//	pTimerItem=m_TimerItemFree[0];
	//	ASSERT(pTimerItem!=NULL);
	//	m_TimerItemFree.RemoveAt(0);
	//}
	//else 
	//{
	//	pTimerItem=new tagMatchTimerItem;
	//}
	//pTimerItem->dwRealID=dwTimerID;
	//pTimerItem->wBindParam=(WPARAM)pMatch;
	//pTimerItem->dwRepeatTimes=dwRepeat;
	//m_TimerItemActive.Add(pTimerItem);
	return m_pITimerEngine->SetTimer(dwTimerID,dwElapse,dwRepeat,dwBindParameter);
}
//定时器
bool CGameMatchServiceManager::KillGameTimer(DWORD dwTimerID,CChampionshipMatch *pMatch)
{
	//for (int i=0;i<m_TimerItemActive.GetCount();i++)
	//{
	//	if ((CChampionshipMatch *)m_TimerItemActive[i)->wBindParam==pMatch&&m_TimerItemActive[i)->dwRealID==dwTimerID)
	//	{
	//		m_TimerItemFree.Add(m_TimerItemActive[i));
	//		m_TimerItemActive.RemoveAt(i--); 
	//	}
	//}
	m_pITimerEngine->KillTimer(dwTimerID);
	return false;
}
//写入奖励
bool CGameMatchServiceManager::WriteUserAward(IServerUserItem *pIServerUserItem, WORD wRank,DWORD dwMatchNO,LPCTSTR szDescribe,CChampionshipMatch *pMatch)
{
	//参数效验
	ASSERT(pIServerUserItem!=NULL);
	if(pIServerUserItem==NULL) return false; 
	
	if(CheckRunWayUser(pMatch,pIServerUserItem->GetUserID()))return false;
	//65535是-1 代表逃跑
	if(wRank==65535&&pMatch)
	{
		pMatch->m_RunWayArray.Add(pIServerUserItem->GetUserID());
	}
	OnUserOut(pIServerUserItem, NULL, 0,wRank,pMatch);

	if(wRank<=m_RewardArr.GetCount())
	{
		if(pIServerUserItem!=NULL)
		{
			tagMatchReward *pMatchReward=m_RewardArr[wRank-1]; 
			if(pMatchReward==NULL)
				return false;
			//变量定义
			CMD_GR_MatchResult MatchResult;
			ZeroMemory(&MatchResult,sizeof(MatchResult));

			//构造数据
			_sntprintf(MatchResult.szDescribe,CountArray(MatchResult.szDescribe),TEXT("%s, 恭喜您在%s中获得第%d名,奖励如下："),
				pIServerUserItem->GetNickName(), m_MatchRule.szMatchName, wRank);
			MatchResult.dwExperience=pMatchReward->dwExperience;
			MatchResult.dwGold=pMatchReward->dwGold;
			MatchResult.dwMedal=pMatchReward->dwMedal;

			tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
			if (m_MatchRule.dwInitalScore==0)
			{
				pUserInfo->lScore+=(SCORE)pMatchReward->dwGold;
			}
			pUserInfo->dwUserMedal+=pMatchReward->dwMedal;
			pUserInfo->dwExperience+=pMatchReward->dwExperience;
			SendGroupUserScore(pIServerUserItem, NULL);

			//发送数据
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GR_MATCH_RESULT, &MatchResult, sizeof(MatchResult));

			if(m_pIDataBaseEngine!=NULL)
			{
				DBR_GR_MatchReward MatchReward;
				ZeroMemory(&MatchReward,sizeof(MatchReward));
				MatchReward.dwMatchID=m_MatchRule.dwMatchID;
				MatchReward.dwUserID=pIServerUserItem->GetUserID();
				MatchReward.dwMatchNO=dwMatchNO;
				MatchReward.wRank=wRank;
				MatchReward.dwExperience=pMatchReward->dwExperience;
				MatchReward.dwGold=pMatchReward->dwGold;
				MatchReward.lMatchScore=pIServerUserItem->GetUserScore();
				MatchReward.dwMedal=pMatchReward->dwMedal;
				MatchReward.dwClientAddr=pIServerUserItem->GetClientAddr();
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MATCH_REWARD, 0, &MatchReward, sizeof(MatchReward));
			}
			if (szDescribe!=NULL)
			{
				SendRoomMessage(pIServerUserItem, szDescribe, SMT_CHAT|SMT_EJECT);
			}
		}
	}
	else
	{
		if(szDescribe==NULL)
		{
			CString str;
			str.Format(TEXT("比赛已结束，恭喜您获得%d名。谢谢您的参与。您可以安全关闭比赛客户端，继续报名参加其他比赛。"),
				wRank);
			SendGameMessage(pIServerUserItem, str, SMT_CLOSE_GAME);
			SendRoomMessage(pIServerUserItem, str, SMT_CHAT|SMT_EJECT);
		}
		else
		{
			SendGameMessage(pIServerUserItem, szDescribe, SMT_CLOSE_GAME);
			SendRoomMessage(pIServerUserItem, szDescribe, SMT_CHAT|SMT_EJECT);
		}
	}
	return true;
}

//比赛结束
bool CGameMatchServiceManager::OnEventMatchOver(CChampionshipMatch *pMatch)
{
	for (int i=0;i<m_MatchGroup.GetCount();i++)
	{
		if(m_MatchGroup[i]==pMatch)
		{
			//发送可报名状态
			BYTE cbMatchStatus=MS_NULL;
			SendGroupData( MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus),pMatch);
			
			//还原编号
			m_LoopTimer.Add(pMatch->m_LoopTimer);
			//回收全部桌子
			for (int j=0;pMatch->m_MatchTableArray.GetCount();)
			{
				ITableFrameEx* pTempTable=pMatch->m_MatchTableArray[j];
				pMatch->m_MatchTableArray.RemoveAt(j);
				SafeDelete(pTempTable);
			}

			m_MatchGroup.RemoveAt(i--);
			m_OverMatchGroup.Add(pMatch);
			break;
		}
	}
	//OnUserQuitGame();

	return true;
}

//发送用户分数
bool CGameMatchServiceManager::SendGroupUserScore(IServerUserItem * pIServerUserItem,CChampionshipMatch *pMatch)
{

	//发送数据
	CMD_GR_UserScore UserScore;

	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//构造数据
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;
	UserScore.UserScore.dwUserMedal=pUserInfo->dwUserMedal;
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;

	//构造积分
	UserScore.UserScore.lGrade=pUserInfo->lGrade;
	UserScore.UserScore.lInsure=pUserInfo->lInsure;

	//构造积分
	UserScore.UserScore.lScore=pUserInfo->lScore;

	if(pMatch!=NULL)
	{
		POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
		IServerUserItem *pUserItem=NULL;
		DWORD dwUserID=0;
		while(pos!=NULL)
		{
			pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
			m_pIGameServiceFrame->SendData(pUserItem,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));	
		}
		return true;
	}
	return m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));
}

//发送状态
bool  CGameMatchServiceManager::SendGroupUserStatus(IServerUserItem * pIServerUserItem,CChampionshipMatch *pMatch)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->IsClientReady()==true);
	if (pIServerUserItem->IsClientReady()==false) return false;

	//变量定义
	CMD_GR_UserStatus UserStatus;
	memset(&UserStatus,0,sizeof(UserStatus));
	tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();

	//构造数据
	UserStatus.dwUserID=pUserData->dwUserID;
	UserStatus.UserStatus.wTableID=pUserData->wTableID;
	UserStatus.UserStatus.wChairID=pUserData->wChairID;
	UserStatus.UserStatus.cbUserStatus=pUserData->cbUserStatus;

	POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem=NULL;
	DWORD dwUserID=0;
	while(pos!=NULL)
	{
		pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
		if(pUserItem->IsClientReady())
			m_pIGameServiceFrame->SendData(pUserItem,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));	
	}
	return true;
}

//用户比赛成绩
bool CGameMatchServiceManager::OnUserMatchResult(DWORD dwUserID, WORD wKindID, WORD wServerID, WORD wSortID)
{
	return true;
}

//发送数据到服务器
bool CGameMatchServiceManager::SendDataToGameServer(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return SendDataToServer(dwAndroidID,wMainCmdID,wSubCmdID,pData,wDataSize);
}

//发送数据到一组用户
bool CGameMatchServiceManager::SendGroupData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize,CChampionshipMatch *pMatch)
{
	POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem=NULL;
	DWORD dwUserID=0;
	while(pos!=NULL)
	{
		pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
		m_pIGameServiceFrame->SendData(pUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);	
	}
	return true;
}

//发送信息
bool CGameMatchServiceManager::SendGroupUserMessage(LPCTSTR pStrMessage,CChampionshipMatch *pMatch)
{
	POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem=NULL;
	DWORD dwUserID=0;
	while(pos!=NULL)
	{
		pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
		SendGameMessage(pUserItem,pStrMessage, SMT_CHAT|SMT_TABLE_ROLL);
	}
	return true;
}

//获取一个空闲的机器人
IAndroidUserItem * CGameMatchServiceManager::GetFreeAndroidUserItem()
{

	if(m_pAndroidUserManager->GetAndroidCount()==0) return NULL;

	//机器状态
	tagAndroidUserInfo AndroidSimulate;
	m_pAndroidUserManager->GetAndroidUserInfo(AndroidSimulate,ANDROID_SIMULATE|ANDROID_PASSIVITY|ANDROID_INITIATIVE);

	if(AndroidSimulate.wFreeUserCount==0) 
		return NULL;
	else
	{
		//避免已报名的机器人重复安排
		for(WORD i=0; i<AndroidSimulate.wFreeUserCount;i++)
		{
			if(AndroidSimulate.pIAndroidUserFree[i]->GetMeUserItem()->GetUserEnlist()==MS_NULL)
			{
				ASSERT(AndroidSimulate.pIAndroidUserFree[i]->GetUserID()==AndroidSimulate.pIAndroidUserFree[i]->GetMeUserItem()->GetUserID());
				return AndroidSimulate.pIAndroidUserFree[i];
			}
		}
	}

	return NULL;
}

//设置替补机器人
bool CGameMatchServiceManager::SetReplaceAndroidItem(IServerUserItem *pIServerUserItem)
{

	return true;
}

//发送数据
bool CGameMatchServiceManager::SendMatchInfo(IServerUserItem * pIServerUserItem)
{
	CMD_GR_Match_Num MatchNum;
	MatchNum.dwWaitting=(DWORD)m_pCurMatch->m_OnMatchUserMap.GetCount();
	MatchNum.dwTotal=m_MatchRule.wUserCount;
	if(pIServerUserItem==NULL)
	{
		m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
		return true;
	}

	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_DESC, &m_MatchDesc, sizeof(m_MatchDesc));
	INT_PTR dwUserCount=m_OnMatchUserItem.GetCount();
	for (INT_PTR i=0;i<dwUserCount;i++)
	{
		if(m_OnMatchUserItem[i]==pIServerUserItem)
		{
			BYTE cbMatchStatus=MS_MATCHING;
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));
			return true;
		}
	}
	if(m_MatchRule.enFirstRule!=TimerRoundRule)
	{
		tagUserInfo *pUserScore=pIServerUserItem->GetUserInfo();
		pUserScore->dwWinCount=pUserScore->dwLostCount=pUserScore->dwFleeCount=pUserScore->dwDrawCount=pUserScore->cbGender=0;
		if((SCORE)m_MatchRule.dwInitalScore!=0)
		{
			pUserScore->lScore=(LONG)m_MatchRule.dwInitalScore;
		}
	}


	return SendGroupUserScore(pIServerUserItem, NULL);
}

//数据库事件
bool CGameMatchServiceManager::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GR_MATCH_FEE_RESULT:		//报名结果
		{
			//参数效验
			if(pIServerUserItem==NULL) return true;
			if(wDataSize!=sizeof(LONG)) return false;

			//变量定义
			LONG lReturnValue=*(LONG*)pData;

			if(lReturnValue==0)
			{
				if(!m_pCurMatch->OnUserSignUp(pIServerUserItem))
				{
					SendRoomMessage(pIServerUserItem, TEXT("报名失败"),SMT_CHAT);
					return false;
				}
				//设置用户比赛报名
				pIServerUserItem->SetUserEnlist(MS_SIGNUP);
				m_OnMatchUserItem.Add(pIServerUserItem);
				SendMatchInfo(NULL);
				SCORE score=SCORE(m_MatchRule.dwFee)*-1;
				if(m_MatchRule.dwInitalScore==0)
				{
					//pIServerUserItem->ModifyUserInsure(score,0,0);//不能用这个 会修改掉记录积分 点报名后在数据库直接扣金币这里不能直接扣分
					pIServerUserItem->GetUserInfo()->lScore+=score;
					m_pIServerUserItem->OnEventUserItemScore(pIServerUserItem,SCORE_REASON_MATCH_FEE);
				}
				SendRoomMessage(pIServerUserItem, TEXT("恭喜，报名成功，请等待系统为您安排座位！"),SMT_CHAT);
				//m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("恭喜，报名成功，请等待系统为您安排座位！"),SMT_EJECT);

			}
			else if(lReturnValue==1)
			{
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("您欢乐豆不够，不能加入比赛！"));
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szDescribe,SMT_EJECT);
			}
			break;
		}
	case DBO_GR_MATCH_QUIT_RESULT:		//退赛结果
		{
			//参数效验
			if(pIServerUserItem==NULL) return true;
			if(wDataSize!=sizeof(LONG)) return false;

			//变量定义
			LONG lReturnValue=*(LONG*)pData;

			if(lReturnValue==0)
			{
				pIServerUserItem->SetUserEnlist(MS_NULL);
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("退赛成功，退还报名费%d欢乐豆！"),m_MatchRule.dwFee);
				if(m_MatchRule.dwInitalScore==0)
				{
					//pIServerUserItem->ModifyUserInsure(m_MatchRule.dwFee,0,0);//跟报名一样
					pIServerUserItem->GetUserInfo()->lScore+=m_MatchRule.dwFee;
					m_pIServerUserItem->OnEventUserItemScore(pIServerUserItem,SCORE_REASON_MATCH_QUIT);
				}
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szDescribe,SMT_EJECT);
			}
			break;
		}	
	}
	return true;
}

//命令消息
bool CGameMatchServiceManager::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_MATCH_FEE:	//比赛费用
		{
			//参数效验
			ASSERT(wDataSize==sizeof(DWORD));
			if(wDataSize!=sizeof(DWORD)) return false;

			DWORD dwMatchFee=*(DWORD*)pData;
			if(dwMatchFee!=m_MatchRule.dwFee)return false;

			DBR_GR_MatchFee MatchFee;
			MatchFee.dwUserID=pIServerUserItem->GetUserID();
			MatchFee.dwMatchFee=m_MatchRule.dwFee;
			MatchFee.dwInoutIndex=pIServerUserItem->GetInoutIndex();
			MatchFee.dwClientAddr=pIServerUserItem->GetClientAddr();
			MatchFee.dwMatchID=m_MatchRule.dwMatchID;
			MatchFee.dwMatchNO=m_pCurMatch->m_dwMatchNO;
			lstrcpyn(MatchFee.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchFee.szMachineID));

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MATCH_FEE,dwSocketID,&MatchFee,sizeof(MatchFee));

			return true;
		}
	case SUB_GR_LEAVE_MATCH:	//退出比赛
		{
			//参数效验
			ASSERT(pIServerUserItem!=NULL);
			if(pIServerUserItem==NULL) return false;

			//if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return true;

			OnUserQuitGame(pIServerUserItem,0,0,dwSocketID);

			return true;
		}
	}
	return true;
}
//进入消息
bool CGameMatchServiceManager::OnEventEnterMatch(DWORD dwSocketID,VOID* pData,DWORD dwUserIP)
{
	//不是时间賽不可以进入
	if(m_MatchRule.enFirstRule!=TimerRoundRule)return false;

	SYSTEMTIME SystemTimeStart;
	GetLocalTime(&SystemTimeStart);

	CTime CurTime(SystemTimeStart.wYear,SystemTimeStart.wMonth,SystemTimeStart.wDay,
		SystemTimeStart.wHour,SystemTimeStart.wMinute,SystemTimeStart.wSecond);
	CTime StartTime(SystemTimeStart.wYear,SystemTimeStart.wMonth,SystemTimeStart.wDay,
		m_MatchRule.sTimerRule.cbStartHour,m_MatchRule.sTimerRule.cbStartMinute,0);
	CTime EndTime(SystemTimeStart.wYear,SystemTimeStart.wMonth,SystemTimeStart.wDay,
		m_MatchRule.sTimerRule.cbEndHour,m_MatchRule.sTimerRule.cbEndMinute,0);

	//首先判断时间
	if (CurTime<StartTime||CurTime>EndTime)
	{
		TCHAR szDescribe[256];
		_sntprintf(szDescribe,sizeof(szDescribe),TEXT("比赛时间 %s - %s 请稍后再试！"),StartTime.Format(TEXT("%H:%M")),EndTime.Format(TEXT("%H:%M")));
		m_pIGameServiceFrame->SendRoomMessage(dwSocketID,szDescribe,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,dwUserIP==0);
		return true;
	}
	//再判断门票和今天是否已经在比赛中或者是否离开限制次数 中途被淘汰的可以继续参加比赛 都不能进入比赛
	//处理消息
	CMD_GR_LogonUserID * pLogonUserID=(CMD_GR_LogonUserID *)pData;
	pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;
	pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID)-1]=0;

	//变量定义
	DBR_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//构造数据
	LogonUserID.dwUserID=pLogonUserID->dwUserID;
	LogonUserID.dwClientAddr=dwUserIP;
	lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_MATCHTIMER,dwSocketID,&LogonUserID,sizeof(LogonUserID));
	return true;
}
//为首轮插入空桌子
void CGameMatchServiceManager::InsertNullTable()
{
	for (int i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		//超过就退出
		if(m_pCurMatch->m_MatchTableArray.GetCount()>=m_MatchRule.wUserCount/m_pGameServiceAttrib->wChairCount)break;

		BOOL bIsInsert=true;
		for (int j=0;j<m_MatchGroup.GetCount();j++)
		{
			WORD wMatchGroupTableCount=(WORD)m_MatchGroup[j]->m_MatchTableArray.GetCount();
			for (int k=0;k<wMatchGroupTableCount;k++)
			{
				WORD wTableID=m_MatchGroup[j]->m_MatchTableArray[k]->wTableID;
				if(i==wTableID){bIsInsert=false;break;}//其他组已经分配了该桌
			}
		}
		//插入桌子
		if( bIsInsert&&m_ppITableFrame[i]->GetNullChairCount()==m_pGameServiceAttrib->wChairCount)
		{
			ITableFrameEx *TableFreameEx=new ITableFrameEx;
			ZeroMemory(TableFreameEx,sizeof(ITableFrameEx));
			TableFreameEx->dwBase=m_MatchRule.dwInitalBase;
			TableFreameEx->wTableID=i;
			TableFreameEx->pTableFrame=m_ppITableFrame[i];
			IGameMatchSink *pSink=m_ppITableFrame[i]->GetGameMatchSink();
			pSink->SetGameEventSink(QUERY_OBJECT_PTR_INTERFACE(m_pCurMatch,IUnknownEx));
			m_pCurMatch->AddMatchTable(TableFreameEx);
		}
	}
}
//获取空桌子
WORD CGameMatchServiceManager::GetNullTable()
{
	WORD wTableCount=0;
	for (int i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		BOOL bIsInsert=true;
		for (int j=0;j<m_MatchGroup.GetCount();j++)
		{
			WORD wMatchGroupTableCount=(WORD)m_MatchGroup[j]->m_MatchTableArray.GetCount();
			for (int k=0;k<wMatchGroupTableCount;k++)
			{
				WORD wTableID=m_MatchGroup[j]->m_MatchTableArray[k]->wTableID;
				if(i==wTableID){bIsInsert=false;break;}//其他组已经分配了该桌
			}
		}
		//插入桌子
		if( bIsInsert&&m_ppITableFrame[i]->GetNullChairCount()==m_pGameServiceAttrib->wChairCount)
		{
			wTableCount++;
		}
	}
	return wTableCount;
}

//////////////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(GameMatchServiceManager);

//////////////////////////////////////////////////////////////////////////

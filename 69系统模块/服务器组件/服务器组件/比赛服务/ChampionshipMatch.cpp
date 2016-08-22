#include "StdAfx.h"
#include ".\championshipmatch.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INVALID_VALUE				0xFFFF								//无效值
WORD CChampionshipMatch::m_wChairCount;

CChampionshipMatch::CChampionshipMatch(tagMatchRule &pMatchRule, DWORD dwMatchNO,IChampionshipMatchSink *pSink)
:m_pMatchRule(pMatchRule),m_dwMatchNO(dwMatchNO),m_pMatchSink(pSink)
{
	m_OnMatchUserMap.RemoveAll();
	m_FreeUserArray.RemoveAll();
	m_ReOutUserArray.RemoveAll();
	m_MatchTableArray.RemoveAll();
	m_dwTimeCount=0;
	m_enMatchStatus=MS_SIGNUPING;
	m_bIsContinue=false;
	m_dwCurBase=m_pMatchRule.dwInitalBase;
	m_wCurRountSec=0;

	m_wAndroidUserCount=0;
	m_LoopTimer=0;
	m_dwMatchTimeCount=0;
}

CChampionshipMatch::~CChampionshipMatch(void)
{
	m_wAndroidUserCount=0;
	m_enMatchStatus=MS_NULL_STATUS;
	m_OnMatchUserMap.RemoveAll();
	m_FreeUserArray.RemoveAll();
	m_ReOutUserArray.RemoveAll();
	m_MatchTableArray.RemoveAll();
}

//接口查询
VOID* CChampionshipMatch::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameEventSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameEventSink,Guid,dwQueryVer);
	return NULL;
}


//用户报名
bool CChampionshipMatch::OnUserSignUp(IServerUserItem * pUserItem)
{
	//用户判断
	ASSERT(pUserItem!=NULL);
	if (pUserItem==NULL) return false;

	//查找用户
	IServerUserItem * pIServerUserExist=NULL;
	m_OnMatchUserMap.Lookup(pUserItem->GetUserID(),pIServerUserExist);

	//用户判断
	ASSERT(pIServerUserExist==NULL);
	if (pIServerUserExist!=NULL) return false;

	//人数效验
	ASSERT(m_OnMatchUserMap.GetCount()<m_pMatchRule.wUserCount);
	if (m_OnMatchUserMap.GetCount()>=m_pMatchRule.wUserCount) return false;

	//机器判断
	if (pUserItem->IsAndroidUser()==true)
	{
		if (m_wAndroidUserCount>=m_pMatchRule.wAndroidUserCount)
		{
			return false;
		}
		m_wAndroidUserCount++;
	}

	//设置成绩
	tagUserInfo *pUserScore=pUserItem->GetUserInfo();
	pUserScore->dwWinCount=0L;
	pUserScore->dwLostCount=0L;
	pUserScore->dwFleeCount=0L;
	pUserScore->dwDrawCount=0L;
	pUserScore->lGrade=0L;
	if((SCORE)m_pMatchRule.dwInitalScore!=0)
	{
		pUserScore->lScore=(SCORE)m_pMatchRule.dwInitalScore;
	}


	//发送成绩
	m_pMatchSink->SendGroupUserScore(pUserItem,NULL);

	//设置用户
	m_OnMatchUserMap[pUserItem->GetUserID()]=pUserItem;

	//报名人满
	if (m_OnMatchUserMap.GetCount()>=m_pMatchRule.wUserCount)
	{
		//事件通知
		ASSERT(m_pMatchSink!=NULL);
		if (m_pMatchSink!=NULL) m_pMatchSink->SendMatchInfo(0);
		EfficacyStartMatch();
	}
	else
	{
		BYTE cbMatchStatus=MS_SIGNUP;
		m_pMatchSink->SendData(pUserItem, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));
	}

	return true;
}
//比赛开始
void CChampionshipMatch::EfficacyStartMatch()
{
	m_pMatchSink->InsertNullTable();
	//没有桌子的情况
	if( m_MatchTableArray.GetCount()<m_pMatchRule.wUserCount/m_wChairCount )
	{
		if(m_OnMatchUserMap.GetCount()<m_wChairCount)
		{
			//构造数据包
			CMD_CM_SystemMessage Message;
			Message.wType=SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME;
			lstrcpyn(Message.szString,TEXT("抱歉，由于某用户强行退出比赛，该场比赛提前结束。你可以继续等待比赛！"),CountArray(Message.szString));
			Message.wLength=CountStringBuffer(Message.szString);
			//发送数据
			WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
			m_pMatchSink->SendGroupData(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&Message,wSendSize, this);
			m_enMatchStatus=MS_SIGNUPING;
			return ;
		}

		m_enMatchStatus=MS_WAITTABLE;
		//3秒后继续调用该函数直到有桌子
		SetMatchGameTimer(IDI_WAITTABLE_FIRST_ROUND,3000,1,NULL);

		//构造数据包
		CMD_CM_SystemMessage Message;
		Message.wType=SMT_CHAT;
		_sntprintf(Message.szString,CountArray(Message.szString),
			TEXT("抱歉，由于过多用户桌子数量不够！还差 %d 张桌子开赛,请等待。。。"),m_pMatchRule.wUserCount/m_wChairCount-m_MatchTableArray.GetCount());
		Message.wLength=CountStringBuffer(Message.szString);

		//发送数据
		WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
		m_pMatchSink->SendGroupData(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&Message,wSendSize, this);
		return ;
	}
	//预赛安排座位
	DWORD dwUserID=0L;
	IServerUserItem *pIServerUserItem=NULL;
	POSITION pos=m_OnMatchUserMap.GetStartPosition();

	WORD wChairID=0;
	while (pos!=NULL)
	{
		//获取用户
		m_OnMatchUserMap.GetNextAssoc(pos,dwUserID,pIServerUserItem);
		//效验结果
		ASSERT(pIServerUserItem!=NULL);
		InsertFreeGroup(pIServerUserItem);
		//设置用户比赛中
		pIServerUserItem->SetUserEnlist(MS_MATCHING);
	}

	//桌子自动拉人
	TabelRequestUser(false);

	//设置状态
	if (m_pMatchRule.dwInitalScore==0)
	{
		m_enMatchStatus=MS_SECOND_ROUND;
		m_wCurRountSec++;
		m_bIsContinue=true;
	}
	else
	{
		m_enMatchStatus=MS_FIRST_ROUND;//MS_FIRST_ROUND;
		//m_enMatchStatus=MS_SECOND_ROUND;
		//m_wCurRountSec++;
	}



	//通知开始
	m_dwTimeCount=(DWORD)time(NULL);
	m_tMatchStart=CTime::GetCurrentTime();
	
	if (m_pMatchSink!=NULL) m_pMatchSink->OnMatchStart(this);
	m_dwMatchTimeCount=0;
	SetMatchGameTimer(IDI_CHECK_MATCH_GROUP,1000,TIMES_INFINITY,0);
}
//用户退赛
BOOL CChampionshipMatch::OnUserQuitMatch(IServerUserItem *pUserItem,bool bByUser)
{
	//用户判断
	ASSERT(pUserItem!=NULL);
	if (pUserItem==NULL) return false;
	//删除用户
	BOOL bSuccess=m_OnMatchUserMap.RemoveKey(pUserItem->GetUserID());
	if(bSuccess==false)return false;

	if (pUserItem->IsAndroidUser())
	{
		ASSERT(m_wAndroidUserCount>0);
		if (m_wAndroidUserCount>0) m_wAndroidUserCount--;
	}
	BYTE cbUserStatus=MS_NULL;
	m_pMatchSink->SendData(pUserItem, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbUserStatus, sizeof(cbUserStatus));
	if(m_enMatchStatus!=MS_SIGNUPING&&m_enMatchStatus!=MS_WAITTABLE)
	{
		m_pMatchSink->WriteUserAward(pUserItem,-1,m_dwMatchNO,TEXT("您已退赛！"),this);
	}
	return bSuccess;
}

//设置回调接口
bool CChampionshipMatch::SetMatchSink(IChampionshipMatchSink *pSink)
{
	return true;
}

//分配桌子
void CChampionshipMatch::SetMatchTable(ITableFrame *pTable[], WORD wTableCount)
{
	
}

//游戏开始
bool CChampionshipMatch::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	pITableFrame->SetCellScore(m_dwCurBase);
	KillMatchTimer(IDI_CheakTabelStart,(WPARAM)pITableFrame);
	GetTableInterface(pITableFrame)->bRoundTableFinish=false;
	GetTableInterface(pITableFrame)->bSwtichTableFinish=false;
	TCHAR szMsg[256]=TEXT("\0");
	if(m_enMatchStatus==MS_SECOND_ROUND)
	{
		if (m_pMatchRule.dwInitalScore==0)
		{
			for(WORD i=0; i<m_MatchTableArray.GetCount(); i++)
			{
				if(m_MatchTableArray[i]->pTableFrame==pITableFrame)
				{
					m_MatchTableArray[i]->cbStartTimes=0;
					m_MatchTableArray[i]->cbGameCount++;
					_sntprintf(szMsg, CountArray(szMsg),TEXT("淘汰赛第%d轮（共%d轮）,其中一位玩家番数总和等于或大于%d翻，该场比赛立即开始%d秒的等待时间，比赛期间金币低于%d将被淘汰"),m_wCurRountSec,
						m_pMatchRule.cbSecRoundCount,m_pMatchRule.arrSecRoundExpand[m_wCurRountSec-1],m_pMatchRule.wSecSwitchTableTime,m_pMatchRule.dwFee*m_pMatchRule.wLeaveBase);
					//构造数据包
					CMD_CM_SystemMessage Message;
					Message.wType=SMT_CHAT|SMT_TABLE_ROLL;

					lstrcpyn(Message.szString,szMsg,CountArray(Message.szString));
					Message.wLength=CountStringBuffer(Message.szString);

					//发送数据
					WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
					pITableFrame->SendTableData(INVALID_CHAIR,SUB_GF_SYSTEM_MESSAGE, (void*)&Message,wSendSize,MDM_GF_FRAME);

					//构造数据
					CMD_GR_Match_ER_Sparrows Match_ER_Sparrows;
					ZeroMemory(&Match_ER_Sparrows,sizeof(Match_ER_Sparrows));
					Match_ER_Sparrows.wALLGameRound=m_pMatchRule.cbSecRoundCount;
					Match_ER_Sparrows.wALLGameRoundFan=m_pMatchRule.arrSecRoundExpand[m_pMatchRule.cbSecRoundCount-1];
					Match_ER_Sparrows.wCurGameRound=m_wCurRountSec;
					Match_ER_Sparrows.wCurGameRoundFan=m_pMatchRule.arrSecRoundExpand[m_wCurRountSec-1];
					if(m_wCurRountSec<m_pMatchRule.cbSecRoundCount)
						Match_ER_Sparrows.wNextGameRoundFan=m_pMatchRule.arrSecRoundExpand[m_wCurRountSec];
					//_sntprintf(MatchInfo.szTitle[0], CountArray(MatchInfo.szTitle[0]),TEXT("游戏基数：%d"), m_dwCurBase);
					//_sntprintf(MatchInfo.szTitle[1], CountArray(MatchInfo.szTitle[1]),TEXT("停止条件：大于%d番"),m_pMatchRule.arrSecRoundExpand[m_wCurRountSec-1]-1);
					//

					for(WORD j=0; j<wChairCount; j++)
					{
						//获取用户
						IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(j);
						if(pITableFrame==NULL) continue;

						//变量定义
						tagUserInfo *pUserInfo=pIServerUserItem->GetUserInfo();
						Match_ER_Sparrows.wLocalGameFan=(WORD)pUserInfo->lGrade;
						Match_ER_Sparrows.wOtherGameFan=(WORD)pITableFrame->GetTableUserItem((j+1)%m_wChairCount)->GetUserInfo()->lGrade;
						////////MatchInfo.wGameCount=m_MatchTableArray[i]->cbGameCount; 
						//_sntprintf(MatchInfo.szTitle[2], CountArray(MatchInfo.szTitle[2]),TEXT("对家番数：%d"),pITableFrame->GetTableUserItem((j+1)%m_wChairCount)->GetUserInfo()->lGrade);
						//_sntprintf(MatchInfo.szTitle[3], CountArray(MatchInfo.szTitle[3]),TEXT("你的番数：%d"), pUserInfo->lGrade);
						//发送数据
						pITableFrame->SendTableData(j, SUB_GR_MATCH_INFO_ER_SPARROWS, &Match_ER_Sparrows, sizeof(Match_ER_Sparrows), MDM_GF_GAME);
						m_pMatchSink->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GR_MATCH_WAIT_TIP,NULL,0);
					}
				}
			}
			return true;
		}
		for(WORD i=0; i<m_MatchTableArray.GetCount(); i++)
		{		
			if(m_MatchTableArray[i]->pTableFrame==pITableFrame)
			{
				m_MatchTableArray[i]->cbStartTimes=0;
				m_MatchTableArray[i]->cbGameCount++;
				_sntprintf(szMsg, CountArray(szMsg),TEXT("淘汰赛第%d轮（共%d轮）第%d局开始（共%d局）。"),m_wCurRountSec,
					m_pMatchRule.cbSecRoundCount,m_MatchTableArray[i]->cbGameCount,m_pMatchRule.cbSecGameCount);

				//构造数据包
				CMD_CM_SystemMessage Message;
				Message.wType=SMT_CHAT;

				lstrcpyn(Message.szString,szMsg,CountArray(Message.szString));
				Message.wLength=CountStringBuffer(Message.szString);

				//发送数据
				WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
				pITableFrame->SendTableData(INVALID_CHAIR,SUB_GF_SYSTEM_MESSAGE, (void*)&Message,wSendSize,MDM_GF_FRAME);

				//构造数据
				CMD_GR_Match_Info MatchInfo;
				ZeroMemory(&MatchInfo,sizeof(MatchInfo));
				_sntprintf(MatchInfo.szTitle[0], CountArray(MatchInfo.szTitle[0]),TEXT("积分方式：积分"));
				_sntprintf(MatchInfo.szTitle[1], CountArray(MatchInfo.szTitle[1]),TEXT("游戏基数：%d"), m_dwCurBase);
				_sntprintf(MatchInfo.szTitle[2], CountArray(MatchInfo.szTitle[2]),TEXT("局制名称：%d局积分制"),m_pMatchRule.cbSecGameCount);
				_sntprintf(MatchInfo.szTitle[3], CountArray(MatchInfo.szTitle[3]),TEXT("%d局积分最高者获胜"), m_pMatchRule.cbSecGameCount);

				for(WORD j=0; j<wChairCount; j++)
				{
					//获取用户
					IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(j);
					if(pITableFrame==NULL) continue;

					//变量定义
					tagUserInfo *pUserInfo=pIServerUserItem->GetUserInfo();
					MatchInfo.wGameCount=m_MatchTableArray[i]->cbGameCount;

					//发送数据
					pITableFrame->SendTableData(j, SUB_GR_MATCH_INFO, &MatchInfo, sizeof(MatchInfo), MDM_GF_FRAME);
				}
				break;
			}
		}
	}
	else if(m_enMatchStatus==MS_FIRST_ROUND)
	{
		//构造数据
		CMD_GR_Match_Info MatchInfo;
		ZeroMemory(&MatchInfo,sizeof(MatchInfo));
		_sntprintf(MatchInfo.szTitle[0], CountArray(MatchInfo.szTitle[0]),TEXT("积分方式：底分×基数"));
		_sntprintf(MatchInfo.szTitle[1], CountArray(MatchInfo.szTitle[1]),TEXT("游戏基数：%d"), m_dwCurBase);
		if(m_pMatchRule.enFirstRule==FirstRoundRule_Score)
		{
			_sntprintf(MatchInfo.szTitle[2], CountArray(MatchInfo.szTitle[2]),TEXT("局制名称：打立出局"));
			_sntprintf(MatchInfo.szTitle[3], CountArray(MatchInfo.szTitle[3]),TEXT("低于 %d 分将被淘汰"), (int)(m_dwCurBase*m_pMatchRule.cbLeastScore/100));
		}
		else
		{
			_sntprintf(MatchInfo.szTitle[2], CountArray(MatchInfo.szTitle[2]),TEXT("局制名称：定时排名"));
			CTimeSpan span=CTime::GetCurrentTime()-m_tMatchStart;
			_sntprintf(MatchInfo.szTitle[3], CountArray(MatchInfo.szTitle[3]),TEXT("大约 %d 分钟后排名"), 
				(m_pMatchRule.dwFirstRoundTime-span.GetTotalSeconds()-m_pMatchRule.dwAheadStatistics)/60+1);
		}
		for(WORD i=0; i<wChairCount; i++)
		{
			//获取用户
			IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(i);
			if(pITableFrame==NULL) continue;

			//变量定义
			tagUserInfo *pUserInfo=pIServerUserItem->GetUserInfo();
			MatchInfo.wGameCount=(WORD)(pUserInfo->dwWinCount+pUserInfo->dwLostCount+pUserInfo->dwFleeCount+pUserInfo->dwDrawCount+1);

			//发送数据
			pITableFrame->SendTableData(i, SUB_GR_MATCH_INFO, &MatchInfo, sizeof(MatchInfo), MDM_GF_FRAME);
		}
	}
	return true;
}
//游戏结束
bool CChampionshipMatch::OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//记录本局和哪些用户玩过
	ITableFrameEx * pITableFrameEx=GetTableInterface(pITableFrame);
	ASSERT(pITableFrameEx);
	if (pITableFrameEx==NULL)return false;
	pITableFrameEx->m_FrontUserArray.RemoveAll();
	for (int i=0;i<m_wChairCount;i++)
	{
		IServerUserItem* UserItem=pITableFrame->GetTableUserItem(i);
		if(UserItem)
		{
			pITableFrameEx->m_FrontUserArray.Add(UserItem);
		}
	}

	if(m_pMatchRule.dwInitalScore==0)//2人麻将比赛结束
	{
		OnTableFinishRoundMJ(pITableFrame);
		CheckExitMatch();
		return true;
	}
	if(m_enMatchStatus==MS_STOP_OUT)
	{
		//if(m_OnMatchUserMap.GetCount()<(INT_PTR)m_pMatchRule.dwSecRoundUserCount)
		//{
		//	m_pMatchSink->SendGroupUserMessage(TEXT("复赛人数不够，比赛结束！你可以再次报名参加比赛"),this);
		//	MatchOver();
		//	return TRUE;
		//}

		LPCTSTR szMsg=TEXT("已经达到淘汰人数，等待其他玩家结束游戏后确定晋级人数!");
		for(WORD j=0;j<m_wChairCount;j++)
		{
			IServerUserItem *pUser=pITableFrame->GetTableUserItem(j);
			if(pUser!=NULL)
			{
				m_pMatchSink->SendGameMessage(pUser, szMsg,SMT_CHAT|SMT_TABLE_ROLL);
				InsertFreeGroup(pUser);
			}
		}
		SetRoundTableFinish(pITableFrame);
		SendWaitTip(pITableFrame);

		if (ChackRoundTableFinish())
		{
			m_enMatchStatus=MS_START_NEXT_ROUND;
			SetMatchGameTimer(IDI_EnterNextRound,5000,1,0);
			CheckExitMatch();
			return true;
		}
		
		//离开桌子 5秒后
		//m_pMatchSink->SetGameTimer(IDI_EXIT_TABLE,5000,1,(WPARAM)pITableFrame,this);
	}
	else if(m_enMatchStatus==MS_FIRST_ROUND)
	{
		if(m_pMatchRule.enFirstRule==FirstRoundRule_Score)
			FirstRoundRuleScoreGameEnd(pITableFrame);
		else if(m_pMatchRule.enFirstRule==FirstRoundRule_Time)
			FirstRoundRuleTimeGameEnd(0);
	}
	else if(m_enMatchStatus==MS_SECOND_ROUND)
	{
		OnTableFinishRound(pITableFrame);
	}
	CheckExitMatch();
	//比赛结束直接返回
	if(m_enMatchStatus==MS_NULL_STATUS)return true;

	if(m_OnMatchUserMap.GetCount()>0)
	{
		//分数通知
		tagMatchScore *pScore = new tagMatchScore[m_OnMatchUserMap.GetCount()];
		ZeroMemory(pScore,sizeof(tagMatchScore)*m_OnMatchUserMap.GetCount());
		WORD wCount=SortMapUser(pScore);
		for (WORD i=0;i<wCount;i++)
		{
			m_pMatchSink->SendGroupUserScore(pScore[i].pUserItem,this);
		}
		SafeDeleteArray(pScore);
	}

	return true;
}
//定时器
bool CChampionshipMatch::OnTimeMessage(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//状态校验
	if(m_enMatchStatus==MS_NULL_STATUS) return true;

	switch(dwTimerID)
	{
	case IDI_CHECK_MATCH_GROUP:
		{
			m_dwMatchTimeCount++;
			//打立出局模式
			if(m_pMatchRule.enFirstRule==FirstRoundRule_Score&&m_dwMatchTimeCount%5==0)//每5秒进来一次
			{
				//预赛状态
				if (m_enMatchStatus==MS_FIRST_ROUND)
				{
					bool bCanStart=true;
					DWORD dwTimeCount=(DWORD)time(NULL)-m_dwTimeCount;
					//预赛的时候 根据后台设置时间累加基注
					if(m_pMatchRule.cbFixedBase==0&&m_enMatchStatus==MS_FIRST_ROUND && dwTimeCount>m_pMatchRule.dwGrowthTime)
					{
						m_dwCurBase=m_dwCurBase*(100+m_pMatchRule.cbGrowthRange)/100;
						m_dwTimeCount=(DWORD)time(NULL);
					}
					//将空闲用户自动配桌
					//TabelRequestUser();
				}
				if(m_dwMatchTimeCount>m_pMatchRule.cbMatchEndTimer*60)//整个比赛不超过X分钟
				{
					//构造数据包
					CString szString;
					szString.Format(TEXT("抱歉，由于比赛系统到时，提前结算。欢迎您继续报名参加比赛！"));
					SendGroupBox(szString);
					LastMatchResult();
					return true;
				}
			}
			//捕获投递过的比赛定时器
			CaptureMatchTimer();

			return true;
		}
	case IDI_WAITTABLE_FIRST_ROUND:
		{
			if(m_enMatchStatus==MS_WAITTABLE)
			{
				EfficacyStartMatch();
			}
			return true;
		}
	case IDI_SWITCH_WAIT_TIME:
		{
			////将空闲用户自动配桌
			RearrangeUserSeat();
			TabelRequestUser();
			//ContinueGame((ITableFrame*)dwBindParameter);
			return true;
		}
	case IDI_TabelRequestUser:
		{
			TabelRequestUser();
			return true;
		}
	case IDI_EnterNextRound:
		{
			if(m_enMatchStatus==MS_START_NEXT_ROUND)
			{
				//继续下一轮
				ContinueRound();
			}
			return true;
		}
	case IDI_AllDismissGame:
		{
			RearrangeUserSeat();
			return true;
		}
	case IDI_LASTMATCHRESULT:
		{
			//最后比赛结果
			LastMatchResult();
			return true;
		}
	}

	return true;
}

//选择晋级用户
void CChampionshipMatch::SelectPromotionUser()
{
	KillMatchGameTimer(IDI_AllDismissGame);
	INT_PTR nUserCount=m_OnMatchUserMap.GetCount();
	if( nUserCount==0 || nUserCount<(INT_PTR)m_pMatchRule.arrSecRoundUserCount[m_wCurRountSec])
	{
		SendGroupBox(TEXT("抱歉，由于过多用户强行退出比赛，该场比赛提前结束。欢迎您继续报名参加比赛！"));
		LastMatchResult();
		return;
	}
	//全部强制停止游戏 所有玩家起立
	RearrangeUserSeat(false);
	
	//去掉多余的桌子 用来分给其他组 只留剩下的桌子数量
	for (WORD i=WORD(m_pMatchRule.arrSecRoundUserCount[m_wCurRountSec]/m_wChairCount);i<m_MatchTableArray.GetCount();)
	{
		ITableFrameEx* pTempTable=m_MatchTableArray[i];
		m_MatchTableArray.RemoveAt(i);
		SafeDelete(pTempTable);
	}

	//根据条件进入不同的判断
	if (m_pMatchRule.dwInitalScore==0)
	{
		m_bIsContinue=true;
		SetEnterUserMJ();
	}
	else
	{
		POSITION pos=m_OnMatchUserMap.GetStartPosition();
		IServerUserItem *pLoopUserItem=NULL;
		DWORD dwUserID=0;
		while(pos!=NULL)
		{
			m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pLoopUserItem);
			//调整积分
			if(m_enMatchStatus==MS_STOP_OUT)
			{
				if(pLoopUserItem->GetUserInfo()->lScore>0)
					pLoopUserItem->GetUserInfo()->lScore=(SCORE)sqrt(FLOAT(pLoopUserItem->GetUserInfo()->lScore))*10L/3L;
				else
					pLoopUserItem->GetUserInfo()->lScore=(SCORE)(pLoopUserItem->GetUserInfo()->lScore*0.15);
			}
			else if(m_enMatchStatus==MS_START_NEXT_ROUND)
			{
				pLoopUserItem->GetUserInfo()->lScore=(SCORE)(pLoopUserItem->GetUserInfo()->lScore*0.3);
			}
			//发送积分
			m_pMatchSink->SendGroupUserScore(pLoopUserItem,this);
		}
		SetEnterUserLand();
	}
	m_enMatchStatus=MS_SECOND_ROUND;
	m_wCurRountSec++;
	if (m_pMatchRule.cbFixedBase==0)m_dwCurBase=m_pMatchRule.dwInitalBase*2;
	
	TabelRequestUser();
}

//将玩家排序
WORD CChampionshipMatch::SortMapUser(tagMatchScore score[])
{
	INT_PTR nCount=m_OnMatchUserMap.GetCount();
	POSITION pos=m_OnMatchUserMap.GetStartPosition();
	nCount=0;
	while(pos!=NULL)
	{
		m_OnMatchUserMap.GetNextAssoc(pos,score[nCount].dwUserID, score[nCount].pUserItem);
		score[nCount].dwUserID=score[nCount].pUserItem->GetUserID();
		score[nCount].lScore=score[nCount].pUserItem->GetUserScore();
		nCount++;
	}
	for(INT_PTR i = 1;i < nCount;i++)
	{
		INT_PTR left = 0,right = i - 1;
		tagMatchScore  Temp = score[i];
		while(left <= right)
		{
			INT_PTR mid = (left + right)/2;
			if(score[i].lScore > score[mid].lScore)right = mid - 1;
			else left = mid + 1;
		}
		for(INT_PTR j = i - 1;j >= left;j--)
			score[j+1] = score[j];
		score[left] = Temp;
	}

	return (WORD)nCount;
}

//将玩家排序 该函数已经单元调试没有任何问题
WORD CChampionshipMatch::SortUserRanking(tagMatchRanking MatchRanking[],bool bIsLand)
{
	if(bIsLand)
	{
		INT_PTR nCount=m_OnMatchUserMap.GetCount();
		POSITION pos=m_OnMatchUserMap.GetStartPosition();
		nCount=0;
		while(pos!=NULL)
		{
			m_OnMatchUserMap.GetNextAssoc(pos,MatchRanking[nCount].dwUserID, MatchRanking[nCount].pUserItem);
			MatchRanking[nCount].dwUserID=MatchRanking[nCount].pUserItem->GetUserID();
			MatchRanking[nCount].lScore=MatchRanking[nCount].pUserItem->GetUserScore();
			nCount++;
		}
		for(INT_PTR i = 1;i < nCount;i++)
		{
			INT_PTR left = 0,right = i - 1;
			tagMatchRanking  Temp = MatchRanking[i];
			while(left <= right)
			{
				INT_PTR mid = (left + right)/2;
				if(MatchRanking[i].lScore > MatchRanking[mid].lScore)right = mid - 1;
				else left = mid + 1;
			}
			for(INT_PTR j = i - 1;j >= left;j--)
				MatchRanking[j+1] = MatchRanking[j];
			MatchRanking[left] = Temp;
		}

		//分数都相等 经验值最多的排前面
		for (INT_PTR i = 0;i < nCount-1;i++)
		{
			for (INT_PTR j = i+1;j < nCount;j++)
			{
				IServerUserItem *pUserItem1=MatchRanking[i].pUserItem;
				IServerUserItem *pUserItem2=MatchRanking[j].pUserItem;
				if(pUserItem1==NULL||pUserItem2==NULL)break;
				tagUserInfo *pUserInfo1=pUserItem1->GetUserInfo();
				tagUserInfo *pUserInfo2=pUserItem2->GetUserInfo();
				if(pUserInfo1==NULL||pUserInfo2==NULL)break;
				if (MatchRanking[i].lScore==MatchRanking[j].lScore&&pUserInfo1->dwExperience<pUserInfo2->dwExperience)
				{
					tagMatchRanking  Temp = MatchRanking[i];
					MatchRanking[i]=MatchRanking[j];
					MatchRanking[j]=Temp;
				}else break;
			}
		}

		return (WORD)nCount;
	}
	INT_PTR nCount=m_OnMatchUserMap.GetCount();
	POSITION pos=m_OnMatchUserMap.GetStartPosition();
	nCount=0;
	while(pos!=NULL)
	{
		m_OnMatchUserMap.GetNextAssoc(pos,MatchRanking[nCount].dwUserID, MatchRanking[nCount].pUserItem);
		MatchRanking[nCount].dwUserID=MatchRanking[nCount].pUserItem->GetUserID();
		MatchRanking[nCount].lScore=MatchRanking[nCount].pUserItem->GetUserScore();
		MatchRanking[nCount].lExpand=(LONG)MatchRanking[nCount].pUserItem->GetUserInfo()->lGrade;//2人麻将的番数
		nCount++;
	}
	//首先排序麻将番数量
	for(INT_PTR i = 1;i < nCount;i++)
	{
		INT_PTR left = 0,right = i - 1;
		tagMatchRanking  Temp = MatchRanking[i];
		while(left <= right)
		{
			INT_PTR mid = (left + right)/2;
			if(MatchRanking[i].lExpand > MatchRanking[mid].lExpand)right = mid - 1;
			else left = mid + 1;
		}
		for(INT_PTR j = i - 1;j >= left;j--)
			MatchRanking[j+1] = MatchRanking[j];
		MatchRanking[left] = Temp;
	}
	//在查询有多个相等的没 如果在在排序金币谁多
	for (INT_PTR i = 0;i < nCount-1;i++)
	{
		for (INT_PTR j = i+1;j < nCount;j++)
		{
			if (MatchRanking[i].lExpand==MatchRanking[j].lExpand&&MatchRanking[i].lScore<MatchRanking[j].lScore)
			{
				tagMatchRanking  Temp = MatchRanking[i];
				MatchRanking[i]=MatchRanking[j];
				MatchRanking[j]=Temp;
			}else break;
		}
	}
	return (WORD)nCount;
}

//将所有玩家起立
void CChampionshipMatch::RearrangeUserSeat(bool bDismissGame)
{
	KillMatchGameTimer(IDI_SWITCH_WAIT_TIME);
	for(WORD i=0; i<m_MatchTableArray.GetCount();i++)
	{
		ITableFrame* pTableFrame=m_MatchTableArray[i]->pTableFrame;
		if (pTableFrame==NULL)continue;
		if (bDismissGame==false)
		{
			m_MatchTableArray[i]->cbGameCount=0;
			m_MatchTableArray[i]->bRoundTableFinish=false;
			m_MatchTableArray[i]->bSwtichTableFinish=false;
		}
		//强制解散游戏
		if(bDismissGame==false&&pTableFrame->IsGameStarted())
		{
			CTraceService::TraceString(TEXT("新的一轮开始了，还有桌子正在比赛？"),TraceLevel_Exception);
		}
		if(bDismissGame&&pTableFrame->IsGameStarted())
		{
			pTableFrame->DismissGame();
		}
		//该桌用户全部离开
		AllUserExitTable(pTableFrame);
	}
	return;
}

//是否可以离开座位 
bool CChampionshipMatch::IsCanStanUp(IServerUserItem *pUserItem)
{
	//AfxMessageBox(L"是否可以离开座位");
	return false;
}
//玩家断线或重连 
void CChampionshipMatch::SetUserOffline(WORD wTableID,IServerUserItem *pUserItem, bool bOffline)
{
	//AfxMessageBox(L"玩家断线或重连");
}

//获取名次
WORD CChampionshipMatch::GetUserRank(IServerUserItem *pUserItem, ITableFrame *pITableFrame)
{
	//参数效验
	ASSERT(pUserItem!=NULL);
	if(pUserItem==NULL) return INVALID_WORD;
	//定义变量
	SCORE lUserScore=pUserItem->GetUserScore();
	WORD wRank=1;
	DWORD dwUserID=0;
	IServerUserItem *pLoopUserItem=NULL;

	if(pITableFrame==NULL)
	{
		//遍历容器
		POSITION pos=m_OnMatchUserMap.GetStartPosition();
		while(pos!=NULL)
		{
			m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pLoopUserItem);
			if(pLoopUserItem!=pUserItem && pLoopUserItem->GetUserScore()>lUserScore)
				wRank++;
		}
	}
	else
	{
		ASSERT(pUserItem->GetTableID()==pITableFrame->GetTableID());
		if(pUserItem->GetTableID()!=pITableFrame->GetTableID()) return INVALID_WORD;

		for(WORD i=0; i<m_wChairCount; i++)
		{
			pLoopUserItem=pITableFrame->GetTableUserItem(i);
			if(pLoopUserItem==NULL)continue;

			if(pLoopUserItem!=pUserItem && pLoopUserItem->GetUserScore()>lUserScore)
				wRank++;
		}
	}

	return wRank;
}
//获取名次
WORD CChampionshipMatch::GetUserRankMJ(IServerUserItem *pUserItem, ITableFrame *pITableFrame)
{
	//参数效验
	ASSERT(pUserItem!=NULL);
	if(pUserItem==NULL) return INVALID_WORD;
	//定义变量
	SCORE lGrade=pUserItem->GetUserInfo()->lGrade;
	WORD wRank=1;
	DWORD dwUserID=0;
	IServerUserItem *pLoopUserItem=NULL;

	if(pITableFrame==NULL)
	{
		INT_PTR nUserCount=m_OnMatchUserMap.GetCount();
		tagMatchRanking *pScore=new tagMatchRanking[nUserCount];
		ZeroMemory(pScore,sizeof(tagMatchRanking)*nUserCount);

		SortUserRanking(pScore,false);

		for (int i=0;i<nUserCount;i++)
		{
			if(pUserItem->GetUserID()==pScore[i].dwUserID)wRank=i+1;
		}

		SafeDeleteArray(pScore);
	}
	else
	{
		ASSERT(pUserItem->GetTableID()==pITableFrame->GetTableID());
		if(pUserItem->GetTableID()!=pITableFrame->GetTableID()) return INVALID_WORD;

		for(WORD i=0; i<m_wChairCount; i++)
		{
			pLoopUserItem=pITableFrame->GetTableUserItem(i);
			if(pLoopUserItem==NULL)continue;

			//先判断番数 相等在判断金币
			if(pLoopUserItem!=pUserItem && pLoopUserItem->GetUserInfo()->lGrade>lGrade)
				wRank++;
			else if(pLoopUserItem!=pUserItem && pLoopUserItem->GetUserInfo()->lGrade==lGrade&&
				pLoopUserItem->GetUserScore()>pUserItem->GetUserScore())
				wRank++;
		}
	}

	return wRank;
}

//获取桌子专用接口
ITableFrameEx* CChampionshipMatch::GetTableInterface(ITableFrame* ITable)
{
	for (int i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if (m_MatchTableArray[i]->pTableFrame==ITable)return m_MatchTableArray[i];
	}
	return NULL;
}

//完成一轮
bool CChampionshipMatch::OnTableFinishRound(ITableFrame *pITableFrame)
{
	ITableFrameEx * pITableFrameEx=GetTableInterface(pITableFrame);
	ASSERT(pITableFrameEx);
	if (pITableFrameEx==NULL)return false;

	if (pITableFrameEx->cbGameCount>=m_pMatchRule.cbSecGameCount)
	{
		if(m_wCurRountSec>=m_pMatchRule.cbSecRoundCount)
		{
			//比赛结束
			TCHAR szMsg[256]=TEXT("");
			_sntprintf(szMsg,CountArray(szMsg),TEXT("比赛结束，请等待系统为你排名！"));

			POSITION pos=m_OnMatchUserMap.GetStartPosition();
			IServerUserItem *pUser=NULL;
			DWORD dwUserID;
			while(pos!=NULL)
			{
				m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUser);
				m_pMatchSink->SendGameMessage(pUser,szMsg,SMT_CHAT|SMT_TABLE_ROLL);
			}

			SetMatchGameTimer(IDI_LASTMATCHRESULT,5000,1,(WPARAM)pITableFrame);
			return true;
		}
		//设定桌子完成
		SetRoundTableFinish(pITableFrame);
		//每桌第一名优先排名 其他为混合排名
		if(m_pMatchRule.cbSecScoreRule==0)
			SetEnterUserOROutUser(pITableFrame);
		SendWaitTip(pITableFrame);
		//查找桌子是否全部完成 如果全部完成将下一个阶段的比赛
		//进入下个阶段
		if (ChackRoundTableFinish())
		{
			//所有桌混合排名
			if(m_pMatchRule.cbSecScoreRule==1)
				SetEnterUserOROutUser();
			m_enMatchStatus=MS_START_NEXT_ROUND;
			SetMatchGameTimer(IDI_EnterNextRound,5000,1,0);
			
			return true;
		}
	}
	else
	{
		//加入空闲列表
		for (int i=0;i<m_wChairCount;i++)
		{
			IServerUserItem * ServerUserItem=pITableFrame->GetTableUserItem(i);
			if(ServerUserItem==NULL)continue;
			InsertFreeGroup(ServerUserItem);
		}

		//复赛自动换桌的控制
		if(m_pMatchRule.cbSecSwitchTableCount>0&&pITableFrameEx->cbGameCount>=m_pMatchRule.cbSecSwitchTableCount)
		{
			SetSwitchTableFinish(pITableFrame);
			SendWaitTip(pITableFrame);
			//检查该桌子是否可以换桌
			if (CheckSwitchTableFinish())
			{
				SetMatchGameTimer(IDI_SWITCH_WAIT_TIME,5000,1,0);
				return true;
			}
			SetMatchGameTimer(IDI_SWITCH_WAIT_TIME,m_pMatchRule.wSecSwitchTableTime*1000,1,0);
		}else//不换桌
		{
			PostMatchTimer(IDI_CONTINUE_GAME,5,(WPARAM)pITableFrame);
			return true;
		}
	}
	//离开桌子 5秒后
	//PostMatchTimer(IDI_EXIT_TABLE,5,(WPARAM)pITableFrame);
	return true;
}
//完成一轮
bool CChampionshipMatch::OnTableFinishRoundMJ(ITableFrame *pITableFrame)
{
	ITableFrameEx * pITableFrameEx=GetTableInterface(pITableFrame);
	ASSERT(pITableFrameEx);
	if (pITableFrameEx==NULL)return false;

	//更新用户番数
	OnEventGameStart(pITableFrame,m_wChairCount);

	for (int i=0;i<m_wChairCount;i++)
	{
		IServerUserItem* UserItem=pITableFrame->GetTableUserItem(i);
		if(UserItem)
		{
			//金币低于一定数量被立即被淘汰
			if(UserItem->GetUserInfo()->lScore<m_pMatchRule.dwInitalBase*m_pMatchRule.wLeaveBase)
			{
				m_pMatchSink->WriteUserAward(UserItem, (WORD)m_OnMatchUserMap.GetCount(),m_dwMatchNO,NULL,this);
			}else
			{
				InsertFreeGroup(UserItem);
			}
			//用户的番薯到达后台设置 停止比赛
			if(m_bIsContinue&&UserItem->GetUserInfo()->lGrade>=m_pMatchRule.arrSecRoundExpand[m_wCurRountSec-1])
			{
				m_bIsContinue=false;
				//等待一段时间 比如这段时间还有人在比赛 强制该轮的比赛
				SetMatchGameTimer(IDI_AllDismissGame,m_pMatchRule.wSecSwitchTableTime*1000,1,(WPARAM)pITableFrame);

				TCHAR szMsg[256]=TEXT("");
				_sntprintf(szMsg,CountArray(szMsg),TEXT("恭喜 %s 首先到达晋级番数，%d秒后即将结束初赛排名，确定晋级玩家！"),
					UserItem->GetUserInfo()->szNickName,m_pMatchRule.wSecSwitchTableTime);

				POSITION pos=m_OnMatchUserMap.GetStartPosition();
				IServerUserItem *pUser=NULL;
				DWORD dwUserID;
				while(pos!=NULL)
				{
					m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUser);
					m_pMatchSink->SendGameMessage(pUser,szMsg,SMT_CHAT|SMT_TABLE_ROLL);
				}
			}
		}
	}
	//继续比赛
	if (m_bIsContinue)
	{
		PostMatchTimer(IDI_CONTINUE_GAME,5,(WPARAM)pITableFrame);
		return true;
	}

	//设置桌子结束该轮标记
	SetRoundTableFinish(pITableFrame);
	SendWaitTipMJ(pITableFrame);
	
	if (ChackRoundTableFinish())
	{
		if(m_wCurRountSec>=m_pMatchRule.cbSecRoundCount)
		{
			//比赛结束
			TCHAR szMsg[256]=TEXT("");
			_sntprintf(szMsg,CountArray(szMsg),TEXT("比赛结束，请等待系统为你排名！"));

			POSITION pos=m_OnMatchUserMap.GetStartPosition();
			IServerUserItem *pUser=NULL;
			DWORD dwUserID;
			while(pos!=NULL)
			{
				m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUser);
				m_pMatchSink->SendGameMessage(pUser,szMsg,SMT_CHAT|SMT_TABLE_ROLL);
			}

			SetMatchGameTimer(IDI_LASTMATCHRESULT,5000,1,(WPARAM)pITableFrame);
			return true;
		}
		m_enMatchStatus=MS_START_NEXT_ROUND;
		SetMatchGameTimer(IDI_EnterNextRound,5000,1,0);
		return true;
	}
	return true;
}
//获取用户
IServerUserItem * CChampionshipMatch::GetSeatUserItem(DWORD dwSeatID)
{

	
	return NULL;
}

//用户坐下
bool CChampionshipMatch::OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if( bLookonUser == false )
	{
		
	}
	return true;
}

//用户起来
bool CChampionshipMatch::OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if( bLookonUser == false )
	{
		
	}
	return true;
}

//用户同意
bool CChampionshipMatch::OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	
	return false;
}

//比赛结束
void CChampionshipMatch::MatchOver()
{
	RearrangeUserSeat();
	AllKillMatchTimer();
	KillMatchGameTimer(IDI_CHECK_MATCH_GROUP);
	KillMatchGameTimer(IDI_WAITTABLE_FIRST_ROUND);
	KillMatchGameTimer(IDI_WAITTABLE_RAND_TABLE);
	KillMatchGameTimer(IDI_SWITCH_WAIT_TIME);
	KillMatchGameTimer(IDI_AllDismissGame);
	KillMatchGameTimer(IDI_LASTMATCHRESULT);
	KillMatchGameTimer(IDI_TabelRequestUser);
	
	if(m_enMatchStatus!=MS_NULL_STATUS)
	{
		m_enMatchStatus=MS_NULL_STATUS;
		m_pMatchSink->OnEventMatchOver(this);
	}
}

//定时排名游戏结束
void CChampionshipMatch::FirstRoundRuleTimeGameEnd(ITableFrameEx *pITableFrameEx)
{
	
}

//打立出局斗地主游戏结束
void CChampionshipMatch::FirstRoundRuleScoreGameEnd(ITableFrame *pITableFrame)
{
	for (WORD j=0;j<m_wChairCount;j++)
	{
		IServerUserItem *pUserItem=pITableFrame->GetTableUserItem(j);

		if(pUserItem!=NULL)
		{
			BYTE cbOutUserCount=0;
			const tagUserInfo *pScore=pUserItem->GetUserInfo();

			if (pScore->lScore<m_dwCurBase*m_pMatchRule.cbLeastScore/100)
			{
				TCHAR szReason[256]=TEXT("");
				_sntprintf(szReason, CountArray(szReason), TEXT("很遗憾，由于您的当前积分不足基数的%d%%，您被淘汰出局了。在本次比赛中获得第%d名。"),
					m_pMatchRule.cbLeastScore,m_OnMatchUserMap.GetCount());
				if(m_OnMatchUserMap.GetCount()<=m_pMatchRule.wRemainUserCount)
				{
					//这里判断一下 防止参数人数和停止淘汰人数一样不淘汰的问题
					CheckMatchStopOut(pITableFrame);
					InsertFreeGroup(pUserItem);
					continue;
				}
				m_pMatchSink->WriteUserAward(pUserItem, (WORD)m_OnMatchUserMap.GetCount(), m_dwMatchNO, szReason,this);
				m_OnMatchUserMap.RemoveKey(pUserItem->GetUserID());
				//预赛被淘汰的就离开座位
				pITableFrame->PerformStandUpAction(pUserItem);
				CheckMatchStopOut(pITableFrame);
			}
			else
			{
				LPCTSTR lpStr=TEXT("本局已结束，请稍候，系统配桌中...");
				m_pMatchSink->SendGameMessage(pUserItem,lpStr, SMT_CHAT|SMT_TABLE_ROLL);
				InsertFreeGroup(pUserItem);
			}
		}
	}
	if (ChackRoundTableFinish())
	{
		m_enMatchStatus=MS_START_NEXT_ROUND;
		SetMatchGameTimer(IDI_EnterNextRound,5000,1,0);
		return ;
	}
	//
	if(m_enMatchStatus!=MS_STOP_OUT)
	{
		SetMatchGameTimer(IDI_TabelRequestUser,5000,1,0);
		//PostMatchTimer(IDI_CONTINUE_GAME,5,(WPARAM)pITableFrame);
		//PostMatchTimer(IDI_EXIT_TABLE,5,(WPARAM)pITableFrame);
	}
}


//发送提示
VOID CChampionshipMatch::SendWaitTip(ITableFrame *pTableFrame)
{
	//状态校验
	if(m_enMatchStatus==MS_NULL_STATUS || m_enMatchStatus==MS_SIGNUPING )
		return;

	//变量定义
	WORD wPlaying=0;
	for(WORD i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if(m_MatchTableArray[i]->pTableFrame->GetGameStatus()>=GAME_STATUS_PLAY)
			wPlaying++;
	}

	//变量定义
	CMD_GR_Match_Wait_Tip WaitTip;
	ZeroMemory(&WaitTip,sizeof(WaitTip));
	WaitTip.wPlayingTable=wPlaying;
	WaitTip.wUserCount=(WORD)m_OnMatchUserMap.GetCount();
	lstrcpyn(WaitTip.szMatchName, m_pMatchRule.szMatchName,LEN_SERVER);

	if(m_enMatchStatus==MS_STOP_OUT || m_enMatchStatus==MS_SECOND_ROUND || m_enMatchStatus==MS_START_NEXT_ROUND)
	{
		for(WORD i=0;i<m_wChairCount;i++)
		{
			IServerUserItem *pTableUserItem=pTableFrame->GetTableUserItem(i);
			if(pTableUserItem==NULL) continue;
			WaitTip.wCurTableRank=GetUserRank(pTableUserItem,pTableFrame);
			if(WaitTip.wCurTableRank==INVALID_WORD) continue;

			
			WaitTip.wRank=GetUserRank(pTableUserItem);
			//if(m_enMatchStatus==MS_SECOND_ROUND && WaitTip.wCurTableRank==m_wChairCount) continue;
			WaitTip.lScore=pTableUserItem->GetUserScore();

			m_pMatchSink->SendData(pTableUserItem,MDM_GF_FRAME,SUB_GR_MATCH_WAIT_TIP,&WaitTip,sizeof(WaitTip));
		}
	}

	//更新本轮所以桌的比赛信息
	for(WORD i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if(pTableFrame->GetTableID()==m_MatchTableArray[i]->pTableFrame->GetTableID())continue;
		if(m_MatchTableArray[i]->pTableFrame->GetGameStatus()>=GAME_STATUS_PLAY) continue; 
		for(WORD j=0;j<m_wChairCount;j++)
		{
			IServerUserItem *pTableUserItem=m_MatchTableArray[i]->pTableFrame->GetTableUserItem(j);
			if(pTableUserItem==NULL) continue;
			WaitTip.wCurTableRank=GetUserRank(pTableUserItem,m_MatchTableArray[i]->pTableFrame);
			if(WaitTip.wCurTableRank==INVALID_WORD) continue;

			WaitTip.wRank=GetUserRank(pTableUserItem);
			//if(m_enMatchStatus==MS_SECOND_ROUND && WaitTip.wCurTableRank==m_wChairCount) continue;
			WaitTip.lScore=pTableUserItem->GetUserScore();

			m_pMatchSink->SendData(pTableUserItem,MDM_GF_FRAME,SUB_GR_MATCH_WAIT_TIP,&WaitTip,sizeof(WaitTip));
		}
	}
	//for(WORD i=0;i<m_wTableCount;i++)
	//{
	//	if(pTableFrame->GetTableID()==m_pAssignTable[i].pTableFrame->GetTableID())continue;
	//	if(m_pAssignTable[i].pTableFrame->GetGameStatus()>=GAME_STATUS_PLAY) continue; 
	//	if(m_pAssignTable[i].cbOnSeatUserCount==0) continue;

	//	for(WORD j=0;j<m_wChairCount;j++)
	//	{
	//		IServerUserItem *pTableUserItem=m_pAssignTable[i].pTableFrame->GetTableUserItem(j);
	//		if(pTableUserItem==NULL) continue;
	//		WaitTip.wCurTableRank=GetUserRank(pTableUserItem,m_pAssignTable[i].pTableFrame);
	//		if(WaitTip.wCurTableRank==INVALID_WORD) continue;

	//		WaitTip.wRank=GetUserRank(pTableUserItem)+1;
	//		if(m_enMatchStatus==MS_SECOND_ROUND && WaitTip.wCurTableRank==m_wChairCount) continue;
	//		WaitTip.lScore=pTableUserItem->GetUserScore();

	//		m_pMatchSink->SendData(pTableUserItem,MDM_GF_FRAME,SUB_GR_MATCH_WAIT_TIP,&WaitTip,sizeof(WaitTip));
	//	}
	//}
}
//发送提示
VOID CChampionshipMatch::SendWaitTipMJ(ITableFrame *pTableFrame)
{
	//状态校验
	if(m_enMatchStatus==MS_NULL_STATUS || m_enMatchStatus==MS_SIGNUPING )
		return;

	//变量定义
	WORD wPlaying=0;
	for(WORD i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if(m_MatchTableArray[i]->pTableFrame->GetGameStatus()>=GAME_STATUS_PLAY)
			wPlaying++;
	}

	//变量定义
	CMD_GR_Match_Wait_Tip WaitTip;
	ZeroMemory(&WaitTip,sizeof(WaitTip));
	WaitTip.wPlayingTable=wPlaying;
	WaitTip.wUserCount=(WORD)m_OnMatchUserMap.GetCount();
	lstrcpyn(WaitTip.szMatchName, m_pMatchRule.szMatchName,LEN_SERVER);

	if(m_enMatchStatus==MS_STOP_OUT || m_enMatchStatus==MS_SECOND_ROUND|| m_enMatchStatus==MS_START_NEXT_ROUND)
	{
		for(WORD i=0;i<m_wChairCount;i++)
		{
			IServerUserItem *pTableUserItem=pTableFrame->GetTableUserItem(i);
			if(pTableUserItem==NULL) continue;
			WaitTip.wCurTableRank=GetUserRankMJ(pTableUserItem,pTableFrame);
			if(WaitTip.wCurTableRank==INVALID_WORD) continue;
			WaitTip.wRank=GetUserRankMJ(pTableUserItem);
			WaitTip.lScore=pTableUserItem->GetUserInfo()->lGrade;//pTableUserItem->GetUserScore();
			m_pMatchSink->SendData(pTableUserItem,MDM_GF_FRAME,SUB_GR_MATCH_WAIT_TIP,&WaitTip,sizeof(WaitTip));
		}
	}

	//更新本轮所以桌的比赛信息
	for(WORD i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if(pTableFrame->GetTableID()==m_MatchTableArray[i]->pTableFrame->GetTableID())continue;
		if(m_MatchTableArray[i]->pTableFrame->GetGameStatus()>=GAME_STATUS_PLAY) continue; 
		for(WORD j=0;j<m_wChairCount;j++)
		{
			IServerUserItem *pTableUserItem=m_MatchTableArray[i]->pTableFrame->GetTableUserItem(j);
			if(pTableUserItem==NULL) continue;
			WaitTip.wCurTableRank=GetUserRankMJ(pTableUserItem,m_MatchTableArray[i]->pTableFrame);
			if(WaitTip.wCurTableRank==INVALID_WORD) continue;

			WaitTip.wRank=GetUserRankMJ(pTableUserItem);
			//if(m_enMatchStatus==MS_SECOND_ROUND && WaitTip.wCurTableRank==m_wChairCount) continue;
			WaitTip.lScore=pTableUserItem->GetUserInfo()->lGrade;//pTableUserItem->GetUserScore();

			m_pMatchSink->SendData(pTableUserItem,MDM_GF_FRAME,SUB_GR_MATCH_WAIT_TIP,&WaitTip,sizeof(WaitTip));
		}
	}
}
//空闲桌子拉空闲用户
void CChampionshipMatch::TabelRequestUser(bool bPrecedeSit)
{
	//参赛用户不够 椅子数量就要终止比赛
	if (CheckMatchUser())
	{
		return;
	}
	CTableFrameMananerArray			FreeTableArray;				//空比赛桌子
	//首先获取空桌子
	for (INT_PTR i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if(m_MatchTableArray[i]->pTableFrame->IsGameStarted()==false&&m_MatchTableArray[i]->bRoundTableFinish==false)
		{
			//获取数量
			INT_PTR cbTimer=m_MatchTimerArray.GetCount();
			bool cbIsInsert=false;
			for(int j=0;j<cbTimer;j++)
			{
				//判断一下有没有卡赛的桌子在等待
				if((m_MatchTimerArray[j]->dwTimerID==IDI_CheakTabelStart||m_MatchTimerArray[j]->dwTimerID==IDI_CONTINUE_GAME)&&(ITableFrame*)(m_MatchTimerArray[j]->wParam)==m_MatchTableArray[i]->pTableFrame)
				{
					cbIsInsert=true;
					break;
				}
			}
			if(cbIsInsert==false)
				FreeTableArray.Add(m_MatchTableArray[i]);
		}
	}
	DWORD dwCurTime=(DWORD)time(NULL);

	bool bIsContinueCheck=true;//优化优先坐下
	for (INT_PTR i=0;i<FreeTableArray.GetCount();i++)
	{
		DWORD nCurUserCount=(DWORD)m_FreeUserArray.GetCount();
		//找到一个空桌 空闲用户必须大于等于椅子
		if(nCurUserCount/m_wChairCount>0)
		{
			AllUserExitTable(FreeTableArray[i]->pTableFrame);
			for (int j=0;j<m_wChairCount;j++)
			{
				//用户插入该桌
				WORD randmer=INVALID_VALUE;
				if(bPrecedeSit&&bIsContinueCheck)
					randmer=GetPrecedeSitUser(dwCurTime);
				if(randmer==INVALID_VALUE)
				{
					bIsContinueCheck=false;//已经没有优先用户了
					randmer=rand()%(WORD)m_FreeUserArray.GetCount();
				}
				
				//先起立在坐下 让该桌的人全部起立
				if(m_FreeUserArray[randmer]->GetTableID()!=INVALID_TABLE)
				{
					for(int m=0;m<m_MatchTableArray.GetCount();m++)
					{
						if(m_FreeUserArray[randmer]->GetTableID()==m_MatchTableArray[m]->wTableID)
						{
							m_MatchTableArray[m]->pTableFrame->PerformStandUpAction(m_FreeUserArray[randmer]);
							//AllUserExitTable(m_MatchTableArray[m]->pTableFrame);
							break;
						}
					}
				}
				if (FreeTableArray[i]->pTableFrame->PerformSitDownAction(j,m_FreeUserArray[randmer]))
				{
					m_FreeUserArray.RemoveAt(randmer);
				}else
				{
					CTraceService::TraceString(TEXT("坐下失败？"),TraceLevel_Exception);
				}
			}
			//30秒后做一个检查 发现该桌还没有开始游戏 说明已经卡掉了
			PostMatchTimer(IDI_CheakTabelStart,30,(WPARAM)FreeTableArray[i]->pTableFrame);
		}
	}

	return ;
}
//发送一组信息
void CChampionshipMatch::SendGroupBox(LPCTSTR pStrMessage)
{
	//构造数据包
	CMD_CM_SystemMessage Message;
	Message.wType=SMT_EJECT|SMT_CHAT;
	lstrcpyn(Message.szString,pStrMessage,CountArray(Message.szString));
	Message.wLength=CountStringBuffer(Message.szString);
	//发送数据
	WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
	m_pMatchSink->SendGroupData(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&Message,wSendSize, this);
}

//设置晋级用户和淘汰用户 每桌第1名晋级
void CChampionshipMatch::SetEnterUserOROutUser(ITableFrame *pITableFrame)
{
	//计算晋级和淘汰用户
	CMatchUserItemArray UserItemArray;
	UserItemArray.RemoveAll();
	for (int i=0;i<m_wChairCount;i++)
	{
		IServerUserItem* UserItem=pITableFrame->GetTableUserItem(i);
		if(UserItem)
		{
			UserItemArray.Add(UserItem);
		}
	}
	if (UserItemArray.GetCount()>0)
	{
		SCORE dwMaxScore=UserItemArray.GetAt(0)->GetUserScore();
		//首先求得一组用户中最大分值
		for (int i=1;i<UserItemArray.GetCount();i++)
		{
			if(UserItemArray.GetAt(i)->GetUserScore()>dwMaxScore)dwMaxScore=UserItemArray.GetAt(i)->GetUserScore();
		}
		for (int i=0;i<UserItemArray.GetCount();i++)
		{
			//小于最大分值 将被转移预淘汰数组 是否直接转移空闲数组直接晋级
			if(UserItemArray.GetAt(i)->GetUserScore()<dwMaxScore)
			{
				if(m_ReOutUserArray.GetCount()==0)m_ReOutUserArray.Add(UserItemArray.GetAt(i));
				else
				{
					//准备被淘汰的 插入的时候 就排好
					BYTE ReOutUserCount=(BYTE)m_ReOutUserArray.GetCount();
					for (int j=0;j<ReOutUserCount;j++)
					{
						if(m_ReOutUserArray[j]->GetUserScore()<UserItemArray.GetAt(i)->GetUserScore())
						{
							m_ReOutUserArray.InsertAt(j,UserItemArray.GetAt(i));
							break;
						}
						if(j==ReOutUserCount-1)m_ReOutUserArray.Add(UserItemArray.GetAt(i));
					}
				}
			}
			else 
				InsertFreeGroup(UserItemArray.GetAt(i));
		}

	}
}

//设置晋级用户和淘汰用户 所有桌混合排名
void CChampionshipMatch::SetEnterUserOROutUser()
{
	DWORD dwUserID=0L;
	IServerUserItem *pIServerUserItem=NULL;
	POSITION pos=m_OnMatchUserMap.GetStartPosition();

	WORD wChairID=0;
	while (pos!=NULL)
	{
		//获取用户
		m_OnMatchUserMap.GetNextAssoc(pos,dwUserID,pIServerUserItem);
		//效验结果
		ASSERT(pIServerUserItem!=NULL);
		InsertFreeGroup(pIServerUserItem);
	}
	//DWORD dwUserID=0;
	//IServerUserItem *pUserItem=NULL;
	//tagMatchScore *pScore = new tagMatchScore[m_OnMatchUserMap.GetCount()];
	//ZeroMemory(pScore,sizeof(tagMatchScore)*m_OnMatchUserMap.GetCount());
	//WORD wCount=SortMapUser(pScore);

	//
	//for (WORD i=0;i<wCount;i++)
	//{
	//	
	//}
	//SafeDeleteArray(pScore);
}
//设置晋级用户 2人麻将比赛
void CChampionshipMatch::SetEnterUserMJ()
{
	INT_PTR nUserCount=m_OnMatchUserMap.GetCount();
	tagMatchRanking *pScore=new tagMatchRanking[nUserCount];
	ZeroMemory(pScore,sizeof(tagMatchRanking)*nUserCount);

	SortUserRanking(pScore,false);
	CString str;
	for (INT_PTR i=0;i<nUserCount;i++)
	{
		if(i>=(INT_PTR)m_pMatchRule.arrSecRoundUserCount[m_wCurRountSec])
		{
			str.Format(TEXT("抱歉，您未能成功晋级，在本次比赛中您获得第%d名"), i+1);
			//查找用户 删除空闲数组已被淘汰的用户 用来实现混合排名
			IServerUserItem * pIServerUserExist=NULL;
			m_OnMatchUserMap.Lookup(pScore[i].dwUserID,pIServerUserExist);
			for (int j=0;j<m_FreeUserArray.GetCount();j++)
			{
				if(m_FreeUserArray[j]==pIServerUserExist)
				{
					m_FreeUserArray.RemoveAt(j);
					break;
				}
			}
			m_OnMatchUserMap.RemoveKey(pScore[i].dwUserID);

			m_pMatchSink->WriteUserAward(pScore[i].pUserItem, (WORD)i+1, m_dwMatchNO, str,this);
			m_pMatchSink->SendData(pScore[i].pUserItem,MDM_GF_FRAME,SUB_GR_MATCH_WAIT_TIP,NULL,0);
		}
		else
		{
			str=TEXT("恭喜您晋级成功，决赛开始！");
			m_pMatchSink->SendGameMessage(pScore[i].pUserItem, str, SMT_CHAT|SMT_TABLE_ROLL);
		}
	}
	SafeDeleteArray(pScore);
}
//设置晋级用户 斗地主比赛
void CChampionshipMatch::SetEnterUserLand()
{
	INT_PTR nUserCount=m_OnMatchUserMap.GetCount();
	tagMatchRanking *pScore=new tagMatchRanking[nUserCount];
	ZeroMemory(pScore,sizeof(tagMatchRanking)*nUserCount);


	if(m_pMatchRule.cbSecScoreRule==0)		//按照每桌第1名排序
	{
		//空闲用户优先排序 加权
		for (int i=0;i<m_FreeUserArray.GetCount();i++)
		{
			m_FreeUserArray[i]->GetUserInfo()->lGrade=1;
		}
		SortUserRanking(pScore,false);
		//取消加权
		for (int i=0;i<m_FreeUserArray.GetCount();i++)
		{
			m_FreeUserArray[i]->GetUserInfo()->lGrade=0;
		}
	}else if(m_pMatchRule.cbSecScoreRule==1)//所有桌排序
	{
		SortUserRanking(pScore,true);
	}


	BYTE NullCount=0;
	CString str;
	for (INT_PTR i=0;i<nUserCount;i++)
	{
		if(pScore[i].pUserItem==NULL)
		{
			CTraceService::TraceString(TEXT("晋级时发现空对象"),TraceLevel_Exception);
			NullCount++;
			continue;
		}
		if(i+NullCount>=(INT_PTR)m_pMatchRule.arrSecRoundUserCount[m_wCurRountSec])
		{
			str.Format(TEXT("抱歉，您未能成功晋级，在本次比赛中您获得第%d名"), i+1);
			//查找用户 删除空闲数组已被淘汰的用户 用来实现混合排名
			IServerUserItem * pIServerUserExist=NULL;
			m_OnMatchUserMap.Lookup(pScore[i].dwUserID,pIServerUserExist);
			for (int j=0;j<m_FreeUserArray.GetCount();j++)
			{
				if(m_FreeUserArray[j]->GetUserID()==pIServerUserExist->GetUserID())
				{
					m_FreeUserArray.RemoveAt(j);
					break;
				}
			}
			m_OnMatchUserMap.RemoveKey(pScore[i].dwUserID);

			m_pMatchSink->WriteUserAward(pScore[i].pUserItem, (WORD)i+1, m_dwMatchNO, str,this);
			m_pMatchSink->SendData(pScore[i].pUserItem,MDM_GF_FRAME,SUB_GR_MATCH_WAIT_TIP,NULL,0);
		}
		else
		{
			str=TEXT("恭喜您晋级成功，决赛开始！");
			m_pMatchSink->SendGameMessage(pScore[i].pUserItem, str, SMT_CHAT|SMT_TABLE_ROLL);
		}
	}
	SafeDeleteArray(pScore);
}
//设置桌子完成标记
void CChampionshipMatch::SetRoundTableFinish(ITableFrame *pITableFrame)
{
	ITableFrameEx * pITableFrameEx=GetTableInterface(pITableFrame);
	ASSERT(pITableFrameEx);
	if (pITableFrameEx)pITableFrameEx->bRoundTableFinish=true;
}
//设置桌子完成标记
bool CChampionshipMatch::ChackRoundTableFinish()
{
	for (int i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if(m_MatchTableArray[i]->bRoundTableFinish==false)return false;
	}
	return true;
}
//设置桌子完成标记
void CChampionshipMatch::SetSwitchTableFinish(ITableFrame *pITableFrame)
{
	ITableFrameEx * pITableFrameEx=GetTableInterface(pITableFrame);
	ASSERT(pITableFrameEx);
	if (pITableFrameEx)pITableFrameEx->bSwtichTableFinish=true;
}
//设置桌子完成标记
bool CChampionshipMatch::CheckSwitchTableFinish()
{
	for (int i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if(m_MatchTableArray[i]->bSwtichTableFinish==false)return false;
	}
	return true;
}
//设置所有用户离开
void CChampionshipMatch::AllUserExitTable(ITableFrame *pITableFrame)
{
	if(pITableFrame)
	{
		//该桌用户全部离开
		for (int i=0;i<m_wChairCount;i++)
		{
			IServerUserItem* pUserItem=pITableFrame->GetTableUserItem(i);
			if(pUserItem&&pUserItem->GetTableID()!=INVALID_TABLE)pITableFrame->PerformStandUpAction(pUserItem);
		}
	}
}
//本桌继续游戏
void CChampionshipMatch::ContinueGame(ITableFrame *pITableFrame)
{
	if (CheckMatchUser())
	{
		return;
	}
	//加入m_enMatchStatus==MS_STOP_OUT 尝试解决用户卡线后影响整场比赛不复赛的问题
	if(m_FreeUserArray.GetCount()<m_wChairCount||m_enMatchStatus==MS_STOP_OUT)
	{
		SetSwitchTableFinish(pITableFrame);
		SetRoundTableFinish(pITableFrame);
		for (int i=0;i<m_FreeUserArray.GetCount();i++)
		{
			LPCTSTR lpStr=TEXT("继续游戏前，检查到人数不足，本轮已结束，请稍候，系统配桌中...");
			m_pMatchSink->SendGameMessage(m_FreeUserArray[i],lpStr, SMT_CHAT|SMT_TABLE_ROLL);
		}
		//AllUserExitTable(pITableFrame);
		if (ChackRoundTableFinish())
		{
			m_enMatchStatus=MS_START_NEXT_ROUND;
			SetMatchGameTimer(IDI_EnterNextRound,5000,1,0);
			return ;
		}
		return;
	}
	ITableFrameEx * pITableFrameEx=GetTableInterface(pITableFrame);
	ASSERT(pITableFrameEx);
	if (pITableFrameEx)
	{
		AllUserExitTable(pITableFrame);
		for(int i=0;i<m_wChairCount;i++)
		{
			IServerUserItem* pUserItem=m_FreeUserArray[0];//pITableFrameEx->m_FrontUserArray[i];
			//本桌缺人 完成上一局 在空闲时间离开比赛了
			if(pUserItem==NULL)
			{
				CTraceService::TraceString(TEXT("空的用户存在于等待列表中。"),TraceLevel_Exception);
				SetSwitchTableFinish(pITableFrame);
				SetRoundTableFinish(pITableFrame);
				//AllUserExitTable(pITableFrame);
				m_FreeUserArray.RemoveAt(0);
				if (ChackRoundTableFinish())
				{
					m_enMatchStatus=MS_START_NEXT_ROUND;
					SetMatchGameTimer(IDI_EnterNextRound,5000,1,0);
					return ;
				}
				return;
			}
			//for (int j=0;j<m_FreeUserArray.GetCount();j++)
			//{
			//	if(m_FreeUserArray[j]==pUserItem)
			//	{
			//		m_FreeUserArray.RemoveAt(j);
			//		break;
			//	}
			//}
			//该用户已经在椅子上 上次没有离开
			//if(pUserItem->GetTableID()!=INVALID_TABLE)
			//{
			//	pUserItem->SetUserStatus(US_READY,pITableFrame->GetTableID(),i);
			//}else
			//{
			//	pITableFrame->PerformSitDownAction(i,pUserItem);
			//}
			//先起立在坐下 让该桌的人全部起立
			BOOL bIsReady=false;
			if(pUserItem->GetTableID()!=INVALID_TABLE)
			{
				for(int m=0;m<m_MatchTableArray.GetCount();m++)
				{
					if(pUserItem->GetTableID()==m_MatchTableArray[m]->wTableID)
					{
						m_MatchTableArray[m]->pTableFrame->PerformStandUpAction(pUserItem);
						//if(pITableFrame->GetTableID()==pUserItem->GetTableID())
						//{
						//	if(pUserItem->SetUserStatus(US_READY,pITableFrame->GetTableID(),i))
						//	{
						//		bIsReady=true;
						//	}else
						//	{
						//		CTraceService::TraceString(TEXT("准备错误？"),TraceLevel_Exception);
						//	}
						//}
						//else
						//{
						//	AllUserExitTable(m_MatchTableArray[m]->pTableFrame);
						//}
						break;
					}
				}
			}

			if(bIsReady==false)
			{
				if (pITableFrame->PerformSitDownAction(i,pUserItem))
				{
					m_FreeUserArray.RemoveAt(0);
				}else
				{
					CTraceService::TraceString(TEXT("坐下失败？1"),TraceLevel_Exception);
				}
			}
		}
		////移除前几个
		//for (int j=0;j<m_wChairCount;j++)
		//{
		//	m_FreeUserArray.RemoveAt(0);
		//}
		//30秒后做一个检查 发现该桌还没有开始游戏 说明已经卡掉了
		PostMatchTimer(IDI_CheakTabelStart,30,(WPARAM)pITableFrame);
	}
}

//检查桌子是否开始游戏
void CChampionshipMatch::CheakTableStart(ITableFrame *pITableFrame)
{
	if(pITableFrame&&pITableFrame->IsGameStarted()==false)
	{
		//pITableFrame->DismissGame();

		//该桌用户全部离开
		for (int i=0;i<m_wChairCount;i++)
		{
			IServerUserItem* pUserItem=pITableFrame->GetTableUserItem(i);
			if(pUserItem&&pUserItem->GetTableID()!=INVALID_TABLE)
			{
				if(pUserItem->IsClientReady())	//已经启动客户端的
				{
					m_pMatchSink->SendGameMessage(pUserItem, TEXT("由于本桌卡赛，请等待再次为您配桌！"),SMT_CHAT|SMT_TABLE_ROLL);
					pITableFrame->PerformStandUpAction(pUserItem);
					InsertFreeGroup(pUserItem);
				}else							//没有启动客户端的
				{
					//删除用户
					m_OnMatchUserMap.RemoveKey(pUserItem->GetUserID());

					if (pUserItem->IsAndroidUser())
					{
						ASSERT(m_wAndroidUserCount>0);
						if (m_wAndroidUserCount>0) m_wAndroidUserCount--;
					}
					//清除比赛后的一些用户
					for (INT_PTR i=0;i<m_FreeUserArray.GetCount();i++)
					{
						if(m_FreeUserArray[i]->GetUserID()==pUserItem->GetUserID())
						{
							m_FreeUserArray.RemoveAt(i);
							break;
						}
					}
					for (INT_PTR i=0;i<m_ReOutUserArray.GetCount();i++)
					{
						if(m_ReOutUserArray[i]->GetUserID()==pUserItem->GetUserID())
						{
							m_ReOutUserArray.RemoveAt(i);
							break;
						}
					}

					m_pMatchSink->WriteUserAward(pUserItem,-1,m_dwMatchNO,TEXT("您出现异常卡线，为了保证比赛正常进行，系统将您退赛。"),this);
				}
			}
		}
		if(CheckMatchUser())return ;
		//如果是复赛这里就设置完成状态
		if(m_wCurRountSec>0)
		{
			SetRoundTableFinish(pITableFrame);
			if (ChackRoundTableFinish())
			{
				//排名
				if(m_wCurRountSec>=m_pMatchRule.cbSecRoundCount)
				{
					//比赛结束
					TCHAR szMsg[256]=TEXT("");
					_sntprintf(szMsg,CountArray(szMsg),TEXT("比赛结束，请等待系统为你排名！"));

					POSITION pos=m_OnMatchUserMap.GetStartPosition();
					IServerUserItem *pUser=NULL;
					DWORD dwUserID;
					while(pos!=NULL)
					{
						m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUser);
						m_pMatchSink->SendGameMessage(pUser,szMsg,SMT_CHAT|SMT_TABLE_ROLL);
					}

					SetMatchGameTimer(IDI_LASTMATCHRESULT,5000,1,(WPARAM)pITableFrame);
					return ;
				}
				m_enMatchStatus=MS_START_NEXT_ROUND;
				SetMatchGameTimer(IDI_EnterNextRound,5000,1,0);
				CheckExitMatch();
				return ;
			}
		}

		//PostMatchTimer(IDI_CONTINUE_GAME,5,(WPARAM)pITableFrame);
	}
}
//继续下一轮
void CChampionshipMatch::ContinueRound()
{
	BYTE FreeUserCount=(BYTE)m_FreeUserArray.GetCount();
	//进入下轮的3种情况
	if(FreeUserCount>=m_pMatchRule.arrSecRoundUserCount[m_wCurRountSec])
	{
		SelectPromotionUser();
	}else if (FreeUserCount<m_pMatchRule.arrSecRoundUserCount[m_wCurRountSec])
	{
		//人数不够就结算
		if((m_ReOutUserArray.GetCount()+FreeUserCount)<m_pMatchRule.arrSecRoundUserCount[m_wCurRountSec])
		{
			SendGroupBox(TEXT("抱歉，由于部分用户强行退出比赛，该场比赛提前结束。欢迎您继续报名参加比赛！"));
			LastMatchResult();
			return ;
		}
		for (int i=0;i<m_pMatchRule.arrSecRoundUserCount[m_wCurRountSec]-FreeUserCount;i++)
		{
			InsertFreeGroup(m_ReOutUserArray[0]);
			//将第一个移除
			m_ReOutUserArray.RemoveAt(0);
		}

		//进入第一种可能 
		SelectPromotionUser();
	}
	m_FreeUserArray.RemoveAll();
	m_ReOutUserArray.RemoveAll();
}

//设定比赛专用定时器
void CChampionshipMatch::SetMatchGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	if(m_pMatchSink)
		m_pMatchSink->SetGameTimer(dwTimerID+10*m_LoopTimer,dwElapse,dwRepeat,dwBindParameter,this);
}

//杀死比赛专用定时器
void CChampionshipMatch::KillMatchGameTimer(DWORD dwTimerID)
{
	if(m_pMatchSink)
		m_pMatchSink->KillGameTimer(dwTimerID+10*m_LoopTimer,this);
}

//最后结算
void CChampionshipMatch::LastMatchResult()
{
	WORD wCount=0;
	DWORD dwUserID=0;
	IServerUserItem *pUserItem=NULL;
	tagMatchRanking *pScore = new tagMatchRanking[m_OnMatchUserMap.GetCount()];
	ZeroMemory(pScore,sizeof(tagMatchRanking)*m_OnMatchUserMap.GetCount());
	if(m_pMatchRule.dwInitalScore==0)wCount=SortUserRanking(pScore,false);
	else wCount=SortUserRanking(pScore,true);
	for (WORD i=0;i<wCount;i++)
	{
		m_pMatchSink->WriteUserAward(pScore[i].pUserItem, i+1,m_dwMatchNO,NULL,this);
	}
	SafeDeleteArray(pScore);

	MatchOver();
}
//检测参赛人数
bool CChampionshipMatch::CheckMatchUser()
{
	//参赛用户不够 椅子数量就要终止比赛
	DWORD nCurUserCount=(DWORD)m_OnMatchUserMap.GetCount();
	if (nCurUserCount<m_wChairCount)
	{
		//构造数据包
		CString szString;
		szString.Format(TEXT("抱歉，由于比赛用户强行退出比赛，人数不足 %d 人，提前结算。欢迎您继续报名参加比赛！"),m_wChairCount);
		SendGroupBox(szString);
		LastMatchResult();
		return true;
	}
	return false;
}
//检查停止淘汰
void CChampionshipMatch::CheckMatchStopOut(ITableFrame *pITableFrame)
{
	INT_PTR nCount=m_OnMatchUserMap.GetCount();
	if(nCount<=m_pMatchRule.wRemainUserCount && m_enMatchStatus!=MS_STOP_OUT)
	{
		//把所有没开始的桌子打个标记
		KillMatchGameTimer(IDI_TabelRequestUser);
		m_enMatchStatus=MS_STOP_OUT;
		if(nCount<(INT_PTR)m_pMatchRule.dwSecRoundUserCount)
		{
			m_pMatchSink->SendGroupUserMessage(TEXT("复赛人数不够，比赛结束！你可以再次报名参加比赛"),this);
			LastMatchResult();
			return ;
		}
		TCHAR szMsg[256]=TEXT("");
		_sntprintf(szMsg,CountArray(szMsg),TEXT("已经达到淘汰人数，%d秒后即将结束初赛排名，确定晋级玩家！"),m_pMatchRule.wWaitTime);
		SetMatchGameTimer(IDI_AllDismissGame,m_pMatchRule.wWaitTime*1000,1,NULL);

		//将正在卡比赛的及时处理
		INT_PTR cbTimer=m_MatchTimerArray.GetCount();
		for(int j=0;j<cbTimer;j++)
		{
			//判断一下有没有卡赛的桌子在等待
			if(m_MatchTimerArray[j]->dwTimerID==IDI_CheakTabelStart)
			{
				CheakTableStart((ITableFrame*)(m_MatchTimerArray[j]->wParam));
				KillMatchTimer(j--);
			}
		}
		
		for (int m=0;m<m_MatchTableArray.GetCount();m++)
		{
			if (m_MatchTableArray[m]->pTableFrame->IsGameStarted()==false)
				m_MatchTableArray[m]->bRoundTableFinish=true;
		}
		SetRoundTableFinish(pITableFrame);

		SendWaitTip(pITableFrame);

		POSITION pos=m_OnMatchUserMap.GetStartPosition();
		IServerUserItem *pUser=NULL;
		DWORD dwUserID;
		while(pos!=NULL)
		{
			m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUser);
			m_pMatchSink->SendGameMessage(pUser,szMsg,SMT_CHAT|SMT_TABLE_ROLL);
		}
	}
}

//检测退賽
void CChampionshipMatch::CheckExitMatch()
{
	//解决中途退赛 先调用了退赛 在调用了游戏结束问题 又会被加入到 晋级组
	for (int i=0;i<m_FreeUserArray.GetCount();i++)
	{
		//查找用户
		IServerUserItem * pIServerUserExist=NULL;
		m_OnMatchUserMap.Lookup(m_FreeUserArray.GetAt(i)->GetUserID(),pIServerUserExist);

		//用户判断
		if(pIServerUserExist==NULL)
		{
			m_FreeUserArray.RemoveAt(i);
		}
	}
	for (int i=0;i<m_ReOutUserArray.GetCount();i++)
	{
		//查找用户
		IServerUserItem * pIServerUserExist=NULL;
		m_OnMatchUserMap.Lookup(m_ReOutUserArray.GetAt(i)->GetUserID(),pIServerUserExist);

		//用户判断
		if(pIServerUserExist==NULL)
		{
			m_ReOutUserArray.RemoveAt(i);
		}
	}
}

//获取优先坐下用户
WORD CChampionshipMatch::GetPrecedeSitUser(DWORD dwCurTime)
{
	for(int i=0;i<m_FreeUserArray.GetCount();i++)
	{
		tagTimeInfo* pTimerInfo=m_FreeUserArray[i]->GetTimeInfo();
		if(dwCurTime-pTimerInfo->dwEndGameTimer>m_pMatchRule.wPrecedeTimer)return i;
	}
	return INVALID_VALUE;
}

//实现该函数可保证Free组唯一
void CChampionshipMatch::InsertFreeGroup(IServerUserItem *pUserItem)
{
	if( pUserItem == NULL )return;
	for(int i=0;i<m_FreeUserArray.GetCount();i++)
	{
		if( m_FreeUserArray[i] != NULL && m_FreeUserArray[i]->GetUserID()==pUserItem->GetUserID())return;
	}
	m_FreeUserArray.Add(pUserItem);
}

//捕获比赛专用定时器
void CChampionshipMatch::CaptureMatchTimer()
{
	for(int i=0;i<m_MatchTimerArray.GetCount();i++)
	{
		if(--m_MatchTimerArray[i]->iElapse<=0)
		{
			if(m_MatchTimerArray[i]->dwTimerID==IDI_CONTINUE_GAME)
			{
				//本桌用户继续游戏
				ContinueGame((ITableFrame*)m_MatchTimerArray[i]->wParam);
			}
			else if(m_MatchTimerArray[i]->dwTimerID==IDI_EXIT_TABLE)
			{
				//离开本桌
				AllUserExitTable((ITableFrame*)m_MatchTimerArray[i]->wParam);
			}else if(m_MatchTimerArray[i]->dwTimerID==IDI_CheakTabelStart)
			{
				//检查桌子是否开始游戏
				CheakTableStart((ITableFrame*)m_MatchTimerArray[i]->wParam);
			}
			//删除定时器
			if(KillMatchTimer(i))i--;
			else ASSERT(false);
		}
	}
}
//投递比赛专用定时器
void CChampionshipMatch::PostMatchTimer(DWORD dwTimerID, int iElapse, WPARAM wParam, LPARAM lParam)
{
	tagMatchTimer* pMatchTimer=new tagMatchTimer;
	pMatchTimer->dwTimerID=dwTimerID;
	pMatchTimer->iElapse=iElapse;
	//pMatchTimer->dwRepeat=dwRepeat;
	pMatchTimer->wParam=wParam;
	pMatchTimer->lParam=lParam;
	m_MatchTimerArray.Add(pMatchTimer);
}
//杀死比赛专用定时器
bool CChampionshipMatch::KillMatchTimer(DWORD dwTimerID, WPARAM wParam)
{
	for(int i=0;i<m_MatchTimerArray.GetCount();i++)
	{
		if(m_MatchTimerArray[i]->dwTimerID==dwTimerID&&m_MatchTimerArray[i]->wParam==wParam)
		{
			tagMatchTimer* pMatchTimer=m_MatchTimerArray[i];
			m_MatchTimerArray.RemoveAt(i);
			SafeDelete(pMatchTimer);
			return true;
		}
	}
	return false;
}
//杀死比赛专用定时器
bool CChampionshipMatch::KillMatchTimer(INT_PTR dwIndexID)
{
	ASSERT(dwIndexID >= 0 && dwIndexID < m_MatchTimerArray.GetCount());
	if( dwIndexID >= 0 && dwIndexID < m_MatchTimerArray.GetCount() )
	{
		tagMatchTimer* pMatchTimer=m_MatchTimerArray[dwIndexID];
		m_MatchTimerArray.RemoveAt(dwIndexID);
		SafeDelete(pMatchTimer);
		return true;
	}
	return false;
}
//全部杀死比赛专用定时器
void CChampionshipMatch::AllKillMatchTimer()
{
	for(int i=0;i<m_MatchTimerArray.GetCount();/*i++*/)
	{
		tagMatchTimer* pMatchTimer=m_MatchTimerArray[i];
		m_MatchTimerArray.RemoveAt(i);
		SafeDelete(pMatchTimer);
	}
}
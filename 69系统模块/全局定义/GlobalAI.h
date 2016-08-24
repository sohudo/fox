#pragma	  once
#pragma pack(push)
#pragma pack(1)
//人工智能通用
class CAIUtil
{
public:

	//判断电脑玩家
	static bool	IsAIUser(TCHAR *szAccount)
	{
		static const TCHAR* szAIAccount = TEXT("computer");
		if (_tcslen(szAccount) < 8)
			return false;
		if (_tcsnicmp(szAccount + 1, szAIAccount, _tcslen(szAIAccount)) == 0)
			return true;
		else
			return false;
	}
	//判断电脑玩家
	static bool	IsAIUser(BOOL bAIFlag)
	{
		return (TRUE == bAIFlag) ? TRUE : FALSE;
	}
	//检查几率
	static bool DetectionProbability(BYTE r)
	{
		if ((rand() % 100) <= r)
			return true;
		else
			return false;
	}
};

#pragma pack(pop)
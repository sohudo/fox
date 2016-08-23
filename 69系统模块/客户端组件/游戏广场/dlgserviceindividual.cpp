#include "StdAfx.h"
#include "Resource.h"
#include "PlatformEvent.h"
#include "PlatformFrame.h"
#include "DlgCustomFace.h"
#include "DlgServiceIndividual.h"
#include ".\dlgserviceindividual.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServiceIndividual, CDlgServiceItem)
	//ON_BN_CLICKED(IDC_SELECT_FACE, OnBnClickedSelectFace)
	//ON_BN_CLICKED(IDC_BUTTON_COPY, OnBnClickedCopy)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedEdit)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServiceIndividual::CDlgServiceIndividual() : CDlgServiceItem(IDD_DLG_SERVICE_INDIVIDUAL)
{
	//设置图标
	m_cbImageIndex=0;

	//服务管理
	m_bQueryMission=false;
	m_bModifyMission=false;

	//设置任务
	m_MissionManager.InsertMissionItem(this);

	//帐号资料
	m_cbGender=0;
	ZeroMemory(m_szNickName,sizeof(m_szNickName));
	ZeroMemory(m_szPassword,sizeof(m_szPassword));
	ZeroMemory(m_szUnderWrite,sizeof(m_szUnderWrite));

	//详细资料
	ZeroMemory(m_szQQ,sizeof(m_szQQ));
	ZeroMemory(m_szEMail,sizeof(m_szEMail));
	ZeroMemory(m_szUserNote,sizeof(m_szUserNote));
	ZeroMemory(m_szSeatPhone,sizeof(m_szSeatPhone));
	ZeroMemory(m_szMobilePhone,sizeof(m_szMobilePhone));
	ZeroMemory(m_szCompellation,sizeof(m_szCompellation));
	ZeroMemory(m_szPassPortID,sizeof(m_szPassPortID));
	ZeroMemory(m_szDwellingPlace,sizeof(m_szDwellingPlace));

	return;
}

//析构函数
CDlgServiceIndividual::~CDlgServiceIndividual()
{
}

//控件绑定
VOID CDlgServiceIndividual::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//其他控件
	DDX_Control(pDX, IDOK, m_btOk);
	//DDX_Control(pDX, IDC_FACE_ITEM, m_FaceItemView);
	//DDX_Control(pDX, IDC_SELECT_FACE, m_btSelectFace);
	//DDX_Control(pDX, IDC_BUTTON_COPY, m_btCopy);
	DDX_Control(pDX, IDC_BUTTON_EDIT, m_btEdit);

	//编辑控件
	DDX_Control(pDX, IDC_GAMEID, m_edGameID);
	DDX_Control(pDX, IDC_ACCOUNTS, m_edAccounts);
	DDX_Control(pDX, IDC_NICKNAME, m_edNickName);
	DDX_Control(pDX, IDC_PASSWORD, m_edPassword);
	DDX_Control(pDX, IDC_UNDER_WRITE, m_edUnderWrite);
	DDX_Control(pDX, IDC_UNDER_WRITE2,m_edUnderWrite2);

	DDX_Control(pDX, IDC_GENDER,m_edGender);
	DDX_Control(pDX, IDC_MEMBER,m_edMember);
	DDX_Control(pDX, IDC_MEDAL,m_edMedal);

	//编辑控件
	DDX_Control(pDX, IDC_QQ, m_edQQ);
	DDX_Control(pDX, IDC_EMAIL, m_edEMail);
	DDX_Control(pDX, IDC_USER_NOTE, m_edUserNote);
	DDX_Control(pDX, IDC_SEAT_PHONE, m_edSeatPhone);
	DDX_Control(pDX, IDC_MOBILE_PHONE, m_edMobilePhone);
	DDX_Control(pDX, IDC_PASS_PORT_ID, m_edPassPortID);
	DDX_Control(pDX, IDC_COMPELLATION, m_edCompellation);
	DDX_Control(pDX, IDC_DWELLING_PLACE, m_edDwellingPlace);
}

//创建函数
BOOL CDlgServiceIndividual::OnInitDialog()
{
	__super::OnInitDialog();

	m_ImageBack.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_PAGE_1"));
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE);

	m_btCopy.SetButtonImage(IDB_BT_COPY,AfxGetInstanceHandle(),false,false);
	m_btEdit.SetButtonImage(IDB_BT_EDIT,AfxGetInstanceHandle(),false,false);
	m_btOk.SetButtonImage(IDB_BT_UNDERLINE,AfxGetInstanceHandle(),false,false);


	m_bEnableEdit=false;
	m_btEdit.ShowWindow(SW_SHOW);
	m_btOk.ShowWindow(SW_HIDE);

	m_edUnderWrite2.ShowWindow(SW_HIDE);
	m_edUnderWrite.ShowWindow(SW_SHOW);


	//帐号信息
	m_edAccounts.LimitText(LEN_ACCOUNTS-1);
	m_edNickName.LimitText(LEN_NICKNAME-1);
	m_edPassword.LimitText(LEN_PASSWORD-1);
	m_edUnderWrite.LimitText(LEN_UNDER_WRITE-1);
	m_edUnderWrite2.LimitText(LEN_UNDER_WRITE-1);
	m_edPassPortID.LimitText(LEN_PASS_PORT_ID-1);

	//限制输入
	m_edQQ.LimitText(LEN_QQ-1);
	m_edEMail.LimitText(LEN_EMAIL-1);
	m_edUserNote.LimitText(LEN_USER_NOTE-1);
	m_edSeatPhone.LimitText(LEN_SEAT_PHONE-1);
	m_edMobilePhone.LimitText(LEN_MOBILE_PHONE-1);
	m_edCompellation.LimitText(LEN_COMPELLATION-1);
	m_edDwellingPlace.LimitText(LEN_DWELLING_PLACE-1);

	////信息控件
	//m_edAccounts.EnableWindow(FALSE);
	//m_edNickName.EnableWindow(FALSE);
	//m_edPassword.EnableWindow(FALSE);
	//m_edUnderWrite.EnableWindow(FALSE);

	////详细资料
	//m_edQQ.EnableWindow(FALSE);
	//m_edEMail.EnableWindow(FALSE);
	//m_edUserNote.EnableWindow(FALSE);
	//m_edSeatPhone.EnableWindow(FALSE);
	//m_edMobilePhone.EnableWindow(FALSE);
	//m_edCompellation.EnableWindow(FALSE);
	//m_edDwellingPlace.EnableWindow(FALSE);

	m_edGameID.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edAccounts.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edNickName.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edPassword.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edUnderWrite.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edQQ.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edEMail.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edUserNote.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edSeatPhone.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edMobilePhone.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edCompellation.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edDwellingPlace.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edPassPortID.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edGender.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edMember.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edMedal.SetDisableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));

	m_edUnderWrite2.SetDisableColor(RGB(10,10,10),RGB(255,255,255),RGB(131,192,219));

	m_edGameID.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edAccounts.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edNickName.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edPassword.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edUnderWrite.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edQQ.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edEMail.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edUserNote.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edSeatPhone.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edMobilePhone.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edCompellation.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edDwellingPlace.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edPassPortID.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));

	m_edUnderWrite2.SetEnableColor(RGB(10,10,10),RGB(255,255,255),RGB(131,192,219));

	m_edGender.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edMember.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));
	m_edMedal.SetEnableColor(RGB(10,10,10),RGB(250,250,250),RGB(250,250,250));

	//加载信息
	LoadAccountsInfo();

	return TRUE;
}

//确定函数
VOID CDlgServiceIndividual::OnOK()
{
	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//获取密码
	TCHAR szPassword[LEN_PASSWORD]=TEXT("");
	GetDlgItemText(IDC_PASSWORD,szPassword,CountArray(szPassword));

	//帐号信息
	GetControlItemText(IDC_NICKNAME,m_szNickName,CountArray(m_szNickName));
	GetControlItemText(IDC_UNDER_WRITE,m_szUnderWrite,CountArray(m_szUnderWrite));
	GetControlItemText(IDC_UNDER_WRITE2,m_szUnderWrite,CountArray(m_szUnderWrite));

	//用户资料
	GetDlgItemText(IDC_USER_NOTE,m_szUserNote,CountArray(m_szUserNote));
	GetDlgItemText(IDC_COMPELLATION,m_szCompellation,CountArray(m_szCompellation));
	GetDlgItemText(IDC_PASS_PORT_ID,m_szPassPortID,CountArray(m_szPassPortID));

	//电话号码
	GetDlgItemText(IDC_SEAT_PHONE,m_szSeatPhone,CountArray(m_szSeatPhone));
	GetDlgItemText(IDC_MOBILE_PHONE,m_szMobilePhone,CountArray(m_szMobilePhone));

	//联系资料
	GetDlgItemText(IDC_QQ,m_szQQ,CountArray(m_szQQ));
	GetDlgItemText(IDC_EMAIL,m_szEMail,CountArray(m_szEMail));
	GetDlgItemText(IDC_DWELLING_PLACE,m_szDwellingPlace,CountArray(m_szDwellingPlace));

	//用户性别
	if (((CButton *)GetDlgItem(IDC_GENDER_FEMALE))->GetCheck()==BST_CHECKED) m_cbGender=GENDER_FEMALE;
	if (((CButton *)GetDlgItem(IDC_GENDER_MANKIND))->GetCheck()==BST_CHECKED) m_cbGender=GENDER_MANKIND;

	//昵称判断
	if (lstrcmp(m_szNickName,pGlobalUserData->szNickName)!=0)
	{
		//变量定义
		TCHAR szDescribe[128]=TEXT("");
		CUserItemElement * pUserItemElement=CUserItemElement::GetInstance();

		//昵称判断
		if (pUserItemElement->EfficacyNickName(m_szNickName,szDescribe,CountArray(szDescribe))==false)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

			//设置焦点
			m_edNickName.SetFocus();

			return;
		}
	}

	//加密密码
	//CWHEncrypt::MD5Encrypt(szPassword,m_szPassword);

	////密码判断
	//if (lstrcmp(m_szPassword,pGlobalUserData->szPassword)!=0)
	//{
	//	//提示消息
	//	CInformation Information(this);
	//	Information.ShowMessageBox(TEXT("用户帐号密码错误，请重新输入！"),MB_ICONERROR,0);

	//	//设置焦点
	//	m_edPassword.SetFocus();

	//	return;
	//}

	lstrcpy(m_szPassword,pGlobalUserData->szPassword);

	//设置变量
	m_bQueryMission=false;
	m_bModifyMission=true;

	//事件处理
	OnMissionStart();

	//发起连接
	if (m_MissionManager.AvtiveMissionItem(this,false)==false) 
	{
		OnMissionConclude();
		return;
	}

	return;
}

//连接事件
bool CDlgServiceIndividual::OnEventMissionLink(INT nErrorCode)
{
	//连接结果
	if (nErrorCode!=0)
	{
		//重试任务
		if (m_MissionManager.AvtiveMissionItem(this,true)==true)
		{
			return true;
		}

		//事件处理
		OnMissionConclude();

		//错误提示
		if (m_bQueryMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("连接服务器超时，用户资料获取失败！"),MB_OK|MB_ICONSTOP,30);
		}

		//错误提示
		if (m_bModifyMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("连接服务器超时，用户资料修改失败！"),MB_OK|MB_ICONSTOP,30);
		}

		return true;
	}

	//修改任务
	if (m_bModifyMission==true)
	{
		//变量定义
		BYTE cbBuffer[SOCKET_TCP_BUFFER];
		ZeroMemory(cbBuffer,sizeof(cbBuffer));

		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//变量定义
		CMD_GP_ModifyIndividual * pModifyIndividual=(CMD_GP_ModifyIndividual *)cbBuffer;
		CSendPacketHelper SendPacket(cbBuffer+sizeof(CMD_GP_ModifyIndividual),sizeof(cbBuffer)-sizeof(CMD_GP_ModifyIndividual));

		//设置变量
		pModifyIndividual->cbGender=m_cbGender;
		pModifyIndividual->dwUserID=pGlobalUserData->dwUserID;
		lstrcpyn(pModifyIndividual->szPassword,m_szPassword,CountArray(pModifyIndividual->szPassword));

		//用户昵称
		if (m_szNickName[0]!=0)
		{
			SendPacket.AddPacket(m_szNickName,DTP_GP_UI_NICKNAME);
		}

		//个性签名
		if (m_szUnderWrite[0]!=0)
		{
			SendPacket.AddPacket(m_szUnderWrite,DTP_GP_UI_UNDER_WRITE);
		}

		//用户备注
		if (m_szUserNote[0]!=0)
		{
			SendPacket.AddPacket(m_szUserNote,DTP_GP_UI_USER_NOTE);
		}

		//真实名字
		if (m_szCompellation[0]!=0) 
		{
			SendPacket.AddPacket(m_szCompellation,DTP_GP_UI_COMPELLATION);
		}

		////身份证号
		//if (m_szPassPortID[0]!=0)
		//{
		//	SendPacket.AddPacket(m_szPassPortID,DTP_GP_UI_PASSPORT_ID);
		//}

		//固定号码
		if (m_szSeatPhone[0]!=0) 
		{
			SendPacket.AddPacket(m_szSeatPhone,DTP_GP_UI_SEAT_PHONE);
		}

		//手机号码
		if (m_szMobilePhone[0]!=0)
		{
			SendPacket.AddPacket(m_szMobilePhone,DTP_GP_UI_MOBILE_PHONE);
		}

		//Q Q 号码
		if (m_szQQ[0]!=0) 
		{
			SendPacket.AddPacket(m_szQQ,DTP_GP_UI_QQ);
		}

		//电子邮件
		if (m_szEMail[0]!=0) 
		{
			SendPacket.AddPacket(m_szEMail,DTP_GP_UI_EMAIL);
		}

		//详细地址
		if (m_szDwellingPlace[0]!=0) 
		{
			SendPacket.AddPacket(m_szDwellingPlace,DTP_GP_UI_DWELLING_PLACE);
		}

		//发送数据
		WORD wSendSize=sizeof(CMD_GP_ModifyIndividual)+SendPacket.GetDataSize();
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_MODIFY_INDIVIDUAL,cbBuffer,wSendSize);
	}

	//查询任务
	if (m_bQueryMission==true)
	{
		//变量定义
		CMD_GP_QueryIndividual QueryIndividual;
		ZeroMemory(&QueryIndividual,sizeof(QueryIndividual));

		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//设置变量
		QueryIndividual.dwUserID=pGlobalUserData->dwUserID;
		lstrcpyn(QueryIndividual.szPassword,pGlobalUserData->szPassword,CountArray(QueryIndividual.szPassword));

		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_QUERY_INDIVIDUAL,&QueryIndividual,sizeof(QueryIndividual));
	}

	return true;
}

//关闭事件
bool CDlgServiceIndividual::OnEventMissionShut(BYTE cbShutReason)
{
	//提示消息
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//重试任务
		if (m_MissionManager.AvtiveMissionItem(this,true)==true)
		{
			return true;
		}

		//查询任务
		if (m_bQueryMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("与服务器的连接异常断开了，用户资料获取失败！"),MB_ICONERROR,30);
		}

		//修改任务
		if (m_bModifyMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("与服务器的连接异常断开了，用户资料修改失败！"),MB_ICONERROR,30);
		}
	}

	//事件处理
	OnMissionConclude();

	return true;
}

//读取事件
bool CDlgServiceIndividual::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//命令处理
	if (Command.wMainCmdID==MDM_GP_USER_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_OPERATE_FAILURE:		//操作失败
			{
				//效验参数
				CMD_GP_OperateFailure * pOperateFailure=(CMD_GP_OperateFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString))) return false;

				//失败处理
				OnMissionConclude();

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);

				//显示消息
				if (pOperateFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pOperateFailure->szDescribeString,MB_ICONERROR,60);
				}

				return true;
			}
		case SUB_GP_OPERATE_SUCCESS:	//操作成功
			{
				//变量定义
				CMD_GP_OperateSuccess * pOperateSuccess=(CMD_GP_OperateSuccess *)pData;

				//效验数据
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString))) return false;

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);

				//变量定义
				CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
				tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
				tagIndividualUserData * pIndividualUserData=pGlobalUserInfo->GetIndividualUserData();

				//帐号资料
				pGlobalUserData->cbGender=m_cbGender;
				lstrcpyn(pGlobalUserData->szNickName,m_szNickName,CountArray(pGlobalUserData->szNickName));
				lstrcpyn(pGlobalUserData->szUnderWrite,m_szUnderWrite,CountArray(pGlobalUserData->szUnderWrite));

				//详细资料
				lstrcpyn(pIndividualUserData->szQQ,m_szQQ,CountArray(pIndividualUserData->szQQ));
				lstrcpyn(pIndividualUserData->szEMail,m_szEMail,CountArray(pIndividualUserData->szEMail));
				lstrcpyn(pIndividualUserData->szUserNote,m_szUserNote,CountArray(pIndividualUserData->szUserNote));
				lstrcpyn(pIndividualUserData->szSeatPhone,m_szSeatPhone,CountArray(pIndividualUserData->szSeatPhone));
				lstrcpyn(pIndividualUserData->szMobilePhone,m_szMobilePhone,CountArray(pIndividualUserData->szMobilePhone));
				lstrcpyn(pIndividualUserData->szCompellation,m_szCompellation,CountArray(pIndividualUserData->szCompellation));
				//lstrcpyn(pIndividualUserData->szPassPortID,m_szPassPortID,CountArray(pIndividualUserData->szPassPortID));
				lstrcpyn(pIndividualUserData->szDwellingPlace,m_szDwellingPlace,CountArray(pIndividualUserData->szDwellingPlace));

				//发送事件
				CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
				if (pPlatformEvent!=NULL) pPlatformEvent->SendPlatformEvent(EVENT_USER_INFO_UPDATE,0L);

				//显示消息
				if (pOperateSuccess->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pOperateSuccess->szDescribeString,MB_ICONINFORMATION,60);
				}

				//成功处理
				OnMissionConclude();

				return true;
			}
		case SUB_GP_USER_INDIVIDUAL:	//个人信息
			{
				//效验参数
				ASSERT(wDataSize>=sizeof(CMD_GP_UserIndividual));
				if (wDataSize<sizeof(CMD_GP_UserIndividual)) return false;

				//变量定义
				CMD_GP_UserIndividual * pUserIndividual=(CMD_GP_UserIndividual *)pData;

				//变量定义
				CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
				tagIndividualUserData * pIndividualUserData=pGlobalUserInfo->GetIndividualUserData();

				//设置变量
				pIndividualUserData->dwUserID=pUserIndividual->dwUserID;
				
				//变量定义
				VOID * pDataBuffer=NULL;
				tagDataDescribe DataDescribe;
				CRecvPacketHelper RecvPacket(pUserIndividual+1,wDataSize-sizeof(CMD_GP_UserIndividual));

				//扩展信息
				while (true)
				{
					pDataBuffer=RecvPacket.GetData(DataDescribe);
					if (DataDescribe.wDataDescribe==DTP_NULL) break;
					switch (DataDescribe.wDataDescribe)
					{
					case DTP_GP_UI_USER_NOTE:		//用户备注
						{
							lstrcpyn(pIndividualUserData->szUserNote,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szUserNote));
							break;
						}
					case DTP_GP_UI_COMPELLATION:	//真实名字
						{
							lstrcpyn(pIndividualUserData->szCompellation,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szCompellation));
							break;
						}
					//case DTP_GP_UI_PASSPORT_ID:		//身份证号
					//	{
					//		lstrcpyn(pIndividualUserData->szPassPortID,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szPassPortID));
					//		break;
					//	}
					case DTP_GP_UI_SEAT_PHONE:		//固定电话
						{
							lstrcpyn(pIndividualUserData->szSeatPhone,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szSeatPhone));
							break;
						}
					case DTP_GP_UI_MOBILE_PHONE:	//移动电话
						{
							lstrcpyn(pIndividualUserData->szMobilePhone,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szMobilePhone));
							break;
						}
					case DTP_GP_UI_QQ:				//Q Q 号码
						{
							lstrcpyn(pIndividualUserData->szQQ,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szQQ));
							break;
						}
					case DTP_GP_UI_EMAIL:			//电子邮件
						{
							lstrcpyn(pIndividualUserData->szEMail,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szEMail));
							break;
						}
					case DTP_GP_UI_DWELLING_PLACE:	//联系地址
						{
							lstrcpyn(pIndividualUserData->szDwellingPlace,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szDwellingPlace));
							break;
						}
					}
				}

				//成功处理
				OnMissionConclude();

				//更新资料
				UpdateIndividualInfo();

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);

				return true;
			}
		}
	}

	return true;
}

//开始任务
VOID CDlgServiceIndividual::OnMissionStart()
{
	////信息控件
	//m_edAccounts.EnableWindow(FALSE);
	//m_edNickName.EnableWindow(FALSE);
	//m_edPassword.EnableWindow(FALSE);
	//m_edUnderWrite.EnableWindow(FALSE);

	////详细资料
	//m_edQQ.EnableWindow(FALSE);
	//m_edEMail.EnableWindow(FALSE);
	//m_edUserNote.EnableWindow(FALSE);
	//m_edSeatPhone.EnableWindow(FALSE);
	//m_edMobilePhone.EnableWindow(FALSE);
	//m_edCompellation.EnableWindow(FALSE);
	//m_edDwellingPlace.EnableWindow(FALSE);

	////功能按钮
	//m_btOk.EnableWindow(FALSE);
	//m_btSelectFace.EnableWindow(FALSE);

	return;
}

//终止任务
VOID CDlgServiceIndividual::OnMissionConclude()
{
	////信息控件
	//m_edAccounts.EnableWindow(TRUE);
	//m_edNickName.EnableWindow(TRUE);
	//m_edPassword.EnableWindow(TRUE);
	//m_edUnderWrite.EnableWindow(TRUE);

	////详细资料
	//m_edQQ.EnableWindow(TRUE);
	//m_edEMail.EnableWindow(TRUE);
	//m_edUserNote.EnableWindow(TRUE);
	//m_edSeatPhone.EnableWindow(TRUE);
	//m_edMobilePhone.EnableWindow(TRUE);
	//m_edCompellation.EnableWindow(TRUE);
	//m_edDwellingPlace.EnableWindow(TRUE);

	////功能按钮
	//m_btOk.EnableWindow(TRUE);
	//m_btSelectFace.EnableWindow(TRUE);

	////设置控件
	//m_edPassword.SetWindowText(TEXT(""));

	return;
}

//帐号信息
VOID CDlgServiceIndividual::LoadAccountsInfo()
{
	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
	tagIndividualUserData * pIndividualUserData=pGlobalUserInfo->GetIndividualUserData();

	//用户头像
	if ((pGlobalUserData->dwCustomID!=0L)&&(pGlobalUserData->CustomFaceInfo.dwDataSize!=0))
	{
		tagCustomFaceInfo * pCustomFaceInfo=&pGlobalUserData->CustomFaceInfo;
		m_FaceItemView.SetCustomFace(pGlobalUserData->dwCustomID,pCustomFaceInfo->dwCustomFace);
	}
	else
	{
		m_FaceItemView.SetSystemFace(pGlobalUserData->wFaceID);
	}

	//基本资料
	SetDlgItemInt(IDC_GAMEID,pGlobalUserData->dwGameID);
	SetDlgItemText(IDC_ACCOUNTS,pGlobalUserData->szAccounts);
	SetDlgItemText(IDC_NICKNAME,pGlobalUserData->szNickName);
	SetDlgItemText(IDC_UNDER_WRITE,pGlobalUserData->szUnderWrite);
	SetDlgItemText(IDC_UNDER_WRITE2,pGlobalUserData->szUnderWrite);

	LPCTSTR pszMember[]={TEXT("普通会员"),TEXT("蓝钻会员"),TEXT("黄钻会员"),TEXT("白钻会员"),TEXT("红钻会员"),TEXT("VIP会员")};
	SetDlgItemText(IDC_MEMBER,pszMember[pGlobalUserData->cbMemberOrder]);
	
	//用户性别
	if (pGlobalUserData->cbGender==GENDER_FEMALE){ ((CButton *)GetDlgItem(IDC_GENDER_FEMALE))->SetCheck(BST_CHECKED);	SetDlgItemText(IDC_GENDER,TEXT("女"));}
	if (pGlobalUserData->cbGender==GENDER_MANKIND){ ((CButton *)GetDlgItem(IDC_GENDER_MANKIND))->SetCheck(BST_CHECKED); SetDlgItemText(IDC_GENDER,TEXT("男"));}

	SetDlgItemText(IDC_MEDAL,TEXT("0"));

	//详细资料
	if (pIndividualUserData->dwUserID==0L)
	{
		QueryIndividualInfo();
	}
	else
	{
		UpdateIndividualInfo();
	}

	return;
}

//查询资料
VOID CDlgServiceIndividual::QueryIndividualInfo()
{
	//设置变量
	m_bQueryMission=true;
	m_bModifyMission=false;

	//事件处理
	OnMissionStart();

	//发起连接
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return;
}

//更新资料
VOID CDlgServiceIndividual::UpdateIndividualInfo()
{
	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagIndividualUserData * pIndividualUserData=pGlobalUserInfo->GetIndividualUserData();

	//用户资料
	SetDlgItemText(IDC_USER_NOTE,pIndividualUserData->szUserNote);
	SetDlgItemText(IDC_COMPELLATION,pIndividualUserData->szCompellation);

	//电话号码
	SetDlgItemText(IDC_SEAT_PHONE,pIndividualUserData->szSeatPhone);
	SetDlgItemText(IDC_MOBILE_PHONE,pIndividualUserData->szMobilePhone);

	//联系资料
	SetDlgItemText(IDC_QQ,pIndividualUserData->szQQ);
	SetDlgItemText(IDC_EMAIL,pIndividualUserData->szEMail);
	SetDlgItemText(IDC_DWELLING_PLACE,pIndividualUserData->szDwellingPlace);

	return;
}

//
VOID CDlgServiceIndividual::OnBnClickedEdit()
{
	//((CEdit*)GetDlgItem(IDC_UNDER_WRITE))->SetReadOnly(FALSE);

	m_bEnableEdit=true;
	m_btEdit.ShowWindow(SW_HIDE);
	m_btOk.ShowWindow(SW_SHOW);

	m_edUnderWrite2.ShowWindow(SW_SHOW);
	m_edUnderWrite.ShowWindow(SW_HIDE);

}

//复制ID昵称
VOID CDlgServiceIndividual::OnBnClickedCopy()
{
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	char szSource[100]=("");
	_snprintf(szSource,sizeof(szSource),"我在游戏中心的用户ID为：%ld,昵称为：%s,请核对!",pGlobalUserData->dwGameID,CT2CA(pGlobalUserData->szNickName));

	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(szSource)+1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, szSource);
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("提示消息"),TEXT("复制成功"),MB_ICONINFORMATION,10);
}

//头像选择
VOID CDlgServiceIndividual::OnBnClickedSelectFace()
{
	//变量定义
	CDlgCustomFace DlgCustomFace;
	DlgCustomFace.SetCustomFaceEvent(QUERY_OBJECT_PTR_INTERFACE(CPlatformFrame::GetInstance(),IUnknownEx));

	//显示窗口
	if (DlgCustomFace.DoModal()==IDCANCEL)
	{
		return;
	}

	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
	tagIndividualUserData * pIndividualUserData=pGlobalUserInfo->GetIndividualUserData();

	//用户头像
	if ((pGlobalUserData->dwCustomID!=0L)&&(pGlobalUserData->CustomFaceInfo.dwDataSize!=0))
	{
		tagCustomFaceInfo * pCustomFaceInfo=&pGlobalUserData->CustomFaceInfo;
		m_FaceItemView.SetCustomFace(pGlobalUserData->dwCustomID,pCustomFaceInfo->dwCustomFace);
	}
	else
	{
		m_FaceItemView.SetSystemFace(pGlobalUserData->wFaceID);
	}

	return;
}

BOOL CDlgServiceIndividual::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if(m_ImageBack.IsNull()!=true)
	m_ImageBack.DrawImage(pDC,0,0);

	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
	IFaceItemControl * pIFaceItemControl=CFaceItemControl::GetInstance();
	pIFaceItemControl->DrawFaceBigNormal(pDC,352,14,pGlobalUserInfo->GetGlobalUserData()->cbGender);

	//if(m_bEnableEdit)
	//{
	//	CRect rcEdit;
	//	GetDlgItem(IDC_UNDER_WRITE2)->GetWindowRect(&rcEdit);
	//	ScreenToClient(&rcEdit);
	//	rcEdit.left-=2;
	//	rcEdit.top-=2;
	//	rcEdit.right+=2;
	//	rcEdit.bottom+=2;

	//	pDC->Draw3dRect(&rcEdit,RGB(131,192,219),RGB(131,192,219));
	//}

	return TRUE;

	//return __super::OnEraseBkgnd(pDC);
}
//////////////////////////////////////////////////////////////////////////////////



HBRUSH CDlgServiceIndividual::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	static CBrush crBrush;
	if(crBrush.GetSafeHandle()==NULL)	crBrush.CreateSolidBrush(RGB(250,250,250));
	if(nCtlColor==CTLCOLOR_STATIC && pWnd!=&m_edUnderWrite2)
	{
		return crBrush;
	}

	return hbr;
}

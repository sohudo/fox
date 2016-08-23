#ifndef SERVICE_INDIVIDUAL_HEAD_FILE
#define SERVICE_INDIVIDUAL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgServiceItem.h"
#include "SkinEditEx2.h"
//////////////////////////////////////////////////////////////////////////////////

//修改资料
class CDlgServiceIndividual : public CDlgServiceItem, public CMissionItem
{
	//友元定义
	friend class CDlgService;

	//变量定义
protected:
	bool							m_bQueryMission;						//查询任务
	bool							m_bModifyMission;						//修改任务
	CMissionManager					m_MissionManager;						//任务管理

	CPngImage						m_ImageBack;

	//帐号资料
protected:
	BYTE							m_cbGender;								//用户性别
	TCHAR							m_szNickName[LEN_NICKNAME];				//游戏昵称
	TCHAR							m_szPassword[LEN_PASSWORD];				//用户密码
	TCHAR							m_szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//详细资料
protected:
	TCHAR							m_szQQ[LEN_QQ];							//Q Q 号码
	TCHAR							m_szEMail[LEN_EMAIL];					//电子邮件
	TCHAR							m_szUserNote[LEN_USER_NOTE];			//用户说明
	TCHAR							m_szSeatPhone[LEN_SEAT_PHONE];			//固定电话
	TCHAR							m_szMobilePhone[LEN_MOBILE_PHONE];		//移动电话
	TCHAR							m_szCompellation[LEN_COMPELLATION];		//真实名字
	TCHAR							m_szPassPortID[LEN_PASS_PORT_ID];		//身份证号
	TCHAR							m_szDwellingPlace[LEN_DWELLING_PLACE];	//联系地址

	//控件变量
protected:
	CSkinEditEx						m_edGameID;								//用户I  D
	CSkinEditEx						m_edAccounts;							//用户帐号
	CSkinEditEx						m_edNickName;							//用户昵称
	CSkinEditEx						m_edPassword;							//修改密码
	CSkinEditEx						m_edUnderWrite;							//个性签名
	CSkinEditEx						m_edUnderWrite2;
	CSkinEditEx						m_edMember;
	CSkinEditEx						m_edMedal;
	CSkinEditEx						m_edGender;

	//控件变量
protected:
	CSkinEditEx						m_edQQ;									//Q Q 号码
	CSkinEditEx						m_edEMail;								//电子邮件
	CSkinEditEx						m_edUserNote;							//用户说明
	CSkinEditEx						m_edSeatPhone;							//固定电话
	CSkinEditEx						m_edMobilePhone;						//移动电话
	CSkinEditEx						m_edCompellation;						//真实名字
	CSkinEditEx						m_edPassPortID;							//身份证号
	CSkinEditEx						m_edDwellingPlace;						//联系地址

	//控件变量
protected:
	CSkinButton						m_btOk;									//修改按钮
	CSkinButton						m_btSelectFace;							//选择按钮
	CSkinButton						m_btCopy;
	CSkinButton						m_btEdit;

	bool							m_bEnableEdit;
    
	//组件变量
protected:
	CFaceItemView					m_FaceItemView;							//头像视图
	CFaceSelectControlHelper		m_FaceSelectControl;					//头像选择

	//函数定义
public:
	//构造函数
	CDlgServiceIndividual();
	//析构函数
	virtual ~CDlgServiceIndividual();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();

	//重载函数
public:
	//获取标题
	virtual LPCTSTR GetServiceItemName() { return TEXT("用户资料"); }

	//网络事件
private:
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);

	//事件处理
protected:
	//开始任务
	VOID OnMissionStart();
	//终止任务
	VOID OnMissionConclude();

	//加载函数
protected:
	//帐号信息
	VOID LoadAccountsInfo();
	//查询资料
	VOID QueryIndividualInfo();
	//更新资料
	VOID UpdateIndividualInfo();

	//消息映射
protected:
	//头像选择
	VOID OnBnClickedSelectFace();
	//
	VOID OnBnClickedCopy();
	//
	VOID OnBnClickedEdit();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

//////////////////////////////////////////////////////////////////////////////////

#endif
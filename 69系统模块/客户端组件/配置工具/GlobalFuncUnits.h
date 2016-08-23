#ifndef GLOBAL_FUNC_UNITS_HEAD_FILE
#define GLOBAL_FUNC_UNITS_HEAD_FILE
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////

//处理消息
extern VOID DoEvents();
//获取图标索引
extern int GetDiskIconIndex();
//获取图标索引
extern int GetDirectoryIconIndex();
//获取图标索引
extern int GetIconIndex(LPCTSTR lpszPath, BOOL bIsDir, BOOL bSelected = FALSE);
//等待操作结果
extern BOOL WaitOperateResult(HANDLE hEventHandle,BOOL * pOperateResult,int nTimeout);

////////////////////////////////////////////////////////////////////////////////////////////////

#endif
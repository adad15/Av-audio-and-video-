#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号

// 
// 程序的入口，会调用CVideoClientApp中的InitInstance来启动程序。
// 
// CVideoClientApp:
//	1.提供InitInstance初始化方法，创建VideoClientController对象，然后调用该对象的Init方法和Invoke方法。
// 
//	                         Init方法：使用 Init 将VideoClientController对象里面的dlg的地址传递出来，传递给m_pMainWnd
//	2.VideoClientController
//							 Invoke方法：在Invoke函数里面调用dlg的DoModal方法
//

class CVideoClientApp : public CWinApp
{
public:
	CVideoClientApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现
	DECLARE_MESSAGE_MAP()
};

extern CVideoClientApp theApp;
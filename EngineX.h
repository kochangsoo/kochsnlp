
// EngineX.h : EngineX ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CEngineXApp:
// �� Ŭ������ ������ ���ؼ��� EngineX.cpp�� �����Ͻʽÿ�.
//

class CEngineXApp : public CWinApp
{
public:
	CEngineXApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CEngineXApp theApp;

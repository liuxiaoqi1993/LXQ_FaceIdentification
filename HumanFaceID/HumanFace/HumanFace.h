
// HumanFace.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CHumanFaceApp:
// �йش����ʵ�֣������ HumanFace.cpp
//

class CHumanFaceApp : public CWinApp
{
public:
	CHumanFaceApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CHumanFaceApp theApp;
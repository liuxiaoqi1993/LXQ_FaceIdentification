
// HumanFaceDlg.h : 头文件
//

#pragma once


// CHumanFaceDlg 对话框
class CHumanFaceDlg : public CDialogEx
{
// 构造
public:
	CHumanFaceDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HUMANFACE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CImage image;
	BOOL flag ;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeAddPicture();
	afx_msg void OnBnClickedInsertInfo();
	afx_msg void OnBnClickedOk();
};

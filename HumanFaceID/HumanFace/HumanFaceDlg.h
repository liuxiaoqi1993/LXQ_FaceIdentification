
// HumanFaceDlg.h : ͷ�ļ�
//

#pragma once


// CHumanFaceDlg �Ի���
class CHumanFaceDlg : public CDialogEx
{
// ����
public:
	CHumanFaceDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_HUMANFACE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CImage image;
	BOOL flag ;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeAddPicture();
	afx_msg void OnBnClickedInsertInfo();
	afx_msg void OnBnClickedOk();
};

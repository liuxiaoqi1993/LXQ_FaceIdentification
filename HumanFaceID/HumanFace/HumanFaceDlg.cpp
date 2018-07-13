
// HumanFaceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HumanFace.h"
#include "HumanFaceDlg.h"
#include "afxdialogex.h"
//#include "resource.h"

//#include <atlimage.h>
//#include "afxwin.h"



// CHumanFaceDlg �Ի���




CHumanFaceDlg::CHumanFaceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHumanFaceDlg::IDD, pParent)
{
	flag = false;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHumanFaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHumanFaceDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_ADD_PICTURE, &CHumanFaceDlg::OnEnChangeAddPicture)
	ON_BN_CLICKED(IDC_INSERT_INFO, &CHumanFaceDlg::OnBnClickedInsertInfo)
	ON_BN_CLICKED(IDOK, &CHumanFaceDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CHumanFaceDlg ��Ϣ�������

BOOL CHumanFaceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CHumanFaceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
		
		
	}
	if(flag){
		int cx, cy;  
		CRect   rect;  
	//����·������ͼƬ  
		//char strPicPath[]="C:/Users/Administrator/Desktop/test.png";
		//image.Load(strText);    
	//��ȡͼƬ�Ŀ� ��
		cx  = image.GetWidth();  
		cy  = image.GetHeight();  
 
		CWnd *pWnd = NULL;  
		pWnd= GetDlgItem(IDC_SHOW_PICTURE);//��ȡ�ؼ����
	//��ȡPicture Control�ؼ��Ŀͻ���
		pWnd->GetClientRect(&rect);
  
		CDC *pDc = NULL;  
		pDc = pWnd->GetDC();//��ȡpicture control��DC  
	//����ָ���豸�����е�λͼ����ģʽ
		int ModeOld=SetStretchBltMode(pDc->m_hDC,STRETCH_HALFTONE); 
	//��Դ�����и���һ��λͼ��Ŀ����Σ���Ŀ���豸���õ�ģʽ����ͼ��������ѹ��
		image.StretchBlt(pDc->m_hDC,rect,SRCCOPY);
		SetStretchBltMode(pDc->m_hDC,ModeOld); 
		ReleaseDC(pDc);  
	}
		
	
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CHumanFaceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CHumanFaceDlg::OnEnChangeAddPicture()
{
	
		CString strText;
		GetDlgItemText(IDC_ADD_PICTURE,strText);
		//CString cstr;
		//string str((LPCTSTR)cstr);
		//Mat read = imread(str);//ʵ����һ��Mat����,����ʹ�õ��Ǿֲ�·����ͼƬ�͹����ļ�����ͬһĿ¼����������ļ���Ŀ¼��
    //assert(read.data);//�������Ϊ�վ���ִֹ��
		//namedWindow("pic",CV_WINDOW_AUTOSIZE);//����һ�����ڣ���С����ӦͼƬ
		//imshow("pic", read);
		//waitKey(0);
	
		int cx, cy;  
		CRect   rect;  
	//����·������ͼƬ  
		//char strPicPath[]="C:/Users/Administrator/Desktop/test.png";
		image.Load(strText);
		if(image) flag = true; 
	//��ȡͼƬ�Ŀ� ��
		cx  = image.GetWidth();  
		cy  = image.GetHeight();  
 
		CWnd *pWnd = NULL;  
		pWnd= GetDlgItem(IDC_SHOW_PICTURE);//��ȡ�ؼ����
	//��ȡPicture Control�ؼ��Ŀͻ���
		pWnd->GetClientRect(&rect);
  
		CDC *pDc = NULL;  
		pDc = pWnd->GetDC();//��ȡpicture control��DC  
	//����ָ���豸�����е�λͼ����ģʽ
		int ModeOld=SetStretchBltMode(pDc->m_hDC,STRETCH_HALFTONE); 
	//��Դ�����и���һ��λͼ��Ŀ����Σ���Ŀ���豸���õ�ģʽ����ͼ��������ѹ��
		image.StretchBlt(pDc->m_hDC,rect,SRCCOPY);
		SetStretchBltMode(pDc->m_hDC,ModeOld); 
		ReleaseDC(pDc);  
	
		
	
	
	
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
int getNextLinePos(FILE *p)
{
	int ch;	
	int curpos = 0;
	ch=getc(p);
	curpos = ftell(p);
	while(ch!=EOF)
	{
		putchar(ch);
		ch=getc(p);
		if(ch == '\n')
		{
			break;
		}
	}
	return (ftell(p) - curpos + 1);
}

void CHumanFaceDlg::OnBnClickedInsertInfo()
{
	CString username;
	CString jobnumber;
	CString imagepath;
	GetDlgItemText(IDC_UN,username);
	GetDlgItemText(IDC_PW,jobnumber);
	GetDlgItemText(IDC_ADD_PICTURE,imagepath);
    //AfxMessageBox(strText);  //��֤�Ƿ���ȷ
	// TODO: �ڴ���ӿؼ�֪ͨ����������
		
	FILE* fp;
	if(fp=fopen("C:\\Users\\Administrator\\Desktop\\userInfo.txt","at+")){
		
			fseek(fp, 0, SEEK_END);
			fprintf(fp,"%s\t%s\t%s\n",username,jobnumber,imagepath);
			//fprintf(fp,"")
		
		//���༭������д�뵽fp�ļ�ָ����
		
	}
	fclose(fp);

}


void CHumanFaceDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}

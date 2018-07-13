
// HumanFaceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HumanFace.h"
#include "HumanFaceDlg.h"
#include "afxdialogex.h"
//#include "resource.h"

//#include <atlimage.h>
//#include "afxwin.h"



// CHumanFaceDlg 对话框




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


// CHumanFaceDlg 消息处理程序

BOOL CHumanFaceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHumanFaceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
		
		
	}
	if(flag){
		int cx, cy;  
		CRect   rect;  
	//根据路径载入图片  
		//char strPicPath[]="C:/Users/Administrator/Desktop/test.png";
		//image.Load(strText);    
	//获取图片的宽 高
		cx  = image.GetWidth();  
		cy  = image.GetHeight();  
 
		CWnd *pWnd = NULL;  
		pWnd= GetDlgItem(IDC_SHOW_PICTURE);//获取控件句柄
	//获取Picture Control控件的客户区
		pWnd->GetClientRect(&rect);
  
		CDC *pDc = NULL;  
		pDc = pWnd->GetDC();//获取picture control的DC  
	//设置指定设备环境中的位图拉伸模式
		int ModeOld=SetStretchBltMode(pDc->m_hDC,STRETCH_HALFTONE); 
	//从源矩形中复制一个位图到目标矩形，按目标设备设置的模式进行图像的拉伸或压缩
		image.StretchBlt(pDc->m_hDC,rect,SRCCOPY);
		SetStretchBltMode(pDc->m_hDC,ModeOld); 
		ReleaseDC(pDc);  
	}
		
	
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
		//Mat read = imread(str);//实例化一个Mat对象,这里使用的是局部路径，图片和工程文件放在同一目录（不是输出文件的目录）
    //assert(read.data);//如果数据为空就终止执行
		//namedWindow("pic",CV_WINDOW_AUTOSIZE);//建立一个窗口，大小自适应图片
		//imshow("pic", read);
		//waitKey(0);
	
		int cx, cy;  
		CRect   rect;  
	//根据路径载入图片  
		//char strPicPath[]="C:/Users/Administrator/Desktop/test.png";
		image.Load(strText);
		if(image) flag = true; 
	//获取图片的宽 高
		cx  = image.GetWidth();  
		cy  = image.GetHeight();  
 
		CWnd *pWnd = NULL;  
		pWnd= GetDlgItem(IDC_SHOW_PICTURE);//获取控件句柄
	//获取Picture Control控件的客户区
		pWnd->GetClientRect(&rect);
  
		CDC *pDc = NULL;  
		pDc = pWnd->GetDC();//获取picture control的DC  
	//设置指定设备环境中的位图拉伸模式
		int ModeOld=SetStretchBltMode(pDc->m_hDC,STRETCH_HALFTONE); 
	//从源矩形中复制一个位图到目标矩形，按目标设备设置的模式进行图像的拉伸或压缩
		image.StretchBlt(pDc->m_hDC,rect,SRCCOPY);
		SetStretchBltMode(pDc->m_hDC,ModeOld); 
		ReleaseDC(pDc);  
	
		
	
	
	
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
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
    //AfxMessageBox(strText);  //验证是否正确
	// TODO: 在此添加控件通知处理程序代码
		
	FILE* fp;
	if(fp=fopen("C:\\Users\\Administrator\\Desktop\\userInfo.txt","at+")){
		
			fseek(fp, 0, SEEK_END);
			fprintf(fp,"%s\t%s\t%s\n",username,jobnumber,imagepath);
			//fprintf(fp,"")
		
		//将编辑框数据写入到fp文件指针内
		
	}
	fclose(fp);

}


void CHumanFaceDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}

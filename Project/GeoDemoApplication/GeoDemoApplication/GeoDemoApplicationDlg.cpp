
// GeoDemoApplicationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GeoDemoApplication.h"
#include "GeoDemoApplicationDlg.h"
#include "afxdialogex.h"

#include <model\LidarChangeDetector.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGeoDemoApplicationDlg dialog



CGeoDemoApplicationDlg::CGeoDemoApplicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_GEODEMOAPPLICATION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGeoDemoApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGEVIEW_1, imageView1);
	DDX_Control(pDX, IDC_IMAGEVIEW_2, imageView2);
	DDX_Control(pDX, IDC_IMAGEVIEW_3, imageView3);
}

BEGIN_MESSAGE_MAP(CGeoDemoApplicationDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CGeoDemoApplicationDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CGeoDemoApplicationDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CGeoDemoApplicationDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CGeoDemoApplicationDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CGeoDemoApplicationDlg message handlers

BOOL CGeoDemoApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	//LidarChangeDetector::DoEverything
	OnBnClickedButton1();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGeoDemoApplicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGeoDemoApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



static void MatToCImage(const Mat& mat, CImage& image) {
	const int width = mat.cols;
	const int height = mat.rows;
	image.Create(width, height, 24);
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			//int debug_depth = mat.depth();
			if(mat.depth() == CV_8U) {
				if(mat.channels() == 1) {
					uint8_t gray = mat.at<uint8_t>(y, x); // open cv ben forditva van az xy sorrend
					image.SetPixelRGB(x, y, gray, gray, gray);
				} else if(mat.channels() == 3) {
					Vec3b rgb = mat.at<Vec3b>(y, x); // open cv ben forditva van az xy sorrend
					image.SetPixelRGB(x, y, rgb[0], rgb[1], rgb[2]);
				} else if(mat.channels() == 4) {
					Vec4b rgb = mat.at<Vec4b>(y, x); // open cv ben forditva van az xy sorrend
					image.SetPixelRGB(x, y, rgb[0], rgb[1], rgb[2]);
				} else {
					assert(0 && "unimplemented");
				}

			} else if(mat.depth() == CV_32F) {
				if(mat.channels() == 3) {
					Vec3f rgb = mat.at<Vec3f>(y, x); // open cv ben forditva van az xy sorrend
					image.SetPixelRGB(x, y, byte(rgb[0]*255), byte(rgb[1]*255), byte(rgb[2]*255));
				} else {
					assert(0 && "unimplemented");
				}
			} else {
				assert(0 && "unimplemented");
			}
			
			
		}
	}
}

void DisplayImage(CStatic& wnd, Mat image) {
	if(image.cols == 0 || image.rows == 0) return;
	CImage im1;
	MatToCImage(image, im1);


	wnd.ModifyStyle(0, SS_BITMAP | SS_CENTERIMAGE);
	wnd.SetBitmap(im1.Detach());
	wnd.Invalidate();
}

void CGeoDemoApplicationDlg::OnBnClickedButton1() {
	
	RunMainAlgorithm("b2.png");
	//CImage im1;
	//MatToCImage(result.im1, im1);


	//imageView1.ModifyStyle(0, SS_BITMAP| SS_CENTERIMAGE);
	//imageView1.SetBitmap(im1.Detach());
	//imageView1.Invalidate();
	//CWnd* imview1 = GetDlgItem(IDC_IMAGEVIEW_1);
	//CStatic* st = dynamic_cast<CStatic*>(imview1);
	//if(st)
	//	st->SetBitmap(im1);
	

}


void CGeoDemoApplicationDlg::OnBnClickedButton2() {
	RunMainAlgorithm("a1.png");
}


void CGeoDemoApplicationDlg::OnBnClickedButton3() {
	RunMainAlgorithm("b1.png");
}


void CGeoDemoApplicationDlg::OnBnClickedButton4() {
	RunMainAlgorithm("c1.png");
}

void CGeoDemoApplicationDlg::RunMainAlgorithm(CStringA fileName) {
	Result result = LidarChangeDetector::DoEverything(fileName);

	DisplayImage(imageView1, result.im1);
	DisplayImage(imageView2, result.im2);
	DisplayImage(imageView3, result.im3);
}

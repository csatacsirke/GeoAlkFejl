
// GeoDemoApplicationDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CGeoDemoApplicationDlg dialog
class CGeoDemoApplicationDlg : public CDialogEx
{
// Construction
public:
	CGeoDemoApplicationDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GEODEMOAPPLICATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CStatic imageView1;
	CStatic imageView2;
	CStatic imageView3;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
private:
	void RunMainAlgorithm(CStringA fileName1, CStringA fileName2);
public:
	CEdit fileEdit1;
	CEdit fileEdit2;
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
};

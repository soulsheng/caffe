#pragma once


// CDialogSetting 对话框
#include <vector>
#include <string>

class CDialogSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogSetting)

public:
	CDialogSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogSetting();

// 对话框数据
	enum { IDD = IDD_DIALOG_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpenFileModel();
	CString m_FileNameModel;

public:
	std::vector<std::string>	getStringVecSetting();
	CString m_FileNameTrain;
	CString m_FileNameMean;
	CString m_FileNameLabel;
	afx_msg void OnBnClickedButtonOpenFileTrain();
	afx_msg void OnBnClickedButtonOpenFileMean();
	afx_msg void OnBnClickedButtonOpenFileLabel();
};

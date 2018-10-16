#pragma once


// CDialogSetting �Ի���
#include <vector>
#include <string>

class CDialogSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogSetting)

public:
	CDialogSetting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogSetting();

// �Ի�������
	enum { IDD = IDD_DIALOG_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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

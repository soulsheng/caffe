// DialogSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "AEye.h"
#include "DialogSetting.h"
#include "afxdialogex.h"


// CDialogSetting 对话框

IMPLEMENT_DYNAMIC(CDialogSetting, CDialogEx)

CDialogSetting::CDialogSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogSetting::IDD, pParent)
	, m_FileNameModel(_T("..\\..\\models\\bvlc_reference_caffenet\\deploy.prototxt"))
	, m_FileNameTrain(_T("..\\..\\models\\bvlc_reference_caffenet\\bvlc_reference_caffenet.caffemodel"))
	, m_FileNameMean(_T("..\\..\\data\\ilsvrc12\\imagenet_mean.binaryproto"))
	, m_FileNameLabel(_T("..\\..\\data\\ilsvrc12\\synset_words_cn.txt"))
{

}

CDialogSetting::~CDialogSetting()
{
}

void CDialogSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FILE_MODEL, m_FileNameModel);
	DDX_Text(pDX, IDC_EDIT_FILE_TRAIN, m_FileNameTrain);
	DDX_Text(pDX, IDC_EDIT_FILE_MEAN, m_FileNameMean);
	DDX_Text(pDX, IDC_EDIT_FILE_LABEL, m_FileNameLabel);
}


BEGIN_MESSAGE_MAP(CDialogSetting, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FILE_MODEL, &CDialogSetting::OnBnClickedButtonOpenFileModel)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FILE_TRAIN, &CDialogSetting::OnBnClickedButtonOpenFileTrain)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FILE_MEAN, &CDialogSetting::OnBnClickedButtonOpenFileMean)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FILE_LABEL, &CDialogSetting::OnBnClickedButtonOpenFileLabel)
END_MESSAGE_MAP()


// CDialogSetting 消息处理程序


void CDialogSetting::OnBnClickedButtonOpenFileModel()
{
	// TODO:  在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,
		"*.prototxt", NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"prototxt(*.prototxt)|| All Files (*.*) |*.*||||", this);
	INT_PTR result = dlg.DoModal();
	if (result == IDOK)
	{
		m_FileNameModel = dlg.GetPathName();

		UpdateData(FALSE);
	}
}

std::vector<std::string> CDialogSetting::getStringVecSetting()
{
	std::vector<std::string> vecSetting;
	vecSetting.push_back(m_FileNameModel.GetBuffer());
	vecSetting.push_back(m_FileNameTrain.GetBuffer());
	vecSetting.push_back(m_FileNameMean.GetBuffer());
	vecSetting.push_back(m_FileNameLabel.GetBuffer());

	return vecSetting;
}


void CDialogSetting::OnBnClickedButtonOpenFileTrain()
{
	// TODO:  在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,
		"*.caffemodel", NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"caffemodel(*.caffemodel)|| All Files (*.*) |*.*||||", this);
	INT_PTR result = dlg.DoModal();
	if (result == IDOK)
	{
		m_FileNameTrain = dlg.GetPathName();

		UpdateData(FALSE);
	}
}


void CDialogSetting::OnBnClickedButtonOpenFileMean()
{
	// TODO:  在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,
		"*.binaryproto", NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"binaryproto(*.binaryproto)|| All Files (*.*) |*.*||||", this);
	INT_PTR result = dlg.DoModal();
	if (result == IDOK)
	{
		m_FileNameMean = dlg.GetPathName();

		UpdateData(FALSE);
	}
}


void CDialogSetting::OnBnClickedButtonOpenFileLabel()
{
	// TODO:  在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,
		"*.txt", NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"txt(*.txt)|| All Files (*.*) |*.*||||", this);
	INT_PTR result = dlg.DoModal();
	if (result == IDOK)
	{
		m_FileNameLabel = dlg.GetPathName();

		UpdateData(FALSE);
	}
}

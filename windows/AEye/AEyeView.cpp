
// AEyeView.cpp : CAEyeView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "AEye.h"
#endif

#include "AEyeDoc.h"
#include "AEyeView.h"
#include "MainFrm.h"

//#include "Utility.h"
#include "CFileUtilityWIN.h"
#include "CFileUtilitySTL.h"

#include "DialogSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include <sstream>

// CAEyeView

IMPLEMENT_DYNCREATE(CAEyeView, CView)

BEGIN_MESSAGE_MAP(CAEyeView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CAEyeView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_FILE_OPEN, &CAEyeView::OnFileOpen)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_COMMAND(ID_OPEN_FILE_LIST, &CAEyeView::OnOpenFileList)
	ON_COMMAND(ID_SETTING, &CAEyeView::OnSetting)
END_MESSAGE_MAP()

// CAEyeView ����/����

CAEyeView::CAEyeView()
{
	// TODO:  �ڴ˴���ӹ������
	setDefault();
}

CAEyeView::~CAEyeView()
{
}

BOOL CAEyeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CAEyeView ����

void CAEyeView::OnDraw(CDC* /*pDC*/)
{
	CAEyeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
	if (image.IsNull())
		return;

	CRect rect;
	CDC *pDC = this->GetDC();
	GetClientRect(rect);
	HDC hDC = pDC->GetSafeHdc();

	::SetStretchBltMode(hDC, HALFTONE);
	::SetBrushOrgEx(hDC, 0, 0, NULL);

	image.Draw(hDC, rect);

	pDC->SetBkMode(TRANSPARENT); //���ñ���Ϊ͸����
	pDC->SetTextColor(RGB(255, 69, 0)); // �����ɫ
	pDC->TextOut(rect.Width()*7.0f / 8, rect.Height()*1.0f / 12, m_currentClassNamePredict.c_str());

	ReleaseDC(pDC);//�ͷ�picture�ؼ���DC
}


// CAEyeView ��ӡ


void CAEyeView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CAEyeView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CAEyeView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CAEyeView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӵ�ӡ����е��������
}

void CAEyeView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CAEyeView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CAEyeView ���

#ifdef _DEBUG
void CAEyeView::AssertValid() const
{
	CView::AssertValid();
}

void CAEyeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAEyeDoc* CAEyeView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAEyeDoc)));
	return (CAEyeDoc*)m_pDocument;
}
#endif //_DEBUG


// CAEyeView ��Ϣ�������


void CAEyeView::OnFileOpen()
{
	// TODO:  �ڴ���������������
	if (!classifier.isInitialized())
	{
		AfxMessageBox("δ����ʶ�������");
		return;
	}

	CFileDialog dlg(TRUE,
		"*.jpg;*.jpeg", NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"ͼƬjpg��jpeg(*.jpg;*.jpeg)|*.jpg;*.jpeg| ���и�ʽ (*.*) |*.*||", this);
	INT_PTR result = dlg.DoModal();
	if (result == IDOK)
	{
		outputInfo("ʶ��һ��������");

		CString fileName = dlg.GetPathName();

		string file = fileName.GetBuffer();
		string shortname = CFileUtilitySTL::getShortFileName(file);

		int msTime = 0;
		std::vector<Prediction> result;
		
		if (predict(file, result, msTime))
		{
			cachePredictionResult(shortname, result, file);

			updateUI(shortname, file, result, msTime);

			outputInfo("");
		}
		else
		{ 
			outputInfo(file.c_str());
			outputInfo("Ԥ��ʧ�ܣ���ѡ��һ��ͼƬ��");
		}
	}

}

void CAEyeView::cachePredictionResult(string shortname, std::vector<Prediction> result, string file)
{
	m_PredictionResultList.insert(PredictionResultPair(shortname, result));

	m_FilesMap.insert(FilesPair(shortname, file));
}

bool CAEyeView::predict(string &file, std::vector<Prediction> &predictions, int &msTime)
{

	cv::Mat img = cv::imread(file, -1);
	if (img.empty()) // "Unable to decode image " << file;
		return false;

	clock_t t = clock();
	predictions = classifier.Classify(img);
	
	msTime = clock() - t;

	return true;
}

void CAEyeView::AddFileViewBranch(std::string fileNameShort, float score)
{
	// MainFrame
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->AddFileViewBranch(fileNameShort, score);
}

void CAEyeView::AddFileViewBranch(ClassTop1Map& names)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->AddFileViewBranch(names);
}

void CAEyeView::AddFileViewBranch(FilesMap& names)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->AddFileViewBranch(names);
}

void CAEyeView::sortPredictionResult()
{
	outputInfo("��������");

	m_ClassTop1Map.clear();

	clock_t msTimeAll = clock();

	for (PredictionResultMap::iterator itrPRM = m_PredictionResultList.begin();
		itrPRM != m_PredictionResultList.end(); itrPRM++)
	{

		Prediction& top1 = (itrPRM->second)[0];

		ClassTop1Map::iterator itr = m_ClassTop1Map.find(top1.first);

		if (m_ClassTop1Map.end() == itr)
		{// not find, add new class
			Top1Map		top1Map;
			top1Map.insert(Top1Pair(top1.second, itrPRM->first));

			m_ClassTop1Map.insert(ClassTop1Pair(top1.first, top1Map));
		}
		else
		{// add to existing
			Top1Map		&top1Map = itr->second;

			top1Map.insert(Top1Pair(top1.second, itrPRM->first));
		}
	}

	std::ostringstream os;

	os << "---------- �ķ� "
		<< clock() - msTimeAll << " ���� ���� "
		<< std::endl;

	outputInfo(os.str().c_str());

	msTimeAll = clock();
	os.str("");

	AddFileViewBranch(m_ClassTop1Map);

	os << "---------- �ķ� "
		<< clock() - msTimeAll << " ���� ���½��� "
		<< std::endl;

	outputInfo(os.str().c_str());
	outputInfo("");
}

void CAEyeView::switchBilViewByName(std::string name)
{
	//predict(name);
	std::string fullname = m_FilesMap[name];

	if (fullname.empty())
		return;

	std::vector<Prediction> result = m_PredictionResultList[name];

	int msTime = 0;
	updateUI(name, fullname, result, msTime, LOG_TYPE_UI_PROPERTY | LOG_TYPE_UI_VIEW);

}

void CAEyeView::setDefault()
{

}

void CAEyeView::release()
{

}

void CAEyeView::outputInfo(const char* message, int value /*= -1*/)
{
	CMainFrame* pMFram = (CMainFrame*)AfxGetMainWnd();

	std::ostringstream os;
	os << message;

	if (-1 != value)
		os << " = " << value;

	pMFram->FillBuildWindow(os.str());
}


void CAEyeView::updateUI(string &shortname, string &file, std::vector<Prediction> &predictions, int msTime, int type)
{
	std::ostringstream os;

	os << "---------- ʶ�� "
		<< shortname << " ----------" << std::endl;

	os << "---------- �ķ� "
		<< msTime << " ����, " << std::endl;

	CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();

	for (size_t i = 0; i < predictions.size(); ++i) {
		Prediction p = predictions[i];
		os << std::fixed << std::setprecision(4) << p.second << " - \""
			<< p.first << "\"" << std::endl;

		if (type & LOG_TYPE_UI_PROPERTY)
		{
			pFrame->updateProperty(i * 2 + 1, p.first);
			pFrame->updateProperty(i * 2 + 2, p.second);
		}
	}

	if (type & LOG_TYPE_UI_VIEW)
	{
		if (!image.IsNull())
			image.Destroy();

		image.Load(file.c_str());

		m_currentClassNamePredict = predictions[0].first;
	}

	if (type & LOG_TYPE_UI_OUTPUT)
		outputInfo(os.str().c_str());

	if (type & LOG_TYPE_UI_FILE)
		AddFileViewBranch(shortname, predictions[0].second);
}

void CAEyeView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	OnDraw(NULL);

	CView::OnTimer(nIDEvent);
}


int CAEyeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	SetTimer(0, 30, NULL);	//��ʱ��ʾ��һ��30���봥��һ�εĶ�ʱ����30֡/�� 

	return 0;
}


void CAEyeView::OnOpenFileList()
{
	// TODO:  �ڴ���������������

	if (!classifier.isInitialized())
	{
		AfxMessageBox("δ����ʶ�������");
		return;
	}

	std::string		imagePath;

	CFileUtilityWIN::getFilePathFromDialog(imagePath);

	if (imagePath.empty())
		return;

	std::vector<std::string>		imageList;
	std::vector<std::string>		fmts;
	fmts.push_back(_T("jpg"));
	fmts.push_back(_T("jepg"));
	fmts.push_back(_T("png"));
	fmts.push_back(_T("bmp"));

	//getFileListFromPath(imagePath, imageList);
	CFileUtilityWIN::getFileListFromPathNest(imagePath, _T(""), fmts, imageList);

	int msTime = 0;
	std::vector<Prediction> result;

	clock_t msTimeAll = clock();

	for (std::vector<std::string>::iterator i = imageList.begin(); i != imageList.end(); i++)
	{
		string file = imagePath + *i;
		string shortname = CFileUtilitySTL::getShortFileName(file);

		if(predict(file, result, msTime))
			cachePredictionResult(shortname, result, file);

		//updateUI(shortname, file, result, msTime);
	}

	std::ostringstream os;
	os << "---------- �ܹ��ķ� "
		<< (clock() - msTimeAll)/1000.0f << " �� ����ʶ�� "
		<< imageList.size() << " ��ͼƬ, " << std::endl
		<< "ƽ��ÿ��ͼƬ��ʱ " << (clock() - msTimeAll)*1.0f / imageList.size() << "���� " << std::endl;

	outputInfo(os.str().c_str());

	msTimeAll = clock();
	os.str("");

	//updateUI( *( imageList.end()-1), result, msTime);
	AddFileViewBranch(m_FilesMap);

	os << "---------- �ķ� "
		<< clock() - msTimeAll << " ���� ���½��� "
		<< std::endl;

	outputInfo(os.str().c_str());
	outputInfo("");

}

void CAEyeView::OnSetting()
{
	// TODO:  �ڴ���������������
	outputInfo("��������ʶ��ģ��...");

	CDialogSetting dlgSetting;
	if (IDOK != dlgSetting.DoModal())
	{
		outputInfo("������ֹ�����������ã�");
		return;
	}

	std::vector<std::string> vecSetting = dlgSetting.getStringVecSetting();

	if (vecSetting.size() != 4)
	{
		AfxMessageBox("��Ҫ4����������������ȫ��");
		return;
	}

	model_file = vecSetting[0];
	trained_file = vecSetting[1];
	mean_file = vecSetting[2];
	label_file = vecSetting[3];


	StringVec::iterator itr = vecSetting.begin();
	for (; itr != vecSetting.end(); itr++)
	{
		if (!CFileUtilitySTL::checkFileExist(*itr))
			break;
	}

	// �����ļ���������
	if (vecSetting.end() != itr)
	{
		AfxMessageBox("�����ļ���������������Щ�ļ��޷��򿪣�");
		outputInfo("������ֹ�����������ã�");
		return;
	}

	classifier.load(model_file, trained_file, mean_file, label_file);

	outputInfo("ʶ��ģ��������ɣ�");
	outputInfo("");

	std::string strDemo("..\\..\\examples\\images\\cat.jpg");
	int msTime = 0;
	std::vector<Prediction> result;

	if( predict(strDemo, result, msTime) )
		outputInfo("ʶ�������ɣ�");
	outputInfo("");


}

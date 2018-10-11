
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
	CFileDialog dlg(TRUE,
		"*.jpg;*.jpeg", NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"jpg(*.jpg;*.jpeg)|*.jpg;*.jpeg|| All Files (*.*) |*.*||||", this);
	INT_PTR result = dlg.DoModal();
	if (result == IDOK)
	{
		CString fileName = dlg.GetPathName();

		string file = fileName.GetBuffer();

		int msTime = 0;
		std::vector<Prediction> result;
		predict(file, result, msTime);

		m_PredictionResultList.insert( PredictionResultPair(file, result) );

		updateUI(file, result, msTime);
	}

}

void CAEyeView::predict(string &file, std::vector<Prediction> &predictions, int &msTime)
{

	cv::Mat img = cv::imread(file, -1);
	CHECK(!img.empty()) << "Unable to decode image " << file;

	clock_t t = clock();
	predictions = classifier.Classify(img);
	
	msTime = clock() - t;
}

void CAEyeView::AddFileViewBranch(std::string fileNameShort)
{
	// MainFrame
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->AddFileViewBranch(fileNameShort);
}

void CAEyeView::switchBilViewByName(std::string name)
{
	//predict(name);
	std::vector<Prediction> result = m_PredictionResultList[name];

	int msTime = 0;
	updateUI(name, result, msTime, LOG_TYPE_UI_PROPERTY | LOG_TYPE_UI_VIEW);

}

void CAEyeView::setDefault()
{
	classifier.load(model_file, trained_file, mean_file, label_file);

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


void CAEyeView::updateUI(string &file, std::vector<Prediction> &predictions, int msTime, int type)
{
	std::ostringstream os;

	os << "---------- Prediction for "
		<< file << " ----------" << std::endl;

	os << "---------- cost "
		<< msTime << " ms, " << std::endl;

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
	}

	if (type & LOG_TYPE_UI_OUTPUT)
		outputInfo(os.str().c_str());

	if (type & LOG_TYPE_UI_FILE)
		AddFileViewBranch(file);
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
	std::string		imagePath;

	getFilePathFromDialog(imagePath);

	if (imagePath.empty())
		return;

	std::vector<std::string>		imageList;

	getFileListFromPath(imagePath, imageList);

	int msTime = 0;
	std::vector<Prediction> result;

	for (std::vector<std::string>::iterator i = imageList.begin(); i != imageList.end(); i++)
	{
		predict(imagePath + *i, result, msTime);

		m_PredictionResultList.insert(PredictionResultPair(imagePath + *i, result));

		updateUI(imagePath + *i, result, msTime);
	}

	//updateUI( *( imageList.end()-1), result, msTime);
}

void CAEyeView::getFilePathFromDialog(std::string &path)
{
	BROWSEINFO bi;
	char Buffer[MAX_PATH];

	//��ʼ����ڲ���bi��ʼ
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;//��ʼ���ƶ���rootĿ¼�ܲ�����
	bi.pszDisplayName = Buffer;//�˲�����ΪNULL������ʾ�Ի���
	bi.lpszTitle = "ѡ��ͼƬ·��";
	bi.ulFlags = BIF_EDITBOX;//���༭��ķ��
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = IDR_MAINFRAME;
	//��ʼ����ڲ���bi����

	std::string strMessage = path;
	strMessage = path + "����ѡ��ͼƬ·��...";
	outputInfo(strMessage.c_str());
	LOG(INFO) << strMessage;

	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//������ʾѡ��Ի���

	if (pIDList)
	{
		SHGetPathFromIDList(pIDList, Buffer);

		//ȡ���ļ���·����Buffer��
		path = std::string(Buffer) + "\\";

		outputInfo(path.c_str());
		outputInfo("ͼƬ·����ѡ��");
		LOG(INFO) << path << "ͼƬ·����ѡ��";
	}


	// free memory used   
	IMalloc * imalloc = 0;
	if (SUCCEEDED(SHGetMalloc(&imalloc)))
	{
		imalloc->Free(pIDList);
		imalloc->Release();
	}
}

void CAEyeView::getFileListFromPath(std::string &path, std::vector<std::string> &list)
{
	CString csDirPath = CString(path.c_str()) + "*.jpeg";
	HANDLE file;
	WIN32_FIND_DATA fileData;
	char line[1024];
	char fn[1000];
	//mbstowcs(fn,csDirPath.GetBuffer(),999);
	file = FindFirstFile(csDirPath.GetBuffer(), &fileData);
	list.push_back(fileData.cFileName);
	bool bState = false;
	bState = FindNextFile(file, &fileData);
	while (bState)
	{
		//wcstombs(line,(const char*)fileData.cFileName,259);
		list.push_back(fileData.cFileName);
		bState = FindNextFile(file, &fileData);
	}

}

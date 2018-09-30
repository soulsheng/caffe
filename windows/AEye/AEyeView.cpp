
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
		"Jpeg(*.jpg)|*.jpg", NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Jpeg(*.jpg)|*.jpg||", this);
	INT_PTR result = dlg.DoModal();
	if (result == IDOK)
	{
		CString fileName = dlg.GetPathName();

		string file = fileName.GetBuffer();

		std::ostringstream os;

		os << "---------- Prediction for "
			<< file << " ----------" << std::endl;

		cv::Mat img = cv::imread(file, -1);
		CHECK(!img.empty()) << "Unable to decode image " << file;
		std::vector<Prediction> predictions = classifier.Classify(img);

		for (size_t i = 0; i < predictions.size(); ++i) {
			Prediction p = predictions[i];
			os << std::fixed << std::setprecision(4) << p.second << " - \""
				<< p.first << "\"" << std::endl;
		}

		outputInfo(os.str().c_str());
	}

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

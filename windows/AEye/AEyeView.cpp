
// AEyeView.cpp : CAEyeView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
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

// CAEyeView 构造/析构

CAEyeView::CAEyeView()
{
	// TODO:  在此处添加构造代码
	setDefault();
}

CAEyeView::~CAEyeView()
{
}

BOOL CAEyeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CAEyeView 绘制

void CAEyeView::OnDraw(CDC* /*pDC*/)
{
	CAEyeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
	if (image.IsNull())
		return;

	CRect rect;
	CDC *pDC = this->GetDC();
	GetClientRect(rect);
	HDC hDC = pDC->GetSafeHdc();

	::SetStretchBltMode(hDC, HALFTONE);
	::SetBrushOrgEx(hDC, 0, 0, NULL);

	image.Draw(hDC, 0, 0/*, rect*/);

	CPen pNewPen;
	pNewPen.CreatePen(PS_SOLID, 1, RGB(255, 69, 0)); // 矩形框颜色
	CPen* poldPen = pDC->SelectObject(&pNewPen);

	pDC->SetBkMode(TRANSPARENT); //设置背景为透明！
	pDC->SetTextColor(RGB(255, 69, 0)); // 字体橙色
	pDC->TextOut(boxDetection.left, boxDetection.top*0.9, m_currentClassNamePredict.c_str());

	pDC->MoveTo(boxDetection.left, boxDetection.top);
	pDC->LineTo(boxDetection.right, boxDetection.top);
	pDC->LineTo(boxDetection.right, boxDetection.bottom);
	pDC->LineTo(boxDetection.left, boxDetection.bottom);
	pDC->LineTo(boxDetection.left, boxDetection.top);

	pDC->SelectObject(poldPen);

	ReleaseDC(pDC);//释放picture控件的DC
}


// CAEyeView 打印


void CAEyeView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CAEyeView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CAEyeView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CAEyeView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
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


// CAEyeView 诊断

#ifdef _DEBUG
void CAEyeView::AssertValid() const
{
	CView::AssertValid();
}

void CAEyeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAEyeDoc* CAEyeView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAEyeDoc)));
	return (CAEyeDoc*)m_pDocument;
}
#endif //_DEBUG


// CAEyeView 消息处理程序


void CAEyeView::OnFileOpen()
{
	// TODO:  在此添加命令处理程序代码
	if (!classifier.isInitialized())
	{
		AfxMessageBox("未配置识别参数！");
		return;
	}

	CFileDialog dlg(TRUE,
		"*.jpg;*.jpeg", NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"图片jpg或jpeg(*.jpg;*.jpeg)|*.jpg;*.jpeg| 所有格式 (*.*) |*.*||", this);
	INT_PTR result = dlg.DoModal();
	if (result == IDOK)
	{
		outputInfo("识别一个样本：");

		CString fileName = dlg.GetPathName();

		string file = fileName.GetBuffer();
		string shortname = CFileUtilitySTL::getShortFileName(file);

		int msTime = 0;
		std::vector<Prediction> result;
		std::vector<Detection> resultDetection;

		if (classifier.getTestType() == ENUM_CLASSIFICATION) {

			if (predict(file, result, msTime))
			{
				cachePredictionResult(shortname, result, file);

				updateUI(shortname, file, result, msTime);

				outputInfo("");
			}
			else
			{
				outputInfo(file.c_str());
				outputInfo("预测失败，请重新选择一张图片！");
			}
		}
		else
		{
			if (detect(file, resultDetection, msTime))
			{
				cacheDetectionResult(shortname, resultDetection, file);

				updateUI(shortname, file, resultDetection, msTime);

				outputInfo("");
			}
			else
			{
				outputInfo(file.c_str());
				outputInfo("检测失败，请重新选择一张图片！");
			}
		}
	}

}

void CAEyeView::cachePredictionResult(string shortname, std::vector<Prediction> result, string file)
{
	m_PredictionResultList.insert(PredictionResultPair(shortname, result));

	m_FilesMap.insert(FilesPair(shortname, file));
}

void CAEyeView::cacheDetectionResult(string shortname, std::vector<Detection> result, string file)
{
	m_DetectionResultList.insert(DetectionResultPair(shortname, result));

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

bool CAEyeView::detect(string &file, std::vector<Detection> &detections, int &msTime)
{
	cv::Mat img = cv::imread(file, -1);
	if (img.empty()) // "Unable to decode image " << file;
		return false;

	clock_t t = clock();
	detections = classifier.Detect(img);

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
	outputInfo("分类排序：");

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

	os << "---------- 耗费 "
		<< clock() - msTimeAll << " 毫秒 排序 "
		<< std::endl;

	outputInfo(os.str().c_str());

	msTimeAll = clock();
	os.str("");

	AddFileViewBranch(m_ClassTop1Map);

	os << "---------- 耗费 "
		<< clock() - msTimeAll << " 毫秒 更新界面 "
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

	os << "---------- 识别 "
		<< shortname << " ----------" << std::endl;

	os << "---------- 耗费 "
		<< msTime << " 毫秒, " << std::endl;

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

Detection CAEyeView::findBestScore(std::vector<Detection>& detections)
{
	if (detections.size() == 0)
		return Detection();

	float score = 0.0f;
	int bestIndex = 0;
	for (size_t i = 0; i < detections.size(); ++i)
	{
		Detection p = detections[i];
		if (p.score > score)
		{
			score = p.score;
			bestIndex = i;
		}
	}

	return detections[bestIndex];
}

void CAEyeView::updateUI(string &shortname, string &file, std::vector<Detection> &detections, int msTime, int type /*= LOG_TYPE_UI_ALL*/)
{
	std::ostringstream os;

	os << "---------- 识别 "
		<< shortname << " ----------" << std::endl;

	os << "---------- 耗费 "
		<< msTime << " 毫秒, " << std::endl;

	CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();

	std::vector<tstring> labels = classifier.getLabels();

	os << "检测到 " << detections.size() << " 个目标" << endl;
	os << "score - label" << std::endl;

	for (size_t i = 0; i < detections.size(); ++i) {
		Detection p = detections[i];

		if( p.label >= labels.size() )
			os << std::fixed << std::setprecision(4) << p.score << " - \""
				<< p.label << " error" << std::endl;
		else
			os << std::fixed << std::setprecision(4) << p.score << " - \""
				<< labels[p.label] << "\"" << std::endl;
#if 0
		if (type & LOG_TYPE_UI_PROPERTY)
		{
			pFrame->updateProperty(i * 2 + 1, p.label);
			pFrame->updateProperty(i * 2 + 2, p.score);
		}
#endif
	}

	Detection bestDetection = findBestScore(detections);

	if (type & LOG_TYPE_UI_VIEW)
	{
		if (!image.IsNull())
			image.Destroy();

		image.Load(file.c_str());

		if (detections.size() != 0)
		{
			int nLabelId = int(bestDetection.label + 0.01) - 1;
			m_currentClassNamePredict = classifier.getLabels()[nLabelId];
			boxDetection = CRect(bestDetection.xmin * image.GetWidth(), bestDetection.ymin * image.GetHeight(), 
				bestDetection.xmax * image.GetWidth(), bestDetection.ymax * image.GetHeight() );
		}
	}

	if (type & LOG_TYPE_UI_OUTPUT)
		outputInfo(os.str().c_str());

	if (type & LOG_TYPE_UI_FILE)
		AddFileViewBranch(shortname, bestDetection.score);
}

void CAEyeView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	OnDraw(NULL);

	CView::OnTimer(nIDEvent);
}


int CAEyeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SetTimer(0, 30, NULL);	//定时显示，一个30毫秒触发一次的定时器，30帧/秒 

	return 0;
}


void CAEyeView::OnOpenFileList()
{
	// TODO:  在此添加命令处理程序代码

	if (!classifier.isInitialized())
	{
		AfxMessageBox("未配置识别参数！");
		return;
	}

	std::string		imagePath;

	CFileUtilityWIN::getFilePathFromDialog(imagePath);

	if (imagePath.empty())
		return;

	std::vector<std::string>		imageList;
	std::vector<std::string>		fmts;
	fmts.push_back(_T("jpg"));
	fmts.push_back(_T("jpeg"));
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
	os << "---------- 总共耗费 "
		<< (clock() - msTimeAll)/1000.0f << " 秒 批量识别 "
		<< imageList.size() << " 张图片, " << std::endl
		<< "平均每张图片耗时 " << (clock() - msTimeAll)*1.0f / imageList.size() << "毫秒 " << std::endl;

	outputInfo(os.str().c_str());

	msTimeAll = clock();
	os.str("");

	//updateUI( *( imageList.end()-1), result, msTime);
	AddFileViewBranch(m_FilesMap);

	os << "---------- 耗费 "
		<< clock() - msTimeAll << " 毫秒 更新界面 "
		<< std::endl;

	outputInfo(os.str().c_str());
	outputInfo("");

}

void CAEyeView::OnSetting()
{
	// TODO:  在此添加命令处理程序代码
	outputInfo("正在配置识别模型...");

	CDialogSetting dlgSetting;
	if (IDOK != dlgSetting.DoModal())
	{
		outputInfo("配置中止，请重新配置！");
		return;
	}

	std::vector<std::string> vecSetting = dlgSetting.getStringVecSetting();

	model_file = vecSetting[0];
	trained_file = vecSetting[1];
	mean_file = vecSetting[2];
	label_file = vecSetting[3];
	
	mean_value = "104,117,123";
	if (!mean_file.empty())
		mean_value = "";

	classifier.load(model_file, trained_file, mean_file, label_file, mean_value);

	classifier.setTestType(ENUM_DETECTION);

	outputInfo("识别模型配置完成！");
	outputInfo("");

	std::string strDemo("..\\..\\examples\\images\\cat.jpg");
	int msTime = 0;
	std::vector<Prediction> result;
	std::vector<Detection> resultDetection;

	if (classifier.getTestType() == ENUM_CLASSIFICATION) {
		if (predict(strDemo, result, msTime))
			outputInfo("目标识别测试完成！");
	}
	else {
		if (detect(strDemo, resultDetection, msTime))
			outputInfo("目标识别与检测测试完成！");
	}

	outputInfo("");


}

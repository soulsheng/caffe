
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

	image.Draw(hDC, rect);
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
	CFileDialog dlg(TRUE,
		"*.jpg;*.jpeg", NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"jpg(*.jpg;*.jpeg)|*.jpg;*.jpeg|| All Files (*.*) |*.*||||", this);
	INT_PTR result = dlg.DoModal();
	if (result == IDOK)
	{
		CString fileName = dlg.GetPathName();

		string file = fileName.GetBuffer();

		predict(file);

	}

}

void CAEyeView::predict(string &file)
{
	std::ostringstream os;

	os << "---------- Prediction for "
		<< file << " ----------" << std::endl;

	cv::Mat img = cv::imread(file, -1);
	CHECK(!img.empty()) << "Unable to decode image " << file;

	clock_t t = clock();
	std::vector<Prediction> predictions = classifier.Classify(img);
	os << "---------- cost "
		<< clock() - t << " ms, " << std::endl;

	for (size_t i = 0; i < predictions.size(); ++i) {
		Prediction p = predictions[i];
		os << std::fixed << std::setprecision(4) << p.second << " - \""
			<< p.first << "\"" << std::endl;
	}

	if (!image.IsNull())
		image.Destroy();

	image.Load(file.c_str());

	outputInfo(os.str().c_str());

	AddFileViewBranch(file);
}

void CAEyeView::AddFileViewBranch(std::string fileNameShort)
{
	// MainFrame
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->AddFileViewBranch(fileNameShort);
}

void CAEyeView::switchBilViewByName(std::string name)
{
	if (!image.IsNull())
		image.Destroy();

	image.Load(name.c_str());
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


void CAEyeView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	OnDraw(this->GetDC());

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
	std::string		imagePath;

	getFilePathFromDialog(imagePath);

	std::vector<std::string>		imageList;

	getFileListFromPath(imagePath, imageList);

	for (std::vector<std::string>::iterator i = imageList.begin(); i != imageList.end(); i++)
	{
		predict( imagePath + *i );
	}
}

void CAEyeView::getFilePathFromDialog(std::string &path)
{
	BROWSEINFO bi;
	char Buffer[MAX_PATH];

	//初始化入口参数bi开始
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;//初始化制定的root目录很不容易
	bi.pszDisplayName = Buffer;//此参数如为NULL则不能显示对话框
	bi.lpszTitle = "选择图片路径";
	bi.ulFlags = BIF_EDITBOX;//带编辑框的风格
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = IDR_MAINFRAME;
	//初始化入口参数bi结束

	std::string strMessage = path;
	strMessage = path + "正在选择图片路径...";
	outputInfo(strMessage.c_str());
	LOG(INFO) << strMessage;

	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//调用显示选择对话框

	if (pIDList)
	{
		SHGetPathFromIDList(pIDList, Buffer);

		//取得文件夹路径到Buffer里
		path = std::string(Buffer) + "\\";

		outputInfo(path.c_str());
		outputInfo("图片路径已选中");
		LOG(INFO) << path << "图片路径已选中";
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


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

	updateImage();

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
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
	pNewPen.CreatePen(PS_SOLID, 1, RGB(255, 69, 0)); // ���ο���ɫ
	CPen* poldPen = pDC->SelectObject(&pNewPen);

	pDC->SetBkMode(TRANSPARENT); //���ñ���Ϊ͸����
	pDC->SetTextColor(RGB(255, 69, 0)); // �����ɫ

	for (DetectionMap::iterator itr = m_DetectionMap.begin(); itr != m_DetectionMap.end(); ++itr) {

		Detection& p = itr->second;
		int nLabelId = int(p.label + 0.01) - 1;
		ostringstream os;
		os << classifier.getLabels()[nLabelId] << " = " << std::setprecision(3) << p.score;

		RECT rect;
		rect.left	= (int)(p.xmin * image.GetWidth() + 0.5);
		rect.top = (int)(p.ymin * image.GetHeight() + 0.5);
		rect.right = (int)(p.xmax * image.GetWidth() + 0.5);
		rect.bottom = (int)(p.ymax * image.GetHeight() + 0.5);

		pDC->TextOut(rect.left, rect.top - 15, os.str().c_str() );

		pDC->MoveTo(rect.left, rect.top);
		pDC->LineTo(rect.right, rect.top);
		pDC->LineTo(rect.right, rect.bottom);
		pDC->LineTo(rect.left, rect.bottom);
		pDC->LineTo(rect.left, rect.top);
	}

	pDC->SelectObject(poldPen);

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
		"ͼƬjpg��jpeg(*.jpg;*.jpeg)|*.jpg;*.jpeg| ��Ƶavi��mp4(*.avi;*.mp4)|*.avi;*.mp4| ���и�ʽ (*.*) |*.*||", this);
	INT_PTR result = dlg.DoModal();
	if (result == IDOK)
	{
		outputInfo("ʶ��һ��������");

		CString fileName = dlg.GetPathName();

		string file = fileName.GetBuffer();
		//classifyDetect2UI(file);

		m_fileCurrent = file;

		string fileExt = CFileUtilitySTL::getFileExt(file);
		if ( fileExt == "jpg" | fileExt == "jpeg")
			bVideoOrImage = false;
		else if(fileExt == "avi" | fileExt == "mp4")
			bVideoOrImage = true;

		if (bVideoOrImage)
		{
			// Read video
			video.open(file);

			// Exit if video is not opened
			if (!video.isOpened())
			{
				cout << "Could not read video file" << endl;
				return ;

			}

			// create sub dir
			string path = CFileUtilitySTL::getPathFileName(m_fileCurrent);
			string name = CFileUtilitySTL::getOnlyFileName(m_fileCurrent);

			m_pathSub = path + name + "\\";

			if (CFileUtilityWIN::getPathExist(m_pathSub))
				CFileUtilityWIN::removePath(m_pathSub);

			CFileUtilityWIN::createPath(m_pathSub);

			// ready to write result video
			bool bRet = writeVideoBegin(m_pathSub + "Snapshots.avi", 
				video.get(CV_CAP_PROP_FRAME_WIDTH), video.get(CV_CAP_PROP_FRAME_HEIGHT));

			if (!bRet)
				outputInfo("�޷�д����Ƶ");

			m_bUpdateImageNeed = true;
			m_msTimeStampBegin = clock();
		}
		else
			classifyDetect2UI(file);

	}

}

void CAEyeView::classifyDetect2UI(string &file)
{
	string shortname = CFileUtilitySTL::getShortFileName(file);
	cv::Mat img = cv::imread(file, -1);

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
			outputInfo("Ԥ��ʧ�ܣ�������ѡ��һ��ͼƬ��");
		}
	}
	else
	{
		if (detect(img, msTime))
		{
			cacheDetectionResult(shortname, resultDetection, file);

			updateUI(img, msTime, "");

			outputInfo("");
		}
		else
		{
			outputInfo(file.c_str());
			outputInfo("���ʧ�ܣ�������ѡ��һ��ͼƬ��");
		}
	}
}

void CAEyeView::classifyDetect2UI(cv::Mat &img)
{
	int msTime = 0;
	std::vector<Prediction> result;
	std::vector<Detection> resultDetection;

	if (classifier.getTestType() == ENUM_CLASSIFICATION) {

		if (predict(img, result, msTime))
		{
			//updateUI(shortname, file, result, msTime);

			outputInfo("");
		}
	}
	else
	{
		if (detect(img, msTime))
		{
			updateUI(img, msTime, "");

			outputInfo("");
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
	
	return predict(img, predictions, msTime);
}

bool CAEyeView::predict(cv::Mat &img, std::vector<Prediction> &predictions, int &msTime)
{
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
	
	return detect(img, detections, msTime);
}

bool CAEyeView::detect(cv::Mat &img, std::vector<Detection> &detections, int &msTime)
{
	if (img.empty()) // "Unable to decode image " << file;
		return false;

	clock_t t = clock();
	detections = classifier.Detect(img);

	msTime = clock() - t;

	return true;
}

bool CAEyeView::detect(cv::Mat &img, int &msTime)
{
	if (img.empty()) // "Unable to decode image " << file;
		return false;

	clock_t t = clock();
	m_DetectionMap = classifier.Detect(img, 10, 0.6);

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
	m_bUpdateImageNeed = false;
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

void CAEyeView::updateImage()
{
	if (!m_bUpdateImageNeed)
		return;

	cv::Mat frame;
	if (bVideoOrImage)
	{
		clock_t t = clock();

		std::ostringstream os;

		bool ok = video.read(frame);
		if (!ok)
		{
			m_bUpdateImageNeed = false;
			outputInfo("��Ƶ������ɣ�");

			writeVideoEnd();

			outputInfo("ץ��ͼƬ������ɣ�");

			return;
		}

		os << "---------- ��Ƶ����ͼƬ��ʱ "
			<< clock() - t << " ����, " << std::endl;
		
		classifyDetect2UI(frame);

		t = clock();

		cacheImage(frame);

		os << "---------- ץ�ĺ�ʱ "
			<< clock() - t << " ����, " << std::endl;

		outputInfo(os.str().c_str());
	}
}

void CAEyeView::MatToCImage(cv::Mat& mat, CImage& cimage)
{
	if (0 == mat.total())
	{
		return;
	}


	int nChannels = mat.channels();
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = mat.cols;
	int nHeight = mat.rows;


	//�ؽ�cimage
	cimage.Destroy();
	cimage.Create(nWidth, nHeight, 8 * nChannels);


	//��������


	uchar* pucRow;									//ָ������������ָ��
	uchar* pucImage = (uchar*)cimage.GetBits();		//ָ����������ָ��
	int nStep = cimage.GetPitch();					//ÿ�е��ֽ���,ע���������ֵ�����и�


	if (1 == nChannels)								//���ڵ�ͨ����ͼ����Ҫ��ʼ����ɫ��
	{
		RGBQUAD* rgbquadColorTable;
		int nMaxColors = 256;
		rgbquadColorTable = new RGBQUAD[nMaxColors];
		cimage.GetColorTable(0, nMaxColors, rgbquadColorTable);
		for (int nColor = 0; nColor < nMaxColors; nColor++)
		{
			rgbquadColorTable[nColor].rgbBlue = (uchar)nColor;
			rgbquadColorTable[nColor].rgbGreen = (uchar)nColor;
			rgbquadColorTable[nColor].rgbRed = (uchar)nColor;
		}
		cimage.SetColorTable(0, nMaxColors, rgbquadColorTable);
		delete[]rgbquadColorTable;
	}


	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (mat.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				*(pucImage + nRow * nStep + nCol) = pucRow[nCol];
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					*(pucImage + nRow * nStep + nCol * 3 + nCha) = pucRow[nCol * 3 + nCha];
				}
			}
		}
	}
}

void CAEyeView::cacheImage(cv::Mat& mat)
{
	// cache 1 image in nFrameStride image,  1/nFrameStride images
	int nFrameStride = 1;
	static int nFrame = 0;
	if (nFrame++ % nFrameStride)
		return;

	cv::Rect rect(boxDetection.left, boxDetection.top, boxDetection.right - boxDetection.left, boxDetection.bottom - boxDetection.top);
	cv::rectangle(mat, rect, cv::Scalar(0, 0, 255));
	
	long msTime = clock() - m_msTimeStampBegin;
	long sTimeInteger = msTime/1000;

	std::ostringstream os;

	os << std::setw(3) << std::setfill('0') << sTimeInteger / 60 << "min";

	os << std::setw(2) << std::setfill('0') << sTimeInteger % 60 << "s";

	os << std::setw(3) << std::setfill('0') << msTime % 1000 << "ms" << ".jpg\0";

	//m_ImageSnapshotMap.insert(ImageSnapshotPair(os.str(), mat));
	writeVideoKernel(mat);
}

void CAEyeView::saveImagesOnce()
{
	for (ImageSnapshotMap::iterator itr = m_ImageSnapshotMap.begin(); itr != m_ImageSnapshotMap.end(); itr++)
	{
		cv::imwrite( m_pathSub + itr->first, itr->second);

		itr->second.release();
	}

	m_ImageSnapshotMap.clear();
}

bool CAEyeView::writeVideoBegin(const string& filename, int width, int height)
{
	m_VideoWriter.open(filename, CV_FOURCC('M', 'P', '4', 'V'), 25.0f, cv::Size(width, height));
	// check if we succeeded
	if (!m_VideoWriter.isOpened()) {
		cerr << "Could not open the output video file for write\n";
		return false;
	}

	return true;
}

void CAEyeView::writeVideoKernel(cv::Mat &img)
{
	if (!m_VideoWriter.isOpened())
		return;

	m_VideoWriter.write(img);
}

void CAEyeView::writeVideoEnd()
{
	if (!m_VideoWriter.isOpened())
		return;

	m_VideoWriter.release();
}

void CAEyeView::updateUI(cv::Mat &img, int msTime, string file)
{
	clock_t t = clock();

	std::ostringstream os;

	os << "---------- ʶ��ķ� "
		<< msTime << " ����, " << std::endl;

	CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();

	std::vector<tstring> labels = classifier.getLabels();

	os << "��⵽ " << m_DetectionMap.size() << " ��Ŀ��" << endl;
	os << "score - label" << std::endl;

	for (DetectionMap::iterator itr = m_DetectionMap.begin(); itr != m_DetectionMap.end(); ++itr) {
		Detection& p = itr->second;

		if (p.label >= labels.size())
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

	//Detection bestDetection = findBestScore(detections);

	if (1/*view*/)
	{
		if (!image.IsNull())
			image.Destroy();

		if (!file.empty())
			image.Load(file.c_str());
		else
		{
			MatToCImage(img, image);
		}
		/*
		if (detections.size() != 0)
		{
			int nLabelId = int(bestDetection.label + 0.01) - 1;
			m_currentClassNamePredict = classifier.getLabels()[nLabelId];
		}

		boxDetection = CRect(bestDetection.xmin * image.GetWidth(), bestDetection.ymin * image.GetHeight(),
			bestDetection.xmax * image.GetWidth(), bestDetection.ymax * image.GetHeight());
		*/
	}

	msTime = clock() - t;

	os << "---------- �����ķ� "
		<< msTime << " ����, " << std::endl;

	outputInfo(os.str().c_str());

}

void CAEyeView::updateUI(string &shortname, string &file, std::vector<Detection> &detections, int msTime, int type /*= LOG_TYPE_UI_ALL*/)
{
	cv::Mat img = cv::imread(file, -1);
	if (img.empty()) // "Unable to decode image " << file;
		return ;

	updateUI(img, msTime, file);
}

void CAEyeView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == 0)
		OnDraw(NULL);

	CView::OnTimer(nIDEvent);
}


int CAEyeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	SetTimer(0, 40, NULL);	//��ʱ��ʾ��һ��40���봥��һ�εĶ�ʱ����25֡/�� 

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

	model_file = vecSetting[0];
	trained_file = vecSetting[1];
	mean_file = vecSetting[2];
	label_file = vecSetting[3];
	
	mean_value = "104,117,123";
	if (!mean_file.empty())
		mean_value = "";

	classifier.load(model_file, trained_file, mean_file, label_file, mean_value);

	classifier.setTestType(ENUM_DETECTION);

	outputInfo("ʶ��ģ��������ɣ�");
	outputInfo("");

	std::string strDemo("..\\..\\examples\\images\\cat.jpg");
	int msTime = 0;
	std::vector<Prediction> result;
	std::vector<Detection> resultDetection;

	if (classifier.getTestType() == ENUM_CLASSIFICATION) {
		if (predict(strDemo, result, msTime))
			outputInfo("Ŀ��ʶ�������ɣ�");
	}
	else {
		if (detect(strDemo, resultDetection, msTime))
			outputInfo("Ŀ��ʶ�����������ɣ�");
	}

	outputInfo("");


}


// AEyeView.h : CAEyeView ��Ľӿ�
//

#pragma once

#include "Classifier.h"
#include "commonDefinition.h"

typedef std::pair<std::string, std::vector<Prediction>>	PredictionResultPair;
typedef std::map<std::string, std::vector<Prediction>>	PredictionResultMap;

typedef std::pair<std::string, std::vector<Detection>>	DetectionResultPair;
typedef std::map<std::string, std::vector<Detection>>	DetectionResultMap;




class CAEyeView : public CView
{
protected: // �������л�����
	CAEyeView();
	DECLARE_DYNCREATE(CAEyeView)

// ����
public:
	CAEyeDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CAEyeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();

	void cachePredictionResult(string shortname, std::vector<Prediction> result, string file);
	void cacheDetectionResult(string shortname, std::vector<Detection> result, string file);

	bool predict(string &file, std::vector<Prediction> &predictions, int &msTime);
	bool detect(string &file, std::vector<Detection> &detections, int &msTime);

	void	switchBilViewByName(std::string name);
	void AddFileViewBranch(std::string fileNameShort, float score);

	void AddFileViewBranch(ClassTop1Map& names);
	void AddFileViewBranch(FilesMap& names);

	void	sortPredictionResult();

protected:
	void	setDefault();
	void	release();
	void	outputInfo(const char* message, int value = -1);	// print to output window or status bar

	void	updateUI(string &shortname, string &file, std::vector<Prediction> &predictions, int msTime, int type = LOG_TYPE_UI_ALL);
	void	updateUI(string &shortname, string &file, std::vector<Detection> &detections, int msTime, int type = LOG_TYPE_UI_ALL);

	Detection findBestScore(std::vector<Detection>& detections);

protected:
	Classifier classifier;

	DetectionResultMap	m_DetectionResultList;
	PredictionResultMap	m_PredictionResultList;
	FilesMap			m_FilesMap;

	ClassTop1Map		m_ClassTop1Map;

	string m_currentClassNamePredict;

	string model_file;
	string trained_file;
	string mean_file;
	string label_file;
	string mean_value;

	CImage image;

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnOpenFileList();

	afx_msg void OnSetting();
};

#ifndef _DEBUG  // AEyeView.cpp �еĵ��԰汾
inline CAEyeDoc* CAEyeView::GetDocument() const
   { return reinterpret_cast<CAEyeDoc*>(m_pDocument); }
#endif


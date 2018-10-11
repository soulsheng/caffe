
// AEyeView.h : CAEyeView 类的接口
//

#pragma once

#include "Classifier.h"
#include "commonDefinition.h"

typedef std::pair<std::string, std::vector<Prediction>>	PredictionResultPair;
typedef std::vector<PredictionResultPair>	PredictionResultList;
typedef std::map<std::string, std::vector<Prediction>>	PredictionResultMap;

class CAEyeView : public CView
{
protected: // 仅从序列化创建
	CAEyeView();
	DECLARE_DYNCREATE(CAEyeView)

// 特性
public:
	CAEyeDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CAEyeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();

	void predict(string &file, std::vector<Prediction> &predictions, int &msTime);

	void	switchBilViewByName(std::string name);
	void AddFileViewBranch(std::string fileNameShort);

protected:
	void	setDefault();
	void	release();
	void	outputInfo(const char* message, int value = -1);	// print to output window or status bar

	void	updateUI(string &file, std::vector<Prediction> &predictions, int msTime, int type = LOG_TYPE_UI_ALL);

protected:
	Classifier classifier;

	PredictionResultMap	m_PredictionResultList;

	string model_file = "G:\\file\\code\\dnn\\caffe\\models\\bvlc_reference_caffenet\\deploy.prototxt";
	string trained_file = "G:\\file\\code\\dnn\\caffe\\models\\bvlc_reference_caffenet\\bvlc_reference_caffenet.caffemodel";
	string mean_file = "../../data/ilsvrc12/imagenet_mean.binaryproto";
	string label_file = "../../data/ilsvrc12/synset_words_cn.txt";

	CImage image;

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnOpenFileList();

	void getFilePathFromDialog(std::string &path);
	void getFileListFromPath(std::string &path, std::vector<std::string> &list);

};

#ifndef _DEBUG  // AEyeView.cpp 中的调试版本
inline CAEyeDoc* CAEyeView::GetDocument() const
   { return reinterpret_cast<CAEyeDoc*>(m_pDocument); }
#endif


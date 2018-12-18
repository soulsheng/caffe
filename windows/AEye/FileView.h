
#pragma once

#include "ViewTree.h"
#include <string>
#include "commonDefinition.h"

class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CFileView : public CDockablePane
{
// 构造
public:
	CFileView();

	void AdjustLayout();
	void OnChangeVisualStyle();

	void	AddBranch(std::string name, float score);
	void	AddBranch(ClassTop1Map& names);
	void	AddBranch(FilesMap& names);

	HTREEITEM InsertItemByScore(float score, std::string filename, int nCountSub=0);

// 特性
protected:

	CViewTree m_wndFileView;
	CImageList m_FileViewImages;
	CFileViewToolBar m_wndToolBar;

	HTREEITEM	m_hRootCertainMost;
	HTREEITEM	m_hRootCertain;
	HTREEITEM	m_hRootMiddle;
	HTREEITEM	m_hRootUncertain;
	HTREEITEM	m_hRootUncertainMost;
	HTREEITEM	m_hRootCurrent;
	HTREEITEM	m_hRootUnsort;

protected:
	void FillFileView();

// 实现
public:
	virtual ~CFileView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	void createRootItem();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnCollapseAll();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenWith();
	afx_msg void OnExpandAll();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
};


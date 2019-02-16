
#include "stdafx.h"
#include "mainfrm.h"
#include "FileView.h"
#include "Resource.h"
#include "AEye.h"

#include "AEyeDoc.h"
#include "AEyeView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CFileView::CFileView()
{
}

CFileView::~CFileView()
{
}

BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_COLLAPES_ALL, OnCollapseAll)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	ON_COMMAND(ID_EXPAND_ALL_File, OnExpandAll)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图: 
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建文件视图\n");
		return -1;      // 未能创建
	}

	// 加载视图图像: 
	m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由: 
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// 填入一些静态树视图数据(此处只需填入虚拟代码，而不是复杂的数据)
	//FillFileView();
	createRootItem();


	AdjustLayout();

	return 0;
}

void CFileView::createRootItem()
{
	m_hRootCertainMost = m_wndFileView.InsertItem(_T("极确定列表(p>0.9)"), 0, 0);
	m_wndFileView.SetItemState(m_hRootCertainMost, TVIS_BOLD, TVIS_BOLD);

	m_hRootCertain = m_wndFileView.InsertItem(_T("确定列表(p>0.6)"), 0, 0);
	m_wndFileView.SetItemState(m_hRootCertain, TVIS_BOLD, TVIS_BOLD);

	m_hRootMiddle = m_wndFileView.InsertItem(_T("中间列表(p>0.3)"), 0, 0);
	m_wndFileView.SetItemState(m_hRootMiddle, TVIS_BOLD, TVIS_BOLD);

	m_hRootUncertain = m_wndFileView.InsertItem(_T("不确定列表(p>0.1)"), 0, 0);
	m_wndFileView.SetItemState(m_hRootUncertain, TVIS_BOLD, TVIS_BOLD);

	m_hRootUncertainMost = m_wndFileView.InsertItem(_T("极不确定列表(p<0.1)"), 0, 0);
	m_wndFileView.SetItemState(m_hRootUncertainMost, TVIS_BOLD, TVIS_BOLD);

	m_hRootUnsort = m_wndFileView.InsertItem(_T("未排序列表"), 0, 0);
	m_wndFileView.SetItemState(m_hRootUnsort, TVIS_BOLD, TVIS_BOLD);

}

void CFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CFileView::AddBranch(std::string name, float score)
{
	HTREEITEM hSubCurrent = InsertItemByScore(score, name);
	m_wndFileView.Expand(m_hRootCurrent, TVE_EXPAND);

	m_wndFileView.SelectItem(hSubCurrent);

	CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();
	OnSetFocus(pFrame);
}

void CFileView::AddBranch(ClassTop1Map& names)
{
	m_wndFileView.DeleteAllItems();

	createRootItem();

	for (ClassTop1Map::iterator itrCTM = names.begin(); itrCTM != names.end(); itrCTM++)
	{
		Top1Map		&top1Map = itrCTM->second;
		if (top1Map.empty())
			continue;

		HTREEITEM hRootSubCurrent;

		Top1Map::iterator itr = top1Map.begin();

		hRootSubCurrent = InsertItemByScore(itr->first, itrCTM->first, top1Map.size());

		for (; itr != top1Map.end(); itr++)
			m_wndFileView.InsertItem(itr->second.c_str(), 2, 2, hRootSubCurrent);

	}

	m_wndFileView.Expand(m_hRootCertainMost, TVE_EXPAND);
	m_wndFileView.SelectItem(m_hRootCertainMost);
}

void CFileView::AddBranch(FilesMap& names)
{
	for (FilesMap::iterator itr = names.begin(); itr != names.end(); itr++)
		m_wndFileView.InsertItem(itr->first.c_str(), 2, 2, m_hRootUnsort);

	m_wndFileView.Expand(m_hRootUnsort, TVE_EXPAND);
	m_wndFileView.SelectItem(m_hRootUnsort);
}

HTREEITEM CFileView::InsertItemByScore(float score, std::string filename, int nCountSub /*= 0*/)
{
	HTREEITEM hRootSubCurrent;

	if (score > 0.9)
		m_hRootCurrent = m_hRootCertainMost;
	else if (score > 0.6)
		m_hRootCurrent = m_hRootCertain;
	else if (score > 0.3)
		m_hRootCurrent = m_hRootMiddle;
	else if (score > 0.1)
		m_hRootCurrent = m_hRootUncertain;
	else
		m_hRootCurrent = m_hRootUncertainMost;

	std::ostringstream os;
	os << filename << "(" << nCountSub << ")";

	hRootSubCurrent = m_wndFileView.InsertItem(os.str().c_str(), 2, 2, m_hRootCurrent);

	return hRootSubCurrent;
}

void CFileView::FillFileView()
{
	HTREEITEM hRoot = m_wndFileView.InsertItem(_T("FakeApp 文件"), 0, 0);
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hSrc = m_wndFileView.InsertItem(_T("FakeApp 源文件"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeApp.rc"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeAppDoc.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeAppView.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("MainFrm.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("StdAfx.cpp"), 1, 1, hSrc);

	HTREEITEM hInc = m_wndFileView.InsertItem(_T("FakeApp 头文件"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("FakeAppDoc.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("FakeAppView.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("Resource.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("MainFrm.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("StdAfx.h"), 2, 2, hInc);

	HTREEITEM hRes = m_wndFileView.InsertItem(_T("FakeApp 资源文件"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.ico"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeApp.rc2"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeAppDoc.ico"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeToolbar.bmp"), 2, 2, hRes);

	m_wndFileView.Expand(hRoot, TVE_EXPAND);
	m_wndFileView.Expand(hSrc, TVE_EXPAND);
	m_wndFileView.Expand(hInc, TVE_EXPAND);
}

void CFileView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项: 
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFileView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CFileView::OnCollapseAll()
{
	m_wndFileView.Expand(m_wndFileView.GetSelectedItem(), TVE_COLLAPSE);

}

void CFileView::OnFileOpen()
{
	// TODO:  在此处添加命令处理程序代码
	m_wndFileView.Expand(m_wndFileView.GetSelectedItem(), TVE_EXPAND);

}

void CFileView::OnFileOpenWith()
{
	// TODO:  在此处添加命令处理程序代码
	CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();
	CAEyeView* pView = (CAEyeView *)pFrame->GetActiveView();

	pView->sortPredictionResult();
}

void CFileView::OnExpandAll()
{
	// TODO:  在此处添加命令处理程序代码
	HTREEITEM hChild = m_wndFileView.GetChildItem(m_wndFileView.GetSelectedItem());

	if (hChild == NULL)
		return;

	do
	{
		m_wndFileView.Expand(hChild, TVE_EXPAND);

		hChild = m_wndFileView.GetNextSiblingItem(hChild);	//兄弟节点使用循环，较清晰

	} while (hChild);

}

void CFileView::OnEditCut()
{
	// TODO:  在此处添加命令处理程序代码
}

void CFileView::OnEditCopy()
{
	// TODO:  在此处添加命令处理程序代码
}

void CFileView::OnEditClear()
{
	// TODO:  在此处添加命令处理程序代码
}

void CFileView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CFileView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();
}

void CFileView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* 锁定*/);

	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("无法加载位图:  %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}



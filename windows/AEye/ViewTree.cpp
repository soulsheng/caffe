
#include "stdafx.h"
#include "ViewTree.h"

#include "mainfrm.h"
#include "AEyeDoc.h"
#include "AEyeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
}

CViewTree::~CViewTree()
{
}

void CViewTree::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{

	HTREEITEM hTreeItem = GetSelectedItem();
	CString name = GetItemText(hTreeItem);

	CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();
	CAEyeView* pView = (CAEyeView *)pFrame->GetActiveView();

	if (!pView)
		return;

	pView->switchBilViewByName( name.GetBuffer(0) );

	*pResult = 0;

}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CViewTree::OnSelChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree 消息处理程序

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}


#pragma once

/////////////////////////////////////////////////////////////////////////////
// CViewTree ����

class CViewTree : public CTreeCtrl
{
// ����
public:
	CViewTree();

// ��д
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// ʵ��
public:
	virtual ~CViewTree();
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	DECLARE_MESSAGE_MAP()
};

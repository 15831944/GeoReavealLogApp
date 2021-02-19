// deferWndMove.cpp: implementation of the CDeferWndMove class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "deferWndMove.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeferWndMove::CDeferWndMove(int nNumWindows)
{
	m_bMoveActive = NULL;
	m_nMaxWnds = 0;
	m_nWndCount = 0;
	m_hdwp = NULL;

	BeginMove(nNumWindows);
}

CDeferWndMove::~CDeferWndMove()
{
	if (m_bMoveActive)
		EndMove();
}

BOOL CDeferWndMove::BeginMove(int nNumWindows)
{
	BOOL bRes = FALSE;
	ASSERT (nNumWindows > 0);

	if (m_bMoveActive)
		EndMove();

	if (nNumWindows > 0)
	{
		m_hdwp = ::BeginDeferWindowPos(nNumWindows);

		if (m_hdwp)
		{
			m_bMoveActive = TRUE;
			m_nMaxWnds = nNumWindows;
			bRes = TRUE;
		}
	}

	return bRes;
}

BOOL CDeferWndMove::EndMove()
{
	ASSERT (m_bMoveActive);

	if (!m_bMoveActive)
		return FALSE;

	return ::EndDeferWindowPos(m_hdwp);
}

BOOL CDeferWndMove::MoveWindow(CWnd* pWnd, int x, int y, int nWidth, int nHeight, BOOL bRepaint)
{
	ASSERT (m_bMoveActive && m_nWndCount < m_nMaxWnds);

	if (!m_bMoveActive || m_nWndCount >= m_nMaxWnds)
		return FALSE;

	int nFlags = SWP_NOACTIVATE | SWP_NOZORDER;

	HDWP hdwp = ::DeferWindowPos(m_hdwp, pWnd->GetSafeHwnd(), HWND_BOTTOM, x, y, nWidth, nHeight, nFlags);
	ASSERT (!hdwp || hdwp == m_hdwp);

	if (m_hdwp)
		m_nWndCount++;
	else
		pWnd->MoveWindow(x, y, nWidth, nHeight, bRepaint);

	return TRUE;
}

BOOL CDeferWndMove::MoveWindow(CWnd* pWnd, LPCRECT lpRect, BOOL bRepaint)
{
	return MoveWindow(pWnd, lpRect->left, lpRect->top, 
							lpRect->right - lpRect->left,
							lpRect->bottom - lpRect->top, bRepaint);
}

CRect CDeferWndMove::OffsetCtrl(CWnd* pParent, UINT nCtrlID, int x, int y)
{
	CWnd* pCtrl = pParent->GetDlgItem(nCtrlID);

	if (pCtrl)
	{
		CRect rChild;
		pCtrl->GetWindowRect(rChild);
		pParent->ScreenToClient(rChild);

		if (x || y)
		{
			rChild.OffsetRect(x, y);
			MoveWindow(pCtrl, rChild); // our own version
		}

		return rChild;
	}

	return CRect(0, 0, 0, 0);
}

CRect CDeferWndMove::ResizeCtrl(CWnd* pParent, UINT nCtrlID, int cx, int cy)
{
	CWnd* pCtrl = pParent->GetDlgItem(nCtrlID);

	if (pCtrl)
	{
		CRect rChild, rParent;
		pCtrl->GetWindowRect(rChild);
		pParent->ScreenToClient(rChild);
		pParent->GetClientRect(rParent);

		if (cx || cy)
		{
			rChild.right += cx;
			rChild.bottom += cy;

			// make sure it also intersects with parent
			if (rChild.IntersectRect(rChild, rParent))
				MoveWindow(pCtrl, rChild); // our own version
		}

		return rChild;
	}

	return CRect(0, 0, 0, 0);
}

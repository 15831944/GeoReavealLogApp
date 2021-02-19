// RuntimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RuntimeDlg.h"
#include "dlgunits.h"
#include "winstyles.h"
#include "winclasses.h"
#include "wclassdefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRuntimeDlg

class RTDLGTEMPLATE : private DLGTEMPLATE
{
public:
	RTDLGTEMPLATE(DWORD dwStyle, DWORD dwExStyle, const CRect& rect)
	{
		style = dwStyle;
		dwExtendedStyle = dwExStyle;
		x = (short)rect.left;
		y = (short)rect.top;
		cx = (short)rect.Width();
		cy = (short)rect.Height();

		CDlgUnits().FromPixels(x, y);
		CDlgUnits().FromPixels(cx, cy);

		cdit = 0; // always 0
		wMenu = 0; // always 0
		wClass = 0; // always 0
		nCaption = 0; // always 0
	}

	virtual ~RTDLGTEMPLATE()
	{
	}

	operator LPCDLGTEMPLATE() { return (LPCDLGTEMPLATE)this; }

private:
	WORD wMenu;
	WORD wClass;
	short nCaption;
};

/////////////////////////////////////////////////////////////////////////////

const CRect CRuntimeDlg::rectAuto = CRect(0, 0, 0, 0);
CMapStringToString CRuntimeDlg::s_mapClasses;

CRuntimeDlg::CRuntimeDlg() : m_hILarge(NULL), m_hISmall(NULL)
{
	AfxInitRichEdit();

	if (!s_mapClasses.GetCount())
		BuildClassMap();

	SetBordersDLU(7);
}

CRuntimeDlg::~CRuntimeDlg()
{
	::DestroyIcon(m_hILarge);
	::DestroyIcon(m_hISmall);
}

BEGIN_MESSAGE_MAP(CRuntimeDlg, CDialog)
	//{{AFX_MSG_MAP(CRuntimeDlg)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRuntimeDlg message handlers

// private mfc functions we need to access
extern void AFXAPI AfxHookWindowCreate(CWnd* pWnd);
extern BOOL AFXAPI AfxUnhookWindowCreate();

void CRuntimeDlg::OnCancel()
{
	// filter out if we're a child
	if (GetStyle() & WS_CHILD)
		return;

	CDialog::OnCancel();
}

void CRuntimeDlg::OnOK()
{
	// filter out if we're a child
	if (GetStyle() & WS_CHILD)
		return;

	CDialog::OnOK();
}

int CRuntimeDlg::DoModal(LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, const CRect& rect, CWnd* pParentWnd, UINT nID)
{
	// fail immediately if no controls have been added
	if (!m_lstControls.GetCount())
		return IDCANCEL;

	// setup for modal loop and creation
	m_nModalResult = -1;
	m_nFlags |= WF_CONTINUEMODAL;

	// disable parent (before creating dialog)
	HWND hWndParent = PreModal();
	AfxUnhookWindowCreate();

	BOOL bEnableParent = FALSE;
	if (hWndParent != NULL && ::IsWindowEnabled(hWndParent))
	{
		::EnableWindow(hWndParent, FALSE);
		bEnableParent = TRUE;
	}

	try
	{
		// create modeless dialog
		if (Create(szCaption, dwStyle, dwExStyle, rect, pParentWnd ? pParentWnd : CWnd::FromHandle(hWndParent), nID))
		{
			if (m_nFlags & WF_CONTINUEMODAL)
			{
				// enter modal loop
				DWORD dwFlags = MLF_SHOWONIDLE;
				if (GetStyle() & DS_NOIDLEMSG)
					dwFlags |= MLF_NOIDLEMSG;
				VERIFY(RunModalLoop(dwFlags) == m_nModalResult);
			}

			// hide the window before enabling the parent, etc.
			if (m_hWnd != NULL)
				SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
		}
	}
	catch(...)
	{
		m_nModalResult = -1;
	}

	if (bEnableParent)
		::EnableWindow(hWndParent, TRUE);

	if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
		::SetActiveWindow(hWndParent);

	// destroy modal window
	DestroyWindow();
	PostModal();

	return m_nModalResult;
}

BOOL CRuntimeDlg::OnInitDialog() 
{
	// make sure we create the controls before calling the base class
	
	CreateControls();

	CDialog::OnInitDialog();
	CRect rect;
	// get the current window size and position
	GetWindowRect( rect );	
	::SetWindowPos(
		m_hWnd ,        // handle to window
		HWND_TOPMOST,   // placement-order handle
		rect.left,      // horizontal position
		rect.top,       // vertical position
		rect.Width(),	// width
		rect.Height(),  // height
		SWP_SHOWWINDOW  // window-positioning options
	);
	// add icons only if we have no parent
	if (!GetParent())
	{
		CWinApp* pApp = AfxGetApp();

		char szAppPath[MAX_PATH + 1];
		::GetModuleFileName(NULL, szAppPath, MAX_PATH);

		if (::ExtractIconEx(szAppPath, 0, &m_hILarge, &m_hISmall, 1))
		{
			SetIcon(m_hILarge ? m_hILarge : m_hISmall, TRUE);
			SetIcon(m_hISmall ? m_hISmall : m_hILarge, FALSE);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CRuntimeDlg::Create(LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, const CRect& rect, CWnd* pParentWnd, UINT nID)
{
	// cache and remove visibility
	BOOL bVisible = (dwStyle & WS_VISIBLE);
	dwStyle &= ~WS_VISIBLE;

	// remove DS_SETFONT (not supported)
	dwStyle &= ~DS_SETFONT;

	// create modeless dialog
	AfxHookWindowCreate(this);

	if (CreateDlgIndirect(RTDLGTEMPLATE(dwStyle, dwExStyle, rect), pParentWnd, NULL) != NULL)
	{
		// size to fit?
		if (rect == rectAuto)
			AutoFit();
		else
			MoveWindow(rect);

		// center
		if (dwStyle & DS_CENTER)
			CenterWindow();

		// set window text
		SetWindowText(szCaption);

		// set control id
		SetDlgCtrlID(nID);

		PostCreate(); // for derived classes

		// reshow?
		if (bVisible)
			ShowWindow(SW_SHOW);

		return TRUE;
	}

	return FALSE;
}

void CRuntimeDlg::AutoFit()
{
	// iterate all the child controls accumulating the largest bottom right coord
	CRect rClient(0, 0, 0, 0);

	CWnd* pCtrl = GetWindow(GW_CHILD);

	while (pCtrl)
	{
		CRect rCtrl;
		pCtrl->GetWindowRect(rCtrl);
		ScreenToClient(rCtrl);

		rClient.right = max(rClient.right, rCtrl.right);
		rClient.bottom = max(rClient.bottom, rCtrl.bottom);

		pCtrl = pCtrl->GetNextWindow();
	}

	// add border
	rClient.right += m_rBorders.right;
	rClient.bottom += m_rBorders.bottom;

	// calc new window rect
	CRect rWindow;
	GetWindowRect(rWindow);
	CalcWindowRect(rClient);

	// match centerpoints of old and new
	rClient = CRect(rWindow.TopLeft(), rClient.Size());
	rClient.OffsetRect(rWindow.CenterPoint() - rClient.CenterPoint());

	MoveWindow(rClient);
}

BOOL CRuntimeDlg::AddControl(LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, 
							 const CRect& rect, UINT nID)
{
	return AddControl(szClass, szCaption, dwStyle, dwExStyle, rect.left, rect.top, rect.Width(), rect.Height(), nID);
}

BOOL CRuntimeDlg::AddControl(LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, 
							 int x, int y, int cx, int cy, UINT nID)
{
	if (GetSafeHwnd())
		return (NULL != CreateControl(szClass, szCaption, dwStyle, dwExStyle, x, y, cx, cy, nID, FALSE));

	else
		m_lstControls.AddTail(RTCONTROL(NULL, szClass, szCaption, dwStyle, dwExStyle, CRect(x, y, x + cx, y + cy), nID, FALSE));

	return TRUE;
}

BOOL CRuntimeDlg::AddControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, 
							 const CRect& rect, UINT nID)
{
	return AddControl(pWnd, szCaption, dwStyle, dwExStyle, rect.left, rect.top, rect.Width(), rect.Height(), nID);
}

BOOL CRuntimeDlg::AddControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
							 int x, int y, int cx, int cy, UINT nID)
{
	if (GetSafeHwnd())
		return CreateControl(pWnd, szCaption, dwStyle, dwExStyle, x, y, cx, cy, nID, FALSE);

	else
		m_lstControls.AddTail(RTCONTROL(pWnd, NULL, szCaption, dwStyle, dwExStyle, CRect(x, y, x + cx, y + cy), nID, FALSE));

	return TRUE;
}

BOOL CRuntimeDlg::AddRCControl(LPCTSTR szRCType, LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, 
							   DWORD dwExStyle, int x, int y, int cx, int cy, UINT nID, UINT nIconID)
{
	CString sClass(szClass);

	// get the win32 class name
	if (sClass.IsEmpty() || stricmp(szRCType, "CONTROL") != 0)
	{
		if (!CRCCtrlParser::GetClassName(szRCType, sClass) || sClass.IsEmpty())
			return FALSE;
	}

	if (!dwStyle)
		dwStyle = CRCCtrlParser::GetDefaultStyles(sClass);

	if (!(dwStyle & WS_NOTVISIBLE))
		dwStyle |= WS_VISIBLE;
	else
		dwStyle &= ~WS_NOTVISIBLE;

	if (CRCCtrlParser::CtrlWantsClientEdge(sClass))
		dwExStyle |= WS_EX_CLIENTEDGE;

	if (GetSafeHwnd())
		return (NULL != CreateControl(sClass, szCaption, dwStyle, dwExStyle, x, y, cx, cy, nID, TRUE, nIconID));

	else
		m_lstControls.AddTail(RTCONTROL(NULL, sClass, szCaption, dwStyle, dwExStyle, CRect(x, y, x + cx, y + cy), nID, TRUE, nIconID));

	return TRUE;
}

BOOL CRuntimeDlg::AddRCControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle, 
							   DWORD dwExStyle, int x, int y, int cx, int cy, UINT nID, UINT nIconID)
{
	CString sClass = GetClassName(pWnd);

	if (!dwStyle)
		dwStyle = CRCCtrlParser::GetDefaultStyles(sClass);

	if (!(dwStyle & WS_NOTVISIBLE))
		dwStyle |= WS_VISIBLE;
	else
		dwStyle &= ~WS_NOTVISIBLE;

	if (CRCCtrlParser::CtrlWantsClientEdge(sClass))
		dwExStyle |= WS_EX_CLIENTEDGE;

	if (GetSafeHwnd())
		return (NULL != CreateControl(pWnd, szCaption, dwStyle, dwExStyle, x, y, cx, cy, nID, TRUE, nIconID));

	else
		m_lstControls.AddTail(RTCONTROL(pWnd, sClass, szCaption, dwStyle, dwExStyle, CRect(x, y, x + cx, y + cy), nID, TRUE, nIconID));

	return TRUE;
}

HWND CRuntimeDlg::CreateControl(LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, 
								int x, int y, int cx, int cy, UINT nID, BOOL bDLU, UINT nIconID)
{
	if (bDLU)
	{
		CDlgUnits dlu(*this);

		dlu.ToPixels(x, y);
		dlu.ToPixels(cx, cy);
	}

	// validate the topleft position against the borders
	x = max(x, m_rBorders.left);
	y = max(y, m_rBorders.top);

	// make reasonably sure control is child
	dwStyle |= WS_CHILD;
	dwStyle &= ~WS_POPUP;

	HWND hwnd = ::CreateWindowEx(dwExStyle, szClass, szCaption, dwStyle, x, y, cx, cy, *this, NULL, NULL, NULL);

	if (hwnd)
	{
		SetWindowLong(hwnd, GWL_ID, nID);

		HFONT hFont = (HFONT)SendMessage(WM_GETFONT); // gets the dialog's font

		if (!hFont)
			hFont = (HFONT)::GetStockObject(ANSI_VAR_FONT);

		::SendMessage(hwnd, WM_SETFONT, (WPARAM)hFont, 0); 

		if (nIconID && stricmp(szClass, "static") == 0)
		{
			HICON hIcon = AfxGetApp()->LoadIcon(nIconID);

			if (hIcon)
				::SendMessage(hwnd, STM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
		}
	}
	else
		TRACE("CreateWindowEx(%s) failed. GetLastError returned %08X\n", szClass, GetLastError());

	return hwnd;
}

BOOL CRuntimeDlg::CreateControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, 
								int x, int y, int cx, int cy, UINT nID, BOOL bDLU, UINT nIconID)
{
	ASSERT (!pWnd->GetSafeHwnd());

	if (pWnd->GetSafeHwnd())
		return FALSE;

	CString sClass = GetControlClassName(pWnd);

	if (sClass.IsEmpty())
		return FALSE;

	HWND hwnd = CreateControl(sClass, szCaption, dwStyle, dwExStyle, x, y, cx, cy, nID, bDLU, nIconID);

	if (hwnd)
		return pWnd->SubclassWindow(hwnd);

	// else
	return FALSE;
}


BOOL CRuntimeDlg::CreateControl(const RTCONTROL& rtc)
{
	if (rtc.m_pWnd)
		return CreateControl(rtc.m_pWnd, rtc.m_sCaption, rtc.m_dwStyle, rtc.m_dwExStyle, 
							rtc.m_rect.left, rtc.m_rect.top, rtc.m_rect.Width(), rtc.m_rect.Height(), 
							rtc.m_nID, rtc.m_bDLU, rtc.m_nIconID);
	
	else if (!rtc.m_sClass.IsEmpty())
		return (NULL != CreateControl(rtc.m_sClass, rtc.m_sCaption, rtc.m_dwStyle, rtc.m_dwExStyle, 
							rtc.m_rect.left, rtc.m_rect.top, rtc.m_rect.Width(), rtc.m_rect.Height(), 
							rtc.m_nID, rtc.m_bDLU, rtc.m_nIconID));

	else
		return FALSE;
}

void CRuntimeDlg::CreateControls()
{
	ASSERT (GetSafeHwnd());

	if (!GetSafeHwnd())
		return;

	while (m_lstControls.GetCount())
		CreateControl(m_lstControls.RemoveHead());
}

CString CRuntimeDlg::GetClassName(CWnd* pWnd)
{
	ASSERT (pWnd);

	if (!pWnd)
		return "";

	CString sName;

	::GetClassName(*pWnd, sName.GetBuffer(128), 127);
	sName.ReleaseBuffer();

	return sName;
}

CString CRuntimeDlg::GetControlClassName(CWnd* pWnd)
{
	ASSERT (pWnd);

	if (!pWnd)
		return "";

	// if there is no permanent mapping to this CWnd just return GetClassName()
	// but only if it has a window handle attached else it must be a real CWnd
	if (pWnd->GetSafeHwnd() && CWnd::FromHandlePermanent(*pWnd) != pWnd)
		return GetClassName(pWnd);

	CRuntimeClass* pRTClass = pWnd->GetRuntimeClass();

	// work our way up the derivation chain till we find a match
	while (pRTClass)
	{
		CString sWinClass, sRTClass = pRTClass->m_lpszClassName;

		if (s_mapClasses.Lookup(sRTClass, sWinClass))
			return sWinClass;

		// try ancestor
#ifdef _AFXDLL
		pRTClass = (*pRTClass->m_pfnGetBaseClass)();
#else
		pRTClass = pRTClass->m_pBaseClass;
#endif
	}

	// means we did not find anything
	ASSERT (0);
	return "";
}

void CRuntimeDlg::BuildClassMap()
{
	if (s_mapClasses.GetCount())
		return; // already done

	s_mapClasses["CStatic"] = "static";
	s_mapClasses["CButton"] = "button";
	s_mapClasses["CListBox"] = "listbox";
	s_mapClasses["CComboBox"] = "combobox";
	s_mapClasses["CEdit"] = "edit";
	s_mapClasses["CScrollBar"] = "scrollbar";
	s_mapClasses["CListCtrl"] = "SysListView32";
	s_mapClasses["CTreeCtrl"] = "SysTreeView32";
	s_mapClasses["CSpinButtonCtrl"] = "msctls_updown32";
	s_mapClasses["CHeaderCtrl"] = "SysHeader32";
	s_mapClasses["CSliderCtrl"] = "msctls_trackbar32";
	s_mapClasses["CProgressCtrl"] = "msctls_progress32";
	s_mapClasses["CHotKeyCtrl"] = "msctls_hotkey32";
	s_mapClasses["CToolTipCtrl"] = "tooltips_class32";
	s_mapClasses["CTabCtrl"] = "SysTabControl";
	s_mapClasses["CAnimateCtrl"] = "SysAnimate32";
	s_mapClasses["CToolBarCtrl"] = "toolbarwindow32";
	s_mapClasses["CStatusBarCtrl"] = "msctls_statusbar32";
	s_mapClasses["CRichEditCtrl"] = "RichEdit20W";
	s_mapClasses["CIPAddressCtrl"] = "SysIPAddress32";
	s_mapClasses["CDateTimeCtrl"] = "SysDateTimePick32";
	s_mapClasses["CMonthCalCtrl"] = "SysMonthCal32";
	s_mapClasses["CReBar"] = "ReBarWindow32";
//	s_mapClasses[""] = "";
}

void CRuntimeDlg::SetBorders(int nLeft, int nTop, int nRight, int nBottom)
{
	m_rBorders.SetRect(max(0, nLeft), max(0, nTop), max(0, nRight), max(0, nBottom));
}

void CRuntimeDlg::SetBordersDLU(int nLeft, int nTop, int nRight, int nBottom)
{
	CDlgUnits().ToPixels(nLeft, nTop);
	CDlgUnits().ToPixels(nRight, nBottom);

	SetBorders(nLeft, nTop, nRight, nBottom);
}

// static
CRect CRuntimeDlg::OffsetCtrl(CWnd* pParent, UINT nCtrlID, int x, int y)
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
			pCtrl->MoveWindow(rChild);
		}

		return rChild;
	}

	return CRect(0, 0, 0, 0);
}

// static
CRect CRuntimeDlg::ResizeCtrl(CWnd* pParent, UINT nCtrlID, int cx, int cy)
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
				pCtrl->MoveWindow(rChild);
		}

		return rChild;
	}

	return CRect(0, 0, 0, 0);
}

CRect CRuntimeDlg::OffsetCtrl(UINT nCtrlID, int x, int y)
{
	return OffsetCtrl(this, nCtrlID, x, y);
}

CRect CRuntimeDlg::ResizeCtrl(UINT nCtrlID, int cx, int cy)
{
	return ResizeCtrl(this, nCtrlID, cx, cy);
}

int CRuntimeDlg::AddRCControls(const CString& sRCControls)
{
	CRCCtrlParser rccp(sRCControls);

	return rccp.GetRCControls(m_lstControls);
}

BOOL CRuntimeDlg::PreTranslateMessage(MSG* pMsg)
{
	// don't handle the enter/cancel keys in child dialogs
	if (GetStyle() & WS_CHILD)
	{
		if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_CANCEL))
			return FALSE;
	}

	// also filter out what look like accelerator keys 
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam != VK_CONTROL && (GetKeyState(VK_CONTROL) & 0x8000))
		return FALSE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CRuntimeDlg::EnableControls(CWnd* pParent, UINT nCtrlIDFrom, UINT nCtrlIDTo, BOOL bEnable)
{
	for (UINT nID = nCtrlIDFrom; nID <= nCtrlIDTo; nID++)
		::EnableWindow(::GetDlgItem(*pParent, nID), bEnable);
}

void CRuntimeDlg::SetControlsReadOnly(CWnd* pParent, UINT nCtrlIDFrom, UINT nCtrlIDTo, BOOL bReadOnly)
{
	ASSERT (pParent);

	for (UINT nID = nCtrlIDFrom; nID <= nCtrlIDTo; nID++)
		SetControlReadOnly(::GetDlgItem(*pParent, nID), bReadOnly);
}

void CRuntimeDlg::SetControlReadOnly(HWND hCtrl, BOOL bReadOnly)
{
	SetControlState(hCtrl, bReadOnly ? RTCS_READONLY : RTCS_ENABLED);
}

void CRuntimeDlg::SetControlState(CWnd* pParent, UINT nCtrlID, RT_CTRLSTATE nState)
{
	SetControlState(::GetDlgItem(*pParent, nCtrlID), nState);
}

void CRuntimeDlg::SetControlState(HWND hCtrl, RT_CTRLSTATE nState)
{
	if (hCtrl)
	{
		switch (nState)
		{
		case RTCS_ENABLED:
			::EnableWindow(hCtrl, TRUE);

			if (IsEdit(hCtrl))
				::SendMessage(hCtrl, EM_SETREADONLY, FALSE, 0);
			break;

		case RTCS_DISABLED:
			::EnableWindow(hCtrl, FALSE);
			break;

		case RTCS_READONLY:
			if (IsEdit(hCtrl))
			{
				::EnableWindow(hCtrl, TRUE);
				::SendMessage(hCtrl, EM_SETREADONLY, TRUE, 0);
			}
			else // disable
				::EnableWindow(hCtrl, FALSE);
			break;
		}
	}
}

BOOL CRuntimeDlg::IsEdit(HWND hCtrl)
{
	CString sClass = CWinClasses::GetClass(hCtrl);

	return (CWinClasses::IsClass(sClass, WC_EDIT) ||
			CWinClasses::IsClass(sClass, WC_RICHEDIT) ||
			CWinClasses::IsClass(sClass, WC_RICHEDIT20));
}

BOOL CRuntimeDlg::IsEdit(CWnd* pParent, UINT nCtrlID) 
{ 
	ASSERT (pParent);

	return IsEdit(::GetDlgItem(*pParent, nCtrlID)); 
}

void CRuntimeDlg::ShowControls(CWnd* pParent, UINT nCtrlIDFrom, UINT nCtrlIDTo, BOOL bShow)
{
	ASSERT (pParent);

	for (UINT nID = nCtrlIDFrom; nID <= nCtrlIDTo; nID++)
		::ShowWindow(::GetDlgItem(*pParent, nID), bShow ? SW_SHOW : SW_HIDE);
}

void CRuntimeDlg::ExcludeControls(CWnd* pParent, CDC* pDC, UINT nCtrlIDFrom, UINT nCtrlIDTo)
{
	ASSERT (pParent);

	for (UINT nID = nCtrlIDFrom; nID <= nCtrlIDTo; nID++)
	{
		if (::IsWindowVisible(::GetDlgItem(*pParent, nID)))
			pDC->ExcludeClipRect(OffsetCtrl(pParent, nID));
	}
}

void CRuntimeDlg::EnableControls(UINT nCtrlIDFrom, UINT nCtrlIDTo, BOOL bEnable)
{
	EnableControls(this, nCtrlIDFrom, nCtrlIDTo, bEnable);
}

void CRuntimeDlg::ShowControls(UINT nCtrlIDFrom, UINT nCtrlIDTo, BOOL bShow)
{
	ShowControls(this, nCtrlIDFrom, nCtrlIDTo, bShow);
}

void CRuntimeDlg::ExcludeControls(CDC* pDC, UINT nCtrlIDFrom, UINT nCtrlIDTo)
{
	ExcludeControls(this, pDC, nCtrlIDFrom, nCtrlIDTo);
}

void CRuntimeDlg::OnSetFocus(CWnd* pOldWnd) 
{
	CDialog::OnSetFocus(pOldWnd);
	
	CWnd* pChild = GetWindow(GW_CHILD);
	
	if (pChild)
		pChild->SetFocus();
}

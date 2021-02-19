// FontDialog.cpp : implementation file
//

#include "stdafx.h"
#include "style.h"
#include "MinFontDialog.h"
#include "helpid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PRINT_FONT 0x0100
#define TT_FONT	0x0200
#define DEVICE_FONT 0x0400
#define SCREEN_FONT 0x0800
#define FIXED_FONT 0x1000

#define FONT_TYPE_WYSIWYG 1

#define BMW 16
#define BMH 15

static int nFontSizes[] = 
	{8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};
int CLocalComboBox::m_nFontHeight = 0;

class CFontDesc
{
public:
	CFontDesc(LPCTSTR lpszName, LPCTSTR lpszScript, BYTE nCharSet, 
		BYTE nPitchAndFamily, DWORD dwFlags);
	CString m_strName;
	CString m_strScript;
	BYTE m_nCharSet;
	BYTE m_nPitchAndFamily;
	DWORD m_dwFlags;
};

CFontDesc::CFontDesc(LPCTSTR lpszName, LPCTSTR lpszScript, BYTE nCharSet, 
	BYTE nPitchAndFamily, DWORD dwFlags)
{
	m_strName = lpszName;
	m_strScript = lpszScript;
	m_nCharSet = nCharSet;
	m_nPitchAndFamily = nPitchAndFamily;
	m_dwFlags = dwFlags;
}

/////////////////////////////////////////////////////////////////////////////
// CLocalComboBox 

BEGIN_MESSAGE_MAP(CLocalComboBox, CComboBox)
	//{{AFX_MSG_MAP(CLocalComboBox)
	//}}AFX_MSG_MAP
	// Global help commands
END_MESSAGE_MAP()


void CLocalComboBox::GetTheText(CString& str)
{
	int nIndex = GetCurSel();
	if (nIndex == CB_ERR)
		GetWindowText(str);
	else
		GetLBText(nIndex, str);
}

void CLocalComboBox::SetTheText(LPCTSTR lpszText,BOOL bMatchExact)
{
	int idx = (bMatchExact) ? FindStringExact(-1,lpszText) : 
		FindString(-1, lpszText);
	SetCurSel( (idx==CB_ERR) ? -1 : idx);
	if (idx == CB_ERR)
		SetWindowText(lpszText);
}

BOOL CLocalComboBox::LimitText(int nMaxChars)
{
	BOOL b = CComboBox::LimitText(nMaxChars);
	if (b)
		m_nLimitText = nMaxChars;
	return b;
}


BOOL CLocalComboBox::PreTranslateMessage(MSG* pMsg)
{
	return CComboBox::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CFontComboBox 

BEGIN_MESSAGE_MAP(CFontComboBox, CLocalComboBox)
	//{{AFX_MSG_MAP(CFontComboBox)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Global help commands
END_MESSAGE_MAP()

CFontComboBox::CFontComboBox()
{
	VERIFY(m_bmFontType.LoadBitmap(IDB_FONTTYPE));
}

void CFontComboBox::OnDestroy() 
{
	// destroy all the CFontDesc's
	EmptyContents();
	CLocalComboBox::OnDestroy();
}

void CFontComboBox::EmptyContents()
{
	// destroy all the CFontDesc's
	int nCount = GetCount();
	for (int i=0;i<nCount;i++)
		delete (CFontDesc*)GetItemData(i);
}

BOOL CALLBACK AFX_EXPORT EnumFontFamProc(ENUMLOGFONT* lf, NEWTEXTMETRICEX* , int nType, LPVOID pThis)
{
	((CFontComboBox *)pThis)->AddFont(lf, nType);
	return 1;
}

void CFontComboBox::EnumFontFamiliesEx(DWORD dwStyle)
{
	
	CMapStringToPtr map;
	CString str;
	GetTheText(str);

	EmptyContents();
	ResetContent();
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	m_Style = dwStyle;
	lf.lfCharSet = DEFAULT_CHARSET;

	if (dwStyle & PRINT_FONT)
	{
		m_bScreen = FALSE;
		CPrintDialog dlg(FALSE);
		VERIFY(AfxGetApp()->GetPrinterDeviceDefaults(&dlg.m_pd));
		HDC hDC= dlg.CreatePrinterDC();
		if (hDC == NULL)
			AfxMessageBox(IDS_PRINTER_ERROR, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
			//MessageBox("Error: Could not create a printer DC!",(LPTSTR)NULL, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		else
			::EnumFontFamilies(hDC, NULL,(FONTENUMPROC)EnumFontFamProc,(LPARAM)this);
	}
	if (dwStyle & SCREEN_FONT)
	{
		m_bScreen = TRUE;
		CDC* pDC = GetDC();
		ASSERT(pDC != NULL);
		::EnumFontFamilies(pDC->GetSafeHdc(),NULL,(FONTENUMPROC)EnumFontFamProc,(LPARAM)this);
		ReleaseDC(pDC);
	}


	if ((dwStyle & SCREEN_FONT) && (dwStyle & PRINT_FONT))
	{
		for (int i=0; i<m_arrayFontDesc.GetSize(); i++)
		{
			CFontDesc* pDesc = (CFontDesc*)m_arrayFontDesc[i];
			if (!((pDesc->m_dwFlags & SCREEN_FONT) && (pDesc->m_dwFlags & PRINT_FONT)))
			{
				m_arrayFontDesc.RemoveAt(i);
				i --;
			}
		}
	}

	// walk through fonts adding names to string map
	// first time add value 0, after that add value 1
	int nCount = m_arrayFontDesc.GetSize();
	for (int i = 0; i<nCount;i++)
	{
		CFontDesc* pDesc = (CFontDesc*)m_arrayFontDesc[i];
		void* pv = NULL;
		if (map.Lookup(pDesc->m_strName, pv)) // found it
		{
			if (pv == NULL) // only one entry so far
			{
				map.RemoveKey(pDesc->m_strName);
				map.SetAt(pDesc->m_strName, (void*)1);
			}
		}
		else // not found
			map.SetAt(pDesc->m_strName, (void*)0);
	}

	for (i = 0; i<nCount;i++)
	{
		CFontDesc* pDesc = (CFontDesc*)m_arrayFontDesc[i];
		CString str = pDesc->m_strName;
		void* pv = NULL;
		VERIFY(map.Lookup(str, pv));
		if (pv != NULL && !pDesc->m_strScript.IsEmpty())
		{
			str += " (";
			str += pDesc->m_strScript;
			str += ")";
		}

		int nIndex = AddString(str);
		ASSERT(nIndex >=0);
		if (nIndex >=0) //no error
			SetItemData(nIndex, (DWORD)pDesc);
	}

	SetTheText(str);
	m_arrayFontDesc.RemoveAll();
}

void CFontComboBox::AddFont(ENUMLOGFONT* pelf, int nType, LPCTSTR lpszScript)
{
	LOGFONT& lf = pelf->elfLogFont;
	
	// don't put in MAC fonts, commdlg doesn't either
	if (lf.lfCharSet == MAC_CHARSET) return;
	
	// Don't display vertical font for FE platform
	if ((GetSystemMetrics(SM_DBCSENABLED)) && (lf.lfFaceName[0] == '@')) return;

	// don't put in non-printer raster fonts
	if (m_bScreen && (nType & RASTER_FONTTYPE)) return;
	
	if ((m_Style & TT_FONT) && (!(nType & TRUETYPE_FONTTYPE))) return;
	if ((m_Style & FIXED_FONT) && (lf.lfPitchAndFamily & VARIABLE_PITCH)) return;

	DWORD dwData = 0;

	if (nType & TRUETYPE_FONTTYPE) dwData |= TT_FONT;
	else if (nType & DEVICE_FONTTYPE) dwData |= DEVICE_FONT;
	
	if (m_bScreen) dwData |= SCREEN_FONT;
	else dwData |= PRINT_FONT;

	// only put same FaceName once;
	int count = m_arrayFontDesc.GetSize();
	for (int i = 0; i<count; i++)
	{
		CFontDesc* pDesc = (CFontDesc*)m_arrayFontDesc[i];
		if (pDesc->m_strName == lf.lfFaceName)
		{
			pDesc->m_dwFlags |= (SCREEN_FONT | PRINT_FONT);
			return;
		}
	}
	
	CFontDesc* pDesc = new CFontDesc(lf.lfFaceName, lpszScript, 
		lf.lfCharSet, lf.lfPitchAndFamily, dwData);
	m_arrayFontDesc.Add(pDesc);
}


void CFontComboBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS->CtlType == ODT_COMBOBOX);
	int id = (int)(WORD)lpDIS->itemID;

	CDC *pDC = CDC::FromHandle(lpDIS->hDC);
	CRect rc(lpDIS->rcItem);
	if (lpDIS->itemState & ODS_FOCUS)
		pDC->DrawFocusRect(rc);
	int nIndexDC = pDC->SaveDC();

	CBrush brushFill;
	if (lpDIS->itemState & ODS_SELECTED)
	{
		brushFill.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else
		brushFill.CreateSolidBrush(pDC->GetBkColor());
	pDC->SetBkMode(TRANSPARENT);
	pDC->FillRect(rc, &brushFill);

	CFontDesc* pDesc= (CFontDesc*)lpDIS->itemData;
	ASSERT(pDesc != NULL);
	DWORD dwData = pDesc->m_dwFlags;
	if (dwData & (TT_FONT|DEVICE_FONT)) // truetype or device flag set by SetItemData
	{
		CDC dc;
		dc.CreateCompatibleDC(pDC);
		CBitmap* pBitmap = dc.SelectObject(&m_bmFontType);
		if (dwData & TT_FONT)
			pDC->BitBlt(rc.left, rc.top, BMW, BMH, &dc, BMW, 0, SRCAND);
		else // DEVICE_FONT
			pDC->BitBlt(rc.left, rc.top, BMW, BMH, &dc, 0, 0, SRCAND);
		dc.SelectObject(pBitmap);
	}
	
	rc.left += BMW + 6;
	CString strText;
	GetLBText(id, strText);
	pDC->TextOut(rc.left,rc.top,strText,strText.GetLength());

	pDC->RestoreDC(nIndexDC);
}

void CFontComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	ASSERT(lpMIS->CtlType == ODT_COMBOBOX);
	ASSERT(m_nFontHeight > 0);
	CRect rc;
	
	GetWindowRect(&rc);
	lpMIS->itemWidth = rc.Width();
	lpMIS->itemHeight = max(BMH, m_nFontHeight);
}

int CFontComboBox::CompareItem(LPCOMPAREITEMSTRUCT lpCIS)
{
	ASSERT(lpCIS->CtlType == ODT_COMBOBOX);
	int id1 = (int)(WORD)lpCIS->itemID1;
	int id2 = (int)(WORD)lpCIS->itemID2;
	CString str1,str2;
	if (id1 == -1)
		return -1;
	if (id2 == -1)
		return 1;
	GetLBText(id1, str1);
	GetLBText(id2, str2);
	return str1.Collate(str2);
}

// find a font with the face name and charset
void CFontComboBox::MatchFont(LPCTSTR lpszName, BYTE nCharSet)
{
	int nFirstIndex = FindString(-1, lpszName);
	if (nFirstIndex != CB_ERR)
	{
		int nIndex = nFirstIndex;
		do
		{ 
			CFontDesc* pDesc = (CFontDesc*)GetItemData(nIndex);
			ASSERT(pDesc != NULL);
			// check the actual font name to avoid matching Courier western
			// to Courier New western
			if ((nCharSet == DEFAULT_CHARSET || pDesc->m_nCharSet == nCharSet) && 
				lstrcmp(lpszName, pDesc->m_strName)==0)
			{
				//got a match
				if (GetCurSel() != nIndex)
					SetCurSel(nIndex);
				return;
			}
			nIndex = FindString(nIndex, lpszName);
		} while (nIndex != nFirstIndex);
		// loop until found or back to first item again
	}
	//enter font name
	SetTheText(lpszName, TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CSizeComboBox 

CSizeComboBox::CSizeComboBox()
{
	m_nTwipsLast = 0;
}

void CSizeComboBox::EnumFontSizes(CDC& dc, LPCTSTR pFontName)
{
	ResetContent();
	if (pFontName == NULL)
		return;
	if (pFontName[0] == NULL)
		return;
	
	ASSERT(dc.m_hDC != NULL);
	m_nLogVert = dc.GetDeviceCaps(LOGPIXELSY);

	::EnumFontFamilies(dc.m_hDC, pFontName,
		(FONTENUMPROC) EnumSizeCallBack, (LPARAM) this);
}

void CSizeComboBox::TwipsToPointString(LPTSTR lpszBuf, int nTwips)
{
	ASSERT(lpszBuf != NULL);
	lpszBuf[0] = NULL;
	if (nTwips >= 0)
	{
		_stprintf(lpszBuf, _T("%ld"), nTwips);
/*		// round to nearest half point
		nTwips = (nTwips+5)/10;
		if ((nTwips%2) == 0)
			_stprintf(lpszBuf, _T("%ld"), nTwips/2);
		else
			_stprintf(lpszBuf, _T("%.1f"), (float)nTwips/2.F);
*/	}
}

void CSizeComboBox::SetTwipSize(int nTwips)
{
	if (nTwips != GetTwipSize())
	{
		TCHAR buf[10];
		TwipsToPointString(buf, nTwips);
		SetTheText(buf, TRUE);
	}
	m_nTwipsLast = nTwips;
}

int CSizeComboBox::GetTwipSize()
{
	// return values
	// -2 -- error
	// -1 -- edit box empty
	// >=0 -- font size in twips
	CString str;
	GetTheText(str);
	LPCTSTR lpszText = str;

	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	if (lpszText[0] == NULL)
		return -1; // no text in control

	double d = _tcstod(lpszText, (LPTSTR*)&lpszText);
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	if (*lpszText != NULL)
		return -2;   // not terminated properly

//	return (d<0.) ? 0 : (int)(d*20.);
	return (d<0.) ? 0 : (int)d;
}

BOOL CALLBACK AFX_EXPORT CSizeComboBox::EnumSizeCallBack(LOGFONT FAR* /*lplf*/, 
		LPNEWTEXTMETRIC lpntm, int FontType, LPVOID lpv)
{
	CSizeComboBox* pThis = (CSizeComboBox*)lpv;
	ASSERT(pThis != NULL);
	TCHAR buf[10];
	if (
		(FontType & TRUETYPE_FONTTYPE) || 
		!( (FontType & TRUETYPE_FONTTYPE) || (FontType & RASTER_FONTTYPE) )
		) // if truetype or vector font
	{
		// this occurs when there is a truetype and nontruetype version of a font
		if (pThis->GetCount() != 0)
			pThis->ResetContent();
					
		for (int i = 0; i < 16; i++)
		{
			wsprintf(buf, _T("%d"), nFontSizes[i]);
			pThis->AddString(buf);
		}
		return FALSE; // don't call me again
	}
	// calc character height in pixels
	pThis->InsertSize(MulDiv(lpntm->tmHeight-lpntm->tmInternalLeading, 
		72, pThis->m_nLogVert));
	return TRUE; // call me again
}

void CSizeComboBox::InsertSize(int nSize)
{
	ASSERT(nSize > 0);
	DWORD dwSize = (DWORD)nSize;
	TCHAR buf[10];
	TwipsToPointString(buf, nSize);
	if (FindStringExact(-1, buf) == CB_ERR)
	{
		int nIndex = -1;
		int nPos = 0;
		DWORD dw;
		while ((dw = GetItemData(nPos)) != CB_ERR)
		{
			if (dw > dwSize)
			{
				nIndex = nPos;
				break;
			}
			nPos++;
		}
		nIndex = InsertString(nIndex, buf);
		ASSERT(nIndex != CB_ERR);
		if (nIndex != CB_ERR)
			SetItemData(nIndex, dwSize);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMinFontPage property page

const DWORD CMinFontPage::m_nHelpIDs[] = 
{
	IDC_ITEM, HID_FONT_ITEM,
	IDC_TEXT, HID_FONT_TEXT,
	IDC_FONT, HID_FONT_FONT,
	IDC_FONT_SIZE, HID_FONT_SIZE,
	IDC_FONT_STYLE, HID_FONT_STYLE,
	IDC_ROTATE, HID_FONT_ROTATE,
	IDC_VSAMPRECT, HID_FONT_PREVIEW,
	IDC_HSAMPRECT, HID_FONT_PREVIEW,
	IDC_FOREGROUND, HID_FONT_FOREGROUND,
	0, 0
};

CMinFontPage::CMinFontPage() : CCSPropertyPage(CMinFontPage::IDD)
{
	//{{AFX_DATA_INIT(CMinFontPage)
	m_ItemNo = -1;
	m_bRotate = FALSE;
	m_Style = _T("");
	m_Text = _T("");
	//}}AFX_DATA_INIT
	SetModified();
	m_HelpPath += "Plot.hlp";
	m_FontNum = 0;
}

CMinFontPage::~CMinFontPage()
{
	m_FontBox.OnDestroy();
}

void CMinFontPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMinFontPage)
	DDX_Control(pDX, IDC_FOREGROUND, m_ForeGround);
	DDX_CBIndex(pDX, IDC_ITEM, m_ItemNo);
	DDX_Check(pDX, IDC_ROTATE, m_bRotate);
	DDX_CBString(pDX, IDC_FONT_STYLE, m_Style);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMinFontPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CMinFontPage)
	ON_BN_CLICKED(IDC_ROTATE, OnRotate)
	ON_EN_CHANGE(IDC_TEXT, OnChangeText)
	ON_WM_PAINT()
	ON_CBN_KILLFOCUS(IDC_FONT, OnKillfocusFont)
	ON_CBN_KILLFOCUS(IDC_FONT_SIZE, OnKillfocusFontSize)
	ON_CBN_DROPDOWN(IDC_FONT_SIZE, OnDropdownFontSize)
	ON_CBN_CLOSEUP(IDC_FONT, OnComboBoxCloseup)
	ON_CBN_SELCHANGE(IDC_ITEM, OnSelchangeItem)
	ON_CBN_SELCHANGE(IDC_FONT_STYLE, OnSelchangeFontStyle)
	ON_CBN_CLOSEUP(IDC_FONT_SIZE, OnComboBoxCloseup)
	ON_CBN_SELCHANGE(IDC_FOREGROUND, OnSelchangeForeground)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMinFontPage message handlers

void CMinFontPage::OnSelchangeItem() 
{
	int i = m_ItemNo;
	UpdateData(TRUE);
	int j = m_ItemNo;
	m_ItemNo = i;
	TransferData(TRUE);	
	m_ItemNo = j;
	TransferData(FALSE);
	UpdateData(FALSE);
	m_FontBox.MatchFont(lf.lfFaceName, lf.lfCharSet);
	m_FontSizeBox.SetTwipSize(lf.lfHeight);
	Preview();
}

void CMinFontPage::OnSelchangeForeground() 
{
	UpdateData(TRUE);	
	Preview();
}

void CMinFontPage::OnRotate() 
{
	UpdateData(TRUE);	
	GetDlgItem(IDC_HSAMPRECT)->ShowWindow(!m_bRotate);
	GetDlgItem(IDC_VSAMPRECT)->ShowWindow(m_bRotate);
	Preview();
}

void CMinFontPage::OnSelchangeFontStyle() 
{
	CComboBox* pBox =  (CComboBox*) GetDlgItem(IDC_FONT_STYLE);
	if (pBox == NULL) return;
	int i = pBox->GetCurSel();
	if (i != LB_ERR)
	{
		pBox->GetLBText(i,m_Style);
		Preview();
	}
}

void CMinFontPage::OnChangeText() 
{
	UpdateData(TRUE);	
	Preview();
}

void CMinFontPage::OnOK() 
{
	CCSPropertyPage::OnOK();
	TransferData(TRUE);	
	for (int i=0; i<m_FontNum; i++)
	{
		m_PtrList[i]->m_ItemName = m_FontList[i].m_ItemName;
		m_PtrList[i]->m_Text = m_FontList[i].m_Text;
		m_PtrList[i]->lf = m_FontList[i].lf;
		m_PtrList[i]->lf.lfHeight = (int) (m_FontList[i].lf.lfHeight / 0.72 - 0.5);
		m_PtrList[i]->m_BackMode = m_FontList[i].m_BackMode;
		m_PtrList[i]->m_ForeGround = m_FontList[i].m_ForeGround;
		m_PtrList[i]->m_BackGround = m_FontList[i].m_BackGround;
	}
}

BOOL CMinFontPage::OnApply() 
{
	return CCSPropertyPage::OnApply();
}

BOOL CMinFontPage::OnInitDialog() 
{
	VERIFY(m_FontBox.SubclassDlgItem(IDC_FONT, this));
	VERIFY(m_FontSizeBox.SubclassDlgItem(IDC_FONT_SIZE, this));
	if(m_FontNum>0)
	{
		m_ItemNo = 0;
		TransferData(FALSE);
		CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_ITEM);
		for(int j=0; j<m_FontNum; j++) 
			pBox->AddString(m_FontList[j].m_ItemName);
	}
	FillFontBox();
	CCSPropertyPage::OnInitDialog();
	if(m_ItemNo == 0)
	{			
		m_FontBox.MatchFont(lf.lfFaceName, lf.lfCharSet);
		m_FontSizeBox.SetTwipSize(lf.lfHeight);			
	}
	SetModified();
	return TRUE;
}

void CMinFontPage::FillFontBox()
{
	DWORD dwStyle = 0;
	dwStyle |= SCREEN_FONT;
	m_FontBox.EnumFontFamiliesEx(dwStyle);
}

void CMinFontPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	Preview();
}

void CMinFontPage::Preview()
{
	CWnd* pWnd;
	if (m_bRotate) pWnd = GetDlgItem(IDC_VSAMPRECT);
	else pWnd = GetDlgItem(IDC_HSAMPRECT);
	if (pWnd == NULL) return;
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	CRect rect ;
	pWnd->GetClientRect(&rect); 
	CDC* pDC = pWnd->GetDC();
	CFont m_Font;
	LOGFONT m_LogFont;
	GetLogFont(&m_LogFont);
	m_LogFont.lfHeight = (int)(1.0 * m_LogFont.lfHeight * pDC->GetDeviceCaps(LOGPIXELSY) / 72.0 - 0.5);
	m_Font.CreateFontIndirect(&m_LogFont);
	CFont* pOldFont = pDC->SelectObject(&m_Font);
    CSize textsize = pDC->GetTextExtent(m_Text,m_Text.GetLength());   
    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	pDC->IntersectClipRect(rect);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_ForeGround.GetColor());
	if (m_bRotate)
		pDC->TextOut((rect.left + rect.right - textsize.cy) / 2, (rect.top + rect.bottom) / 2, m_Text);
	else
		pDC->TextOut((rect.left + rect.right) / 2, (rect.top + rect.bottom + textsize.cy) / 2, m_Text);
	pDC->SelectObject(pOldFont);
	pWnd->ReleaseDC(pDC);   
}


void CMinFontPage::AddFont(TEXTFONT* pTextFont)
{
	if(m_FontNum <20)
	{
		m_FontList[m_FontNum].m_ItemName = pTextFont->m_ItemName;
		m_FontList[m_FontNum].m_Text = pTextFont->m_Text;
		m_FontList[m_FontNum].lf = pTextFont->lf;
		m_FontList[m_FontNum].m_BackMode = pTextFont->m_BackMode;
		m_FontList[m_FontNum].m_ForeGround = pTextFont->m_ForeGround;
		m_FontList[m_FontNum].m_BackGround = pTextFont->m_BackGround;
		m_FontList[m_FontNum].lf.lfHeight = (int) (m_FontList[m_FontNum].lf.lfHeight * 0.72 - 0.5);
		m_PtrList[m_FontNum] = pTextFont;
		m_FontNum++;
	}
}

void CMinFontPage::SetLogFont(LOGFONT* pLogFont)
{
	memset(&lf,0,sizeof(LOGFONT));

	lf.lfUnderline = pLogFont->lfUnderline;
	strcpy(lf.lfFaceName,pLogFont->lfFaceName);
	lf.lfItalic = pLogFont->lfItalic;
	lf.lfHeight = pLogFont->lfHeight;
	if (lf.lfHeight < 0) lf.lfHeight = -lf.lfHeight;
	lf.lfCharSet = pLogFont->lfCharSet;
	lf.lfPitchAndFamily = pLogFont->lfPitchAndFamily;

	m_bRotate = (pLogFont->lfEscapement == 2700);
	if (m_bRotate) lf.lfEscapement = 2700;
	GetDlgItem(IDC_HSAMPRECT)->ShowWindow(!m_bRotate);
	GetDlgItem(IDC_VSAMPRECT)->ShowWindow(m_bRotate);
	CString szHT;
	if (pLogFont->lfWeight > 550)
	{
		lf.lfWeight = FW_BOLD;
		szHT.LoadString(IDS_ITALICBOLD);
		if (pLogFont->lfItalic)	m_Style = szHT;
		else 
		{
			szHT.LoadString(IDS_BOLD);
			m_Style = szHT;
		}
	}
	else if (pLogFont->lfItalic) 
	{
		szHT.LoadString(IDS_ITALIC);
		m_Style = szHT;
	}
	else 
	{
		lf.lfWeight = FW_NORMAL;
		szHT.LoadString(IDS_NORMAL);
		m_Style = szHT;
	}
}

void CMinFontPage::GetLogFont(LOGFONT* pLogFont)
{
	CString szHT, szHT1, szHT2;
	szHT.LoadString(IDS_BOLD);
	szHT1.LoadString(IDS_ITALICBOLD);
	szHT2.LoadString(IDS_ITALIC);
	memcpy(pLogFont,&lf,sizeof(LOGFONT));
	pLogFont->lfHeight = -pLogFont->lfHeight;
	pLogFont->lfEscapement = m_bRotate ? 2700:0;
	pLogFont->lfWeight = ((m_Style == szHT) || (m_Style == szHT1)) ? FW_BOLD : FW_NORMAL;
	pLogFont->lfItalic = (m_Style == szHT2) || (m_Style == szHT1);
}

void CMinFontPage::TransferData(BOOL bOutput)
{
	if (m_ItemNo < 0) return;
	ASSERT(m_ItemNo < m_FontNum);
	if(bOutput)
	{
		m_FontList[m_ItemNo].m_ForeGround = m_ForeGround.GetColor();
		m_FontList[m_ItemNo].m_Text = m_Text;
		GetLogFont(&m_FontList[m_ItemNo].lf);
	}
	else
	{
		m_ForeGround .SetColor(m_FontList[m_ItemNo].m_ForeGround);
		m_Text = m_FontList[m_ItemNo].m_Text;
		SetLogFont(&m_FontList[m_ItemNo].lf);
	}
}

void CMinFontPage::OnKillfocusFont() 
{
	// this will retrieve the font entered in the edit control
	// it tries to match the font to something already present in the combo box
	// this effectively ignores case of a font the user enters
	// if a user enters arial, this will cause it to become Arial
	CString str;
	m_FontBox.GetTheText(str);	// returns "arial"
	m_FontBox.SetTheText(str);	// selects "Arial"
	m_FontBox.GetTheText(str);	// returns "Arial"

	// if font name box is not empty
	if (str[0] != NULL)
	{
		int nIndex = m_FontBox.FindStringExact(-1, str);
		if (nIndex != CB_ERR)
		{
			CFontDesc* pDesc = (CFontDesc*)m_FontBox.GetItemData(nIndex);
			ASSERT(pDesc != NULL);
			ASSERT(pDesc->m_strName.GetLength() < LF_FACESIZE);
			strncpy(lf.lfFaceName, pDesc->m_strName, LF_FACESIZE);
			lf.lfCharSet = pDesc->m_nCharSet;
			lf.lfPitchAndFamily = pDesc->m_nPitchAndFamily;
		}
		else // unknown font
		{
			ASSERT(str.GetLength() < LF_FACESIZE);
			strncpy(lf.lfFaceName, str, LF_FACESIZE);
			lf.lfCharSet = DEFAULT_CHARSET;
			lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		}
	}
	Preview();
}

void CMinFontPage::OnKillfocusFontSize() 
{
	int nSize = m_FontSizeBox.GetTwipSize();
	if (nSize == -2)
	{
		AfxMessageBox(IDS_NULL_INPUT, MB_OK|MB_ICONINFORMATION);
		nSize = m_FontSizeBox.m_nTwipsLast;
	}
	else if ((nSize >= 0 && nSize < 1) || nSize > 32760)
	{
		AfxMessageBox(IDS_ERROR_DIMENSION, MB_OK|MB_ICONINFORMATION);
		nSize = m_FontSizeBox.m_nTwipsLast;
	}
	else if (nSize > 0)
	{
		lf.lfHeight = nSize;
		Preview();
	}
}

void CMinFontPage::OnDropdownFontSize() 
{
	CString str;
	m_FontBox.GetTheText(str);
	LPCTSTR lpszName = NULL;
	BOOL bPrinterFont;
	int nIndex = m_FontBox.FindStringExact(-1, str);
	if (nIndex != CB_ERR)
	{
		CFontDesc* pDesc = (CFontDesc*)m_FontBox.GetItemData(nIndex);
		ASSERT(pDesc != NULL);
		bPrinterFont = pDesc->m_dwFlags & PRINT_FONT;
		lpszName = pDesc->m_strName;
	}

	int nSize = m_FontSizeBox.GetTwipSize();
	if (nSize == -2)
	{
		AfxMessageBox(IDS_NULL_INPUT, MB_OK|MB_ICONINFORMATION);
		nSize = m_FontSizeBox.m_nTwipsLast;
	}
	else if ((nSize >= 0 && nSize < 1) || nSize > 32760)
	{
		AfxMessageBox(IDS_ERROR_DIMENSION, MB_OK|MB_ICONINFORMATION);
		nSize = m_FontSizeBox.m_nTwipsLast;
	}

	if (bPrinterFont)
	{
		CDC m_dcPrinter;
		VERIFY(AfxGetApp()->CreatePrinterDC(m_dcPrinter));
		m_FontSizeBox.EnumFontSizes(m_dcPrinter, lpszName);
	}
	else
	{
		CDC* pDC = GetDC();
		ASSERT(pDC != NULL);
		m_FontSizeBox.EnumFontSizes(*pDC, lpszName);
		ReleaseDC(pDC);
	}
	m_FontSizeBox.SetTwipSize(nSize);
}


void CMinFontPage::OnComboBoxCloseup() 
{
	SetFocus();
//	Preview();
}


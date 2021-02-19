// SkinWin.cpp: implementation of the CSkinWin class.
//
//		A class to enable draw windows-blind style window
//
//  ToDo:
//		how to popup system menu by my program??
//
//	History:
//		2002.11.24
//				CAN WORK NOW.			
//		2002.11.23	intial version
//
//
//		
//	AUthor:
//		szhao00@mails.tsinghua.edu.cn
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "SkinWin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkinWin::CSkinWin()
{
	m_bInit = FALSE;
	m_winstate = 0;
	m_bActive=TRUE;
	m_barcolor = RGB(207,207,207);
	m_menucolor = RGB(207,207,207);
}

CSkinWin::~CSkinWin()
{

}

CString GetPathName( const char * filename );
CString GetFileName( const char * filename, int ext = 0);
char *next_token( char *buf, char *token, char *stopchars );

COLORREF ReadColor( CString section, CString key, CString file, COLORREF defcolor )
{
	char buf[1000];
	GetPrivateProfileString( section, key, "", buf, 1000, file );
	if ( *buf )
	{
		char token[255];
		char *p = buf;
		int r, g, b;
		p = next_token( p, token, NULL );
		r = atoi(token);
		p = next_token( p, token, NULL );
		g = atoi(token);
		p = next_token( p, token, NULL );
		b = atoi(token);
		return RGB(r, g, b );			
	}
	else
		return defcolor;
}

BOOL CSkinWin::LoadSkin( const char * skinfile )
{
	static const char * ps = "Personality";
	char buf[1000];
	CString path = GetPathName( skinfile );
	
	if ( m_bInit )
	{
		m_bmpDlg.DeleteObject();
		m_bmpTitle.DeleteObject();
		m_bmpLeft.DeleteObject();
		m_bmpRight.DeleteObject();
		m_bmpBottom.DeleteObject();
		m_bmpMaxBtn.DeleteObject();
		m_bmpRestoreBtn.DeleteObject();
		m_bmpMinBtn.DeleteObject();
		m_bmpCloseBtn.DeleteObject();

		//set deafult
		m_barcolor = RGB(207,207,207);
		m_menucolor = RGB(207,207,207);

		m_bInit = FALSE;
	}

	GetPrivateProfileString( ps, "DialogBmp", "", buf, 1000, skinfile );
	if ( *buf != 0 )
		m_bmpDlg.LoadBitmap( path + "/" + GetFileName( buf,1 ));
		

	GetPrivateProfileString( ps, "Top", "", buf, 1000, skinfile );
	if ( *buf == 0 || !m_bmpTitle.LoadBitmap( path + "/" + GetFileName( buf,1 )) )
		return FALSE;
	GetPrivateProfileString( ps, "Left", "", buf, 1000, skinfile );
	if ( *buf == 0 || !m_bmpLeft.LoadBitmap( path + "/" + GetFileName( buf,1 ) )) 
		return FALSE;
	GetPrivateProfileString( ps, "Right", "", buf, 1000, skinfile );
	if ( *buf == 0 || !m_bmpRight.LoadBitmap( path + "/" + GetFileName( buf,1 ) )) 
		return FALSE;
	GetPrivateProfileString( ps, "Bottom", "", buf, 1000, skinfile );
	if ( *buf == 0 || !m_bmpBottom.LoadBitmap( path + "/" + GetFileName( buf,1 ) )) 
		return FALSE;

	m_TitleHeight = m_bmpTitle.Height()/2;
	m_BorderLeftWidth = m_bmpLeft.Width()/2;
	m_BorderRightWidth = m_bmpRight.Width()/2;
	m_BorderBottomHeight = m_bmpBottom.Height()/2;

	m_titleoff1 = GetPrivateProfileInt( ps, "TopTopHeight", 0, skinfile );
	m_titleoff2 = m_bmpTitle.Width() - GetPrivateProfileInt( ps, "TopBotHeight", 0, skinfile );
	if ( m_titleoff2 <= m_titleoff1 )
		m_titleoff2 = m_titleoff1 + 1;
	m_leftoff1 = GetPrivateProfileInt( ps, "LeftTopHeight", 0, skinfile );
	m_leftoff2 = m_bmpLeft.Height() - GetPrivateProfileInt( ps, "LeftBotHeight", 0, skinfile );
	if ( m_leftoff2 <= m_leftoff1 )
		m_leftoff2 = m_leftoff1 + 1;

	m_rightoff1 = GetPrivateProfileInt( ps, "RightTopHeight", 0, skinfile );
	m_rightoff2 = m_bmpRight.Height() - GetPrivateProfileInt( ps, "RightBotHeight", 0, skinfile );
	if ( m_rightoff2 <= m_rightoff1 )
		m_rightoff2 = m_rightoff1 + 1;

	m_bottomoff1 = GetPrivateProfileInt( ps, "BottomTopHeight", 0, skinfile );
	m_bottomoff2 = m_bmpBottom.Width() - GetPrivateProfileInt( ps, "BottomBotHeight", 0, skinfile );
	if ( m_bottomoff2 <= m_bottomoff1 )
		m_bottomoff2 = m_bottomoff1 + 1;

	//load buttons
	int count = GetPrivateProfileInt( ps, "ButtonCount", 0, skinfile );
	int icount = GetPrivateProfileInt( ps, "ButtonImgCount", 3, skinfile );
	for ( int i = 0; i < count; i++ )
	{
		CString sec;
		sec.Format( "Button%d", i );
		GetPrivateProfileString( sec, "ButtonImage", "", buf, 1000, skinfile );
		int action = GetPrivateProfileInt( sec, "Action", 0,  skinfile );
		int x = GetPrivateProfileInt( sec, "XCoord", 0, skinfile );
		int y = GetPrivateProfileInt( sec, "YCoord", 0, skinfile );

		int state = icount;
		if ( action == 0 )
		{
			//close
			if ( !m_bmpCloseBtn.LoadBitmap( path + "/" + GetFileName( buf,1 )) ) 
				return FALSE;
			//state = m_bmpCloseBtn.Width()/m_bmpCloseBtn.Height();
			m_rectCloseBtn = CRect( x-m_bmpCloseBtn.Width()/state, y, x , y + m_bmpCloseBtn.Height() );
		}
		if ( action == 2 )
		{
			//min
			if ( !m_bmpMinBtn.LoadBitmap( path + "/" + GetFileName( buf,1 )) ) 
				return FALSE;
			//state = m_bmpMinBtn.Width()/m_bmpMinBtn.Height();
			m_rectMinBtn = CRect( x-m_bmpMinBtn.Width()/state, y, x, y + m_bmpMinBtn.Height() );
		}
		if ( action == 1 )
		{			
			if ( !m_bmpMaxBtn.GetSafeHandle() )
			{
				//max
				if ( !m_bmpMaxBtn.LoadBitmap( path + "/" + GetFileName( buf,1 )) ) 
					return FALSE;
				//state = m_bmpMaxBtn.Width()/m_bmpMaxBtn.Height();
				m_rectMaxBtn = CRect( x-m_bmpMaxBtn.Width()/state, y, x , y + m_bmpMaxBtn.Height() );
			}
			else
			{
				//restore
				if ( !m_bmpRestoreBtn.LoadBitmap( path + "/" + GetFileName( buf,1 )) ) 
					return FALSE;
				//state = m_bmpRestoreBtn.Width()/m_bmpRestoreBtn.Height();
				m_rectRestoreBtn = CRect( x-m_bmpRestoreBtn.Width()/state, y, x , y + m_bmpRestoreBtn.Height() );
			}
		}
		if ( action == 4 )
		{
		}
	}
	m_textShift = GetPrivateProfileInt( ps, "TextShift", 0, skinfile );
	m_textShiftVer = GetPrivateProfileInt( ps, "TextShiftVert", 0, skinfile );

	char * colours = "Colours";
	m_colTitle1 = ReadColor(colours, "TitleText", skinfile, GetSysColor(COLOR_CAPTIONTEXT) );
	m_colTitle2 = ReadColor(colours, "InactiveTitleText", skinfile, GetSysColor(COLOR_CAPTIONTEXT) );//
//	0xE8A66A;//0x00FFFF;//
	m_bTrans = GetPrivateProfileInt( ps, "UsesTran", 0, skinfile );
	if ( m_bTrans )
		m_colTrans = ReadColor(colours, "TransColor", skinfile, RGB(255,0,255) );


	m_barcolor = ReadColor(colours, "BarColor", skinfile, RGB(207,207,207) );
	m_menucolor = ReadColor(colours, "MenuColor", skinfile, RGB(207,207,207) );

	m_btnbgcolor = ReadColor(colours, "ButtonFace", skinfile, GetSysColor(COLOR_BTNFACE) );
	m_btntextcolor = ReadColor(colours, "ButtonText", skinfile, GetSysColor(COLOR_BTNTEXT));
	m_btnhovercolor = ReadColor(colours, "ButtonHilight", skinfile, GetSysColor(COLOR_BTNFACE) );
	m_btnfocuscolor = ReadColor(colours, "ButtonFocus", skinfile, GetSysColor(COLOR_BTNFACE) );
	
	m_bInit = TRUE;
	return TRUE;
}

BOOL CSkinWin::DrawTitle(CDC *pDC, int x, int y, int w, int state)
{
	int padding; 
	int ox = x;
	padding = ( w - m_bmpTitle.Width() )/( m_titleoff2 - m_titleoff1 ) + 1 ;
	if ( padding < 0 ) padding = 0;

	RECT sr;
	if ( state == 0 )
		sr = CRect( 0, 0, m_titleoff1, m_TitleHeight );
	else
		sr = CRect( 0, m_TitleHeight, m_titleoff1, m_bmpTitle.Height()  );	
	m_bmpTitle.Draw( pDC, x, y, &sr );
	 
	x += m_titleoff1;
	if ( state == 0 )
		sr = CRect(  m_titleoff1, 0, m_titleoff2, m_TitleHeight );
	else
		sr = CRect(  m_titleoff1, m_TitleHeight, m_titleoff2, m_bmpTitle.Height()  );	

	for ( int i = 0; i <= padding; i++, x += m_titleoff2 - m_titleoff1 )
	{
		int d = ( x + m_titleoff2 - m_titleoff1 - ox - w);
		if ( d > 0 )
			sr.right = sr.right - d;
		m_bmpTitle.Draw( pDC, x, y, &sr );
	}

	x = ox + w - ( m_bmpTitle.Width() - m_titleoff2 ) + 1 ;
	if ( state == 0 )
		sr = CRect(  m_titleoff2, 0, m_bmpTitle.Width()-1, m_TitleHeight);
	else
		sr = CRect(  m_titleoff2, m_TitleHeight, m_bmpTitle.Width()-1, m_bmpTitle.Height()  );	
	m_bmpTitle.Draw( pDC, x, y, &sr );
	return TRUE;
}

BOOL CSkinWin::DrawBottom(CDC *pDC, int x, int y, int w, int state)
{
	int padding; 
	int ox = x;
	padding = ( w - m_bmpBottom.Width() )/( m_bottomoff2 - m_bottomoff1 ) + 1 ;
	if ( padding < 0 ) padding = 0;

	RECT sr;
	if ( state == 0 )
		sr = CRect( 0, 0, m_bottomoff1, m_BorderBottomHeight);
	else
		sr = CRect( 0, m_BorderBottomHeight, m_bottomoff1, m_bmpBottom.Height()  );	
	m_bmpBottom.Draw( pDC, x, y, &sr );
	
	x += m_bottomoff1;
	if ( state == 0 )
		sr = CRect(  m_bottomoff1, 0, m_bottomoff2, m_BorderBottomHeight );
	else
		sr = CRect(  m_bottomoff1, m_BorderBottomHeight, m_bottomoff2, m_bmpBottom.Height() );	

	for ( int i = 0; i <= padding; i++, x += m_bottomoff2 - m_bottomoff1 )
	{
		int d = ( x + m_bottomoff2 - m_bottomoff1 - ox - w);
		if ( d > 0 )
			sr.right = sr.right - d;
		m_bmpBottom.Draw( pDC, x, y, &sr );
	}

	x = ox + w - ( m_bmpBottom.Width() - m_bottomoff2 );
	if ( state == 0 )
		sr = CRect(  m_bottomoff2, 0, m_bmpBottom.Width()-1, m_BorderBottomHeight );
	else
		sr = CRect(  m_bottomoff2, m_BorderBottomHeight, m_bmpBottom.Width()-1, m_bmpBottom.Height()  );	
	m_bmpBottom.Draw( pDC, x, y, &sr );
	return TRUE;
}

BOOL CSkinWin::DrawLeft(CDC *pDC, int x, int y, int h, int state)
{
	int padding; 
	int oy = y;
	padding = ( h - m_bmpLeft.Height() )/( m_leftoff2 - m_leftoff1 ) + 1 ;
	if ( padding < 0 ) padding = 0;

	RECT sr;
	if ( state == 0 )
		sr = CRect( 0, 0, m_BorderLeftWidth, m_leftoff1 );
	else
		sr = CRect( m_BorderLeftWidth, 0, m_bmpLeft.Width(), m_leftoff1  );	
	m_bmpLeft.Draw( pDC, x, y, &sr );
	
	y += m_leftoff1;
	if ( state == 0 )
		sr = CRect(  0, m_leftoff1,  m_BorderLeftWidth, m_leftoff2 );
	else
		sr = CRect(  m_BorderLeftWidth, m_leftoff1, m_bmpLeft.Width(), m_leftoff2 );	

	for ( int i = 0; i <= padding; i++, y += m_leftoff2 - m_leftoff1 )
	{
		int d = ( y + m_leftoff2 - m_leftoff1 - oy - h);
		if ( d > 0 )
			sr.bottom = sr.bottom - d;
		m_bmpLeft.Draw( pDC, x, y, &sr );
	}

	y = oy + h - ( m_bmpLeft.Height() - m_leftoff2 ) ;
	if ( state == 0 )
		sr = CRect(  0, m_leftoff2, m_BorderLeftWidth, m_bmpLeft.Height());
	else
		sr = CRect(  m_BorderLeftWidth, m_leftoff2,  m_bmpLeft.Width(), m_bmpLeft.Height()  );	
	m_bmpLeft.Draw( pDC, x, y, &sr );

	return TRUE;
}

BOOL CSkinWin::DrawRight(CDC *pDC, int x, int y, int h, int state)
{
	int padding; 
	int oy = y;
	padding = ( h - m_bmpRight.Height() )/( m_rightoff2 - m_rightoff1 ) + 1 ;
	if ( padding < 0 ) padding = 0;

	RECT sr;
	if ( state == 0 )
		sr = CRect( 0, 0, m_BorderRightWidth, m_rightoff1 );
	else
		sr = CRect( m_BorderRightWidth, 0, m_bmpRight.Width(), m_rightoff1  );	
	m_bmpRight.Draw( pDC, x, y, &sr );
	
	y += m_rightoff1;
	if ( state == 0 )
		sr = CRect(  0, m_rightoff1,  m_BorderRightWidth, m_rightoff2 );
	else
		sr = CRect(  m_BorderRightWidth, m_rightoff1, m_bmpRight.Width(), m_rightoff2 );	

	for ( int i = 0; i <= padding; i++, y += m_rightoff2 - m_rightoff1 )
	{
		int d = ( y + m_rightoff2 - m_rightoff1 - oy - h);
		if ( d > 0 )
			sr.bottom = sr.bottom - d;
		m_bmpRight.Draw( pDC, x, y, &sr );
	}

	y = oy + h - ( m_bmpRight.Height() - m_rightoff2 ) ;
	if ( state == 0 )
		sr = CRect(  0, m_rightoff2, m_BorderRightWidth, m_bmpRight.Height());
	else
		sr = CRect(  m_BorderRightWidth, m_rightoff2,  m_bmpRight.Width(), m_bmpRight.Height()  );	
	m_bmpRight.Draw( pDC, x, y, &sr );

	return TRUE;

}

BOOL CSkinWin::DrawFrame(CDC *pDC, int x, int y, int w, int h, int state, int title )
{
	if ( title )
		DrawTitle( pDC, x + m_BorderLeftWidth , y, 
   			w - m_BorderRightWidth - m_BorderLeftWidth + 1, state );
	DrawLeft( pDC, x, y, h, state );
	DrawRight( pDC, x + w - m_BorderRightWidth , y, h, state );
	DrawBottom( pDC, x + m_BorderLeftWidth, 
		y + h - m_BorderBottomHeight, w - m_BorderRightWidth - m_BorderLeftWidth, state );
	return TRUE;

}

BOOL CSkinWin::InstallSkin(CWnd *wnd)
{
	if ( !wnd  || !m_bInit ) return FALSE;
	HookWindow( (HWND)NULL);
	int r = HookWindow( wnd);

	DWORD style = GetWindowLong( m_hWnd, GWL_STYLE );
	m_sizable = style & WS_SIZEBOX;
	m_minable = style & WS_MINIMIZEBOX;
	m_maxable = style & WS_MAXIMIZEBOX;
	m_sysmenu = style & WS_MAXIMIZEBOX;
	style &= ~WS_SIZEBOX;
	style &= ~WS_MINIMIZEBOX;
	style &= ~WS_MAXIMIZEBOX;
	style &= ~WS_SYSMENU;
	SetWindowLong( m_hWnd, GWL_STYLE, style );

	return r;
}

LRESULT CSkinWin::WindowProc(UINT msg, WPARAM wp, LPARAM lp)
{
	if ( !IsWindow(m_hWnd) )
		return 0;

	if ( !m_bInit  ) 
		return Default();

	switch ( msg )
	{
	case WM_SHOWWINDOW:
		if ( wp )
			SetWindowPos( m_hWnd, 0, 0, 0, 400, 400, SWP_NOSIZE|SWP_NOMOVE|SWP_FRAMECHANGED );
		Default();
		return 0;
		break;
	case WM_INITMENUPOPUP:
		Default();
		return 0;
		
	case WM_INITMENU:
		//a hack, when popup sysmenu, redraw title bar
		HMENU h;
		h = (HMENU)wp;
		int c;
		h = GetSubMenu(h, 0);
		c = GetMenuItemID( h, 0 );
		if ( c == SC_RESTORE )
		{
			CRect r;
			GetClientRect( m_hWnd, r );
			OnNcPaint(0);
			r.bottom = GetSystemMetrics(SM_CYCAPTION);
			InvalidateRect( m_hWnd, r, TRUE );
			UpdateWindow( m_hWnd );
		}
		Default();
		return 0;
		
	case WM_SYSCOMMAND:
		OnSysCommand( wp, lp );
		//Default();
		return 0;
	case WM_SETTEXT:
		return OnSetText( wp, lp );
	case WM_NCPAINT:
		OnNcPaint( (HRGN)wp );
		return 0;
	case WM_NCCALCSIZE:
		OnNcCalcSize( (BOOL)wp, (NCCALCSIZE_PARAMS *)lp );
		return 0;
	case WM_SIZE:
		OnSize( wp, LOWORD(lp), HIWORD(lp) );
		return 0;	
	case WM_NCACTIVATE:
		return OnNcActivate( (BOOL)wp );
	case WM_NCHITTEST:
		return OnNcHitTest(CPoint(LOWORD(lp), HIWORD(lp)));
	case WM_NCLBUTTONUP:
		OnNcLButtonUp(wp, CPoint(LOWORD(lp), HIWORD(lp)));
		return 0;
	case WM_NCLBUTTONDOWN:
		OnNcLButtonDown(wp, CPoint(LOWORD(lp), HIWORD(lp)));
		return 0;
	case WM_NCLBUTTONDBLCLK:
		OnNcLButtonDblClk(wp, CPoint(LOWORD(lp), HIWORD(lp)));
		return 0;
	case WM_NCRBUTTONUP:
		OnNcRButtonUp(wp, CPoint(LOWORD(lp), HIWORD(lp)));
		return 0;
	case WM_NCRBUTTONDOWN:
		OnNcRButtonDown(wp, CPoint(LOWORD(lp), HIWORD(lp)));
		return 0;
	case WM_NCMOUSEMOVE:
		OnNcMouseMove( wp,CPoint(LOWORD(lp), HIWORD(lp)));
		return 0;
	case WM_GETMINMAXINFO:
		OnGetMinMaxInfo( (MINMAXINFO *)lp );
		return 0;
	case WM_WINDOWPOSCHANGING:
		OnWindowPosChanging((WINDOWPOS *)lp);
		return 0;
	case WM_SIZING:
		OnSizing( wp, (LPRECT)lp );
		return 0;
	case WM_ACTIVATE:
		OnActivate( wp, CWnd::FromHandle((HWND)lp), TRUE);
		return 0;
	case WM_COMMAND:
		if ( !HandleSysCommand( wp, lp ) )
			Default();
		return 0;
	default:
		return Default();
	}
}

void CSkinWin::OnNcPaint(HRGN rgn1)
{	
	CWnd *pWnd = CWnd::FromHandle(m_hWnd);
	CDC * pDC = pWnd->GetWindowDC();
	CRect wr;
	pWnd->GetWindowRect( wr );
	int state = 0;
	if ( m_bActive)
		state = 0;
	else state = 1;
	
	pDC->ExcludeClipRect(0, 0, wr.Width(),	m_TitleHeight );
	DrawFrame( pDC, 0, 0, wr.Width(), wr.Height(), state, 0 );	
	pDC->SelectClipRgn( NULL ); 

	CDC memDC, *pNewDC;
	CMyBitmap bmp;
	CBitmap  *obmp;
	memDC.CreateCompatibleDC( pDC );
	bmp.CreateCompatibleBitmap( pDC, wr.Width(), m_TitleHeight );
	obmp = memDC.SelectObject(&bmp);
	pNewDC = &memDC;
	
	DrawTitle( pNewDC, m_BorderLeftWidth , 0, 
   			wr.Width() - m_BorderRightWidth - m_BorderLeftWidth + 1, state );
	DrawLeft( pNewDC, 0, 0, m_bmpLeft.Height(), state );
	DrawRight( pNewDC, wr.Width() - m_BorderRightWidth , 0, m_bmpRight.Height(), state );

	
	CRgn newrgn;
	newrgn.CreateRectRgn( 0, 0, wr.Width(), wr.Height() );
	if ( m_bTrans )
	{
		CRgn rgn;
		rgn.CreateRectRgn( 0, m_TitleHeight, wr.Width(), wr.Height() );
		HRGN hrgn = bmp.CreateRgnFromFile( m_colTrans );
		newrgn.CombineRgn( &rgn, CRgn::FromHandle(hrgn), RGN_XOR  );
		pDC->SelectClipRgn( &newrgn ); 
	}
	else
		SetWindowRgn( m_hWnd, newrgn, FALSE );

	if ( m_downHitTest == HTCLOSE )
		DrawButton( pNewDC, 0, 1 );
	else if ( m_moveHitTest == HTCLOSE)
		DrawButton( pNewDC, 0, 2 );
	else
		DrawButton( pNewDC, 0, 0 );
	
	if ( m_downHitTest == HTMINBUTTON )
		DrawButton( pNewDC, 2, 1 );
	else if ( m_moveHitTest == HTMINBUTTON)
		DrawButton( pNewDC, 2, 2 );
	else
		DrawButton( pNewDC, 2, 0 );	

	if ( m_downHitTest == HTMAXBUTTON )
		DrawButton( pNewDC, 1, 1 );
	else if ( m_moveHitTest == HTMAXBUTTON)
		DrawButton( pNewDC, 1, 2 );
	else
		DrawButton( pNewDC, 1, 0 );		

	int cx = GetSystemMetrics(SM_CXSMICON);
	int cy = GetSystemMetrics(SM_CYSMICON);
	HICON hi = (HICON)SendMessage( m_hWnd, WM_GETICON, ICON_SMALL, 0);
	if ( !hi )
	{
#ifdef IDR_MAINFRAME
		hi = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
#endif
	}
	//draw icon
	::DrawIconEx( pNewDC->GetSafeHdc(), m_BorderLeftWidth, 5, (HICON)
		CopyImage( hi, IMAGE_ICON, 
		cx, cy, 0), cx, cy, 0, 0, DI_NORMAL);
	//draw text
	if ( m_title.IsEmpty() )
		pWnd->GetWindowText(m_title);

	if (m_bActive)
		pNewDC->SetTextColor( m_colTitle1 );
	else
		pNewDC->SetTextColor( m_colTitle2 );

	CFont font, *ofont;
	font.CreatePointFont( GetSystemMetrics(SM_CYSMCAPTION), "System" );//
	ofont = pNewDC->SelectObject(&font);

	pNewDC->SetBkMode(TRANSPARENT);
	pNewDC->DrawText( m_title, CRect( m_textShift+40, m_textShiftVer, wr.Width() - m_bmpTitle.Width() + m_titleoff2, 
		m_TitleHeight-4 ), DT_SINGLELINE | DT_LEFT  | DT_VCENTER | DT_WORD_ELLIPSIS  );

	pNewDC->SelectObject(&font);

	pDC->BitBlt( 0, 0, wr.Width(),
		m_TitleHeight, pNewDC, 0, 0, SRCCOPY );
	memDC.SelectObject(obmp);

	pDC->SelectClipRgn(NULL);
}

void CSkinWin::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpncsp)
{
	if ( bCalcValidRects )
	{
		lpncsp->rgrc[0].left = lpncsp->rgrc[0].left + m_BorderLeftWidth;
		lpncsp->rgrc[0].right = lpncsp->rgrc[0].right - m_BorderRightWidth;
		lpncsp->rgrc[0].top = lpncsp->rgrc[0].top + m_TitleHeight;
		lpncsp->rgrc[0].bottom = lpncsp->rgrc[0].bottom - m_BorderBottomHeight;
		lpncsp->rgrc[1] = lpncsp->rgrc[0];
	}
	else
		Default();
}

void CSkinWin::OnSizing(UINT nSide, LPRECT lpRect)
{
	CWnd *pWnd = CWnd::FromHandle(m_hWnd);
	CRect wr(lpRect);
}


void CSkinWin::OnSize(UINT nType, int cx, int cy)
{
	//Default();
	CWnd *pWnd = CWnd::FromHandle(m_hWnd);
	CRect wr;	
	Default();
	pWnd->GetWindowRect(wr);
	pWnd->Invalidate();
	OnNcPaint(0);
	
	if ( m_bTrans )
		SetWindowRgn( m_hWnd, GetRegion(wr.Width(), wr.Height() ), TRUE );
	else
		SetWindowRgn( m_hWnd, NULL, TRUE );


}

HRGN CSkinWin::GetRegion(int w, int h)
{
	CWnd *pWnd = CWnd::FromHandle(m_hWnd);
	CRect wr;
	pWnd->GetWindowRect(wr);
	
	CRgn rgn;
	if ( m_bTrans )
	{
		CDC *pDC = pWnd->GetDC();
		CDC memDC;
		CMyBitmap bmp;
		CBitmap *obmp;
		memDC.CreateCompatibleDC(pDC);
		bmp.CreateCompatibleBitmap( pDC, w, m_TitleHeight );
		obmp = memDC.SelectObject(&bmp);

		DrawTitle( &memDC, m_BorderLeftWidth , 0, 
   				wr.Width() - m_BorderRightWidth - m_BorderLeftWidth + 1, 0 );
		DrawLeft( &memDC, 0, 0, m_bmpLeft.Height(), 0 );
		DrawRight( &memDC, wr.Width() - m_BorderRightWidth , 0, m_bmpRight.Height(), 0 );
		
		memDC.SelectObject(obmp);
		pWnd->ReleaseDC( pDC );

		rgn.CreateRectRgn( 0, m_TitleHeight, wr.Width(), wr.Height() );
		HRGN hrgn = bmp.CreateRgnFromFile( m_colTrans );
		CRgn newrgn;
		newrgn.CreateRectRgn( 0, m_TitleHeight, wr.Width(), wr.Height() );
		newrgn.CombineRgn( &rgn, CRgn::FromHandle(hrgn), RGN_XOR  );
		
		return (HRGN)newrgn.Detach();
	}
	else
		rgn.CreateRectRgn( 0, 0, wr.Width(), wr.Height() );
		
	
	return (HRGN)rgn.Detach();
}


BOOL CSkinWin::OnNcActivate(BOOL bActive)
{
	m_bActive = bActive;
	OnNcPaint(0);
	if ( !bActive )
		return 1;
	else
		return 0;
}

UINT CSkinWin::OnNcHitTest(CPoint point)
{
	CWnd *pWnd = CWnd::FromHandle(m_hWnd);
	CRect wr;
	pWnd->GetWindowRect(wr);
	
	point.x -= wr.left;
	point.y -= wr.top;
	if ( PtInRect( GetButtonRect(0), point ) )
		return HTCLOSE;
	if ( PtInRect( GetButtonRect(2), point ) && m_minable )
		return HTMINBUTTON;
	if ( PtInRect( GetButtonRect(1), point ) && m_maxable )
		return HTMAXBUTTON;

	CRect r;
	int cx = GetSystemMetrics(SM_CXSMICON);
	int cy = GetSystemMetrics(SM_CYSMICON);
	if ( PtInRect( CRect(m_BorderLeftWidth, 5, m_BorderLeftWidth + cx, cy + 5), point ) )
		return HTSYSMENU;

	r = CRect( 0, 0, m_BorderLeftWidth, m_TitleHeight );
	if ( PtInRect( r, point ) && m_sizable && m_winstate != 1 )  //!IsZoomed(m_hWnd) )
		return HTTOPLEFT;
	r = CRect( wr.Width() - m_BorderLeftWidth, 0,  wr.Width(), m_TitleHeight  );
	if ( PtInRect( r, point ) && m_sizable && m_winstate != 1 )  //!IsZoomed(m_hWnd) )
		return HTTOPRIGHT;
	r = CRect( 0, wr.Height() - m_BorderBottomHeight, m_BorderLeftWidth, wr.Height() );
	if ( PtInRect( r, point ) && m_sizable && m_winstate != 1 )  //!IsZoomed(m_hWnd) )
		return HTBOTTOMLEFT;
	r = CRect( wr.Width()-m_BorderRightWidth, wr.Height() - m_BorderBottomHeight,  wr.Width(), wr.Height() );
	if ( PtInRect( r, point ) && m_sizable && m_winstate != 1 )  //!IsZoomed(m_hWnd) )
		return HTBOTTOMRIGHT;
	r = CRect( 0, m_TitleHeight,  m_BorderLeftWidth, wr.Height() - m_BorderBottomHeight  );
	if ( PtInRect( r, point ) && m_sizable && m_winstate != 1 )  //!IsZoomed(m_hWnd) )
		return HTLEFT;
	r = CRect( wr.Width()-m_BorderRightWidth, m_TitleHeight,  wr.Width(), wr.Height() - m_BorderBottomHeight  );
	if ( PtInRect( r, point ) && m_sizable && m_winstate != 1 )  //!IsZoomed(m_hWnd) )
		return HTRIGHT;
	r = CRect( m_BorderLeftWidth, wr.Height() - m_BorderBottomHeight,  wr.Width()-m_BorderRightWidth, wr.Height() );
	if ( PtInRect( r, point ) && m_sizable && m_winstate != 1 )  //!IsZoomed(m_hWnd) )
		return HTBOTTOM;
	r = CRect( m_BorderLeftWidth, 0,  wr.Width()-m_BorderRightWidth, m_BorderBottomHeight );
	if ( PtInRect( r, point ) && m_sizable && m_winstate != 1 )  //!IsZoomed(m_hWnd) )
		return HTTOP;
	//set to boder 5
	r = CRect( m_BorderLeftWidth, 5 , wr.Width()-m_BorderRightWidth, m_TitleHeight );
	if ( PtInRect( r, point ) )
		return HTCAPTION;
	return HTCLIENT;
}

void CSkinWin::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	if ( nHitTest == HTCAPTION && m_sizable )
	{
		if ( m_winstate == 1 )
			Restore();
		else
			Maximize(); 
		m_downHitTest = 0;
		m_moveHitTest = 0;
		OnNcPaint(0);
	}
}

void CSkinWin::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	m_downHitTest = nHitTest;
	m_moveHitTest = m_downHitTest;

	OnNcPaint(0);	
	if ( nHitTest >= HTLEFT && nHitTest <= HTBOTTOMRIGHT || 
		nHitTest == HTCAPTION && m_winstate != 1 )  //!IsZoomed(m_hWnd) )
		Default();
	else if ( nHitTest == HTSYSMENU )
		PopupSysMenu(point);
}

void CSkinWin::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	if ( nHitTest == HTCLOSE )
	{
		SendMessage(m_hWnd, WM_CLOSE, 0, 0);
		return;
	}
	else if ( nHitTest == HTMINBUTTON )
		Minimize();
	else if ( nHitTest == HTMAXBUTTON )
	{
		if ( m_winstate == 1 ) // IsZoomed(m_hWnd) )
			Restore(); //ShowWindow(m_hWnd, SW_RESTORE);
		else
			Maximize(); //ShowWindow(m_hWnd, SW_MAXIMIZE);
	}
	else
		return;
	m_downHitTest = 0;
	m_moveHitTest = 0;
	OnNcPaint(0);

}

void CSkinWin::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if ( nHitTest >= HTLEFT && nHitTest <= HTBOTTOMRIGHT || 
		 nHitTest == HTCAPTION && m_winstate != 1 )  //!IsZoomed(m_hWnd) )
		Default();
	
	m_moveHitTest = nHitTest;
	m_downHitTest = 0;
	if ( m_oldHitTest != nHitTest )
	{
		OnNcPaint(0);
		m_oldHitTest = nHitTest;
	}	
}

void CSkinWin::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	if ( nHitTest == HTCAPTION )
	{
		
		CWnd * pWnd = CWnd::FromHandle(m_hWnd);
		pWnd->ModifyStyle( 0, WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX );			
		Default();
		pWnd->ModifyStyle( WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 0 );			
		
		PopupSysMenu(point);
	}
}

void CSkinWin::OnNcRButtonUp(UINT nHitTest, CPoint point)
{


}

CRect CSkinWin::GetButtonRect(int i)
{
	CWnd *pWnd = CWnd::FromHandle(m_hWnd);
	CRect wr;
	pWnd->GetWindowRect(wr);

	CRect r;
	if ( i == 0 && m_bmpCloseBtn.GetSafeHandle() )
	{
		//close
		r = m_rectCloseBtn;
		r.left = wr.Width() - m_rectCloseBtn.right;
		r.right = wr.Width() - m_rectCloseBtn.left;
	}
	if ( i == 1 && m_bmpMaxBtn.GetSafeHandle())
	{
		//max
		
		if ( m_winstate != 1 || !m_bmpRestoreBtn.GetSafeHandle() )
		{
			r = m_rectMaxBtn;
			r.left = wr.Width() - m_rectMaxBtn.right;
			r.right = wr.Width() - m_rectMaxBtn.left;
		}
		else
		{
			r = m_rectRestoreBtn;
			r.left = wr.Width() - m_rectRestoreBtn.right;
			r.right = wr.Width() - m_rectRestoreBtn.left;
		}
			
	}
	if ( i == 2 && m_bmpMinBtn.GetSafeHandle())
	{
		//min
		r = m_rectMinBtn;
		r.left = wr.Width() - m_rectMinBtn.right;
		r.right = wr.Width() - m_rectMinBtn.left;
	}

	return r;

}

BOOL CSkinWin::DrawButton( CDC * pDC, int i, int state)
{
	if ( i == 0 )
	{
		if ( m_bmpCloseBtn.GetSafeHandle() )
		{
			CRect r = GetButtonRect(0);
			m_bmpCloseBtn.Draw( pDC, r.left, r.top, 
				CRect( state * r.Width(), 0, (state + 1 ) * r.Width(), m_bmpCloseBtn.Height() ), m_colTrans, m_bTrans );
		}
	}
	if ( i == 1 )
	{
		CRect r = GetButtonRect(1);
		if ( m_maxable && m_bmpMaxBtn.GetSafeHandle() )
			if ( m_winstate == 1&& m_bmpRestoreBtn.GetSafeHandle() )
				m_bmpRestoreBtn.Draw( pDC, r.left, r.top, 
					CRect( state * r.Width(), 0, (state + 1 ) * r.Width(), m_bmpCloseBtn.Height() ), m_colTrans, m_bTrans );
			else
				m_bmpMaxBtn.Draw( pDC, r.left, r.top, 
					CRect( state * r.Width(), 0, (state + 1 ) * r.Width(), m_bmpCloseBtn.Height() ) , m_colTrans, m_bTrans);
	}
	if ( i == 2 )
	{
		if ( m_minable && m_bmpMinBtn.GetSafeHandle() )
		{
			CRect r = GetButtonRect(2);
			m_bmpMinBtn.Draw( pDC, r.left, r.top, 
				CRect( state * r.Width(), 0, (state + 1 ) * r.Width(), m_bmpCloseBtn.Height() ) , m_colTrans, m_bTrans);

		}
	}
	return TRUE;
	
}

void CSkinWin::OnGetMinMaxInfo(MINMAXINFO *lpMMI)
{
	lpMMI->ptMinTrackSize = CPoint( m_bmpTitle.Width() + m_bmpLeft.Width() +
		m_bmpRight.Width(),  m_bmpLeft.Height() + 20 );
}


void CSkinWin::OnWindowPosChanging(WINDOWPOS *lpwndpos)
{
}


BOOL CSkinWin::OnEraseBkgnd(CDC *pDC)
{
	if ( m_bmpDlg.GetSafeHandle() )
	{
		CRect r;
		GetClientRect( m_hWnd, r);
		for ( int i = 0; i < r.Width()/m_bmpDlg.Width() + 1; i++ )
			for ( int j = 0; j < r.Height()/m_bmpDlg.Height() + 1; j++ )
			{
				m_bmpDlg.Draw( pDC, CRect( i *  m_bmpDlg.Width(), j * m_bmpDlg.Height(),
					i *  m_bmpDlg.Width() + m_bmpDlg.Width(), j * m_bmpDlg.Height() + m_bmpDlg.Height() ) );
			}

		return TRUE;
	}
	else
		return FALSE;
}

CRect CSkinWin::GetMaximizeRect()
{
	CRect r;
	SystemParametersInfo(SPI_GETWORKAREA, 0, r, 0);
	return r;
}

BOOL CSkinWin::Maximize()
{
	CRect r= GetMaximizeRect();
	GetWindowRect( m_hWnd, m_rectRestoreWin );
	m_winstate = 1;
	::MoveWindow( m_hWnd, r.left, r.top, r.Width(), r.Height(), TRUE  );
	UpdateWindow( m_hWnd );
	return TRUE;
}

BOOL CSkinWin::Minimize()
{
	m_oldwinstate = m_winstate;
	m_winstate = 2;
	ShowWindow(m_hWnd, SW_MINIMIZE);
	return TRUE;
}

BOOL CSkinWin::Restore()
{
	if ( m_winstate == 1 )
	{
		::MoveWindow( m_hWnd, m_rectRestoreWin.left, m_rectRestoreWin.top,
			m_rectRestoreWin.Width(), m_rectRestoreWin.Height(), TRUE  );
		m_winstate = 0;
		UpdateWindow( m_hWnd );
	}
	return TRUE;
}

//void OnActivate(UINT nState,CWnd* pWndOther, BOOL bMinimized );

void CSkinWin::OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized)
{
	Default();
	m_bActive = ( nState == WA_ACTIVE || nState == WA_CLICKACTIVE );
	OnNcActivate(m_bActive);
}

LRESULT CSkinWin::OnSetText(WPARAM wp, LPARAM lp)
{
	char * str = ( char *)lp;
	Default();
	m_title = str;
	OnNcPaint(0);
	return 0;
}


void CSkinWin::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ( nID == SC_MAXIMIZE )
		Maximize();
	else if ( nID == SC_RESTORE && m_winstate == 1 )
		Restore();
	else if ( nID == SC_RESTORE && m_winstate == 2 )
	{
		ShowWindow( m_hWnd, SW_RESTORE );
		m_winstate = m_oldwinstate;
		OnNcPaint(0);
	}
	else 
		Default();
}

BOOL CSkinWin::PopupSysMenu(CPoint point)
{
	CWnd * pWnd = CWnd::FromHandle(m_hWnd);
	CMenu SysMenu;
	SysMenu.Attach( GetSystemMenu(m_hWnd, FALSE) );	
	SysMenu.TrackPopupMenu( 0, point.x, point.y, pWnd  );		
	SysMenu.Detach();
	return TRUE;
}

BOOL CSkinWin::HandleSysCommand(WPARAM wp, LPARAM lp)
{
	if ( lp == 0 )
	{
		CMenu SysMenu;
		SysMenu.Attach( GetSystemMenu(m_hWnd, FALSE) );
		int count = SysMenu.GetMenuItemCount();
		for( int i = 0; i < count ; i++ )
			if ( SysMenu.GetMenuItemID(i) == wp )
				break;
		SysMenu.Detach();
		if ( i >= count )
			return FALSE;
		if ( wp == SC_MAXIMIZE )
			Maximize();
		else if ( wp == SC_MINIMIZE )
			Minimize();
		else if ( wp == SC_RESTORE && m_winstate == 1 )
			Restore();
		else if ( wp == SC_RESTORE && m_winstate == 2 )
		{
			ShowWindow( m_hWnd, SW_RESTORE );
			m_winstate = m_oldwinstate;
			OnNcPaint(0);
		}
		else 
			SendMessage( m_hWnd, WM_SYSCOMMAND, wp, lp );
		return TRUE;
	}

	return FALSE;
}

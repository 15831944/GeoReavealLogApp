// Grippie.cpp : implementation file
#include "stdafx.h"
//#include "thumbnail.h"
#include "Grippie.h"

IMPLEMENT_DYNAMIC(CGrippie, CScrollBar)

CGrippie::CGrippie()
{
	m_parent=NULL;
}

CGrippie::~CGrippie()
{
}

BEGIN_MESSAGE_MAP(CGrippie, CScrollBar)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL CGrippie::Create(CWnd* parent)
{
	m_parent=parent;
	if(m_parent==NULL)
		return FALSE;
	CRect clientRect;
	m_parent->GetClientRect(&clientRect);
	
	CRect grippieRect;
	grippieRect.right=clientRect.right;
	grippieRect.bottom=clientRect.bottom;
	grippieRect.left=clientRect.right-GRIPPIE_SQUARE_SIZE;
	grippieRect.top=clientRect.bottom-GRIPPIE_SQUARE_SIZE;
	return CScrollBar::Create(WS_CHILD|WS_VISIBLE|SBS_SIZEGRIP|WS_CLIPSIBLINGS,
									grippieRect, m_parent, 0);
}

void CGrippie::OnParentSize(void)
{
	if(m_parent==NULL)
		return;
	CRect clientRect;
	m_parent->GetClientRect(&clientRect);
	if(m_hWnd!=NULL)
		SetWindowPos(NULL,
			clientRect.right-GRIPPIE_SQUARE_SIZE,
			clientRect.bottom-GRIPPIE_SQUARE_SIZE,
			GRIPPIE_SQUARE_SIZE,
			GRIPPIE_SQUARE_SIZE,
			SWP_NOZORDER|SWP_SHOWWINDOW);
}


BOOL CGrippie::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if ( pWnd == this  &&  nHitTest == HTCLIENT )
    {
        ::SetCursor( ::LoadCursor( NULL, IDC_SIZENWSE ) );
        return( TRUE );
    }
    return CScrollBar::OnSetCursor(pWnd, nHitTest, message);
}
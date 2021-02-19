/*************************************************************************
*   ResultListDrw.cpp - implementation of ResultList draw object class   *           
**************************************************************************/

#include "stdafx.h" 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "TrackDraw.h"
#include "ResultListDraw.h"            


//#include "MinFontDialog.h"
//#include "LineListPage.h"
//#include "FillListPage.h"
//#include "ResultListPage.h"
//#include "ResultListOut.h"
//#include "HeadPlotView.h"

IMPLEMENT_SERIAL(CResultListObj, CTextObj, 0)

CResultListObj::CResultListObj()
{
	m_ShapeName = "ResultList";
	m_TopSpace = 80;
	m_LeftSpace=4;
	m_RemHight=250;
	m_HeadCY=40;
	m_BodyCY=28;
	ResultSum=0;
}

CResultListObj::CResultListObj(const CRect& rect,CLogPlotDoc* pDocument) 
		: CTextObj(rect,NULL,pDocument)
{
	m_bClip = TRUE;
	m_ShapeName = "ResultList";
	m_TextVertAlignMode = IDM_TOP;
	m_TextHoriAlignMode = IDM_LEFT;
	m_LogPen.lopnColor = RGB(0, 0x80, 0);
	m_TopSpace = 80;
	m_LeftSpace=4;
	m_RemHight=250;
	m_HeadCY=40;
	m_BodyCY=28;
	m_TitleText="测井综合解释成果表";
	m_NoteText="";
	m_WellName="";
	ResultSum=0;
	LOGFONT m_LogFont={-34,0,0,0,FW_BOLD,0,0,0,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH | FF_ROMAN,"隶书"};
	m_TitleFontColor=RGB(0,0x80,0);
	m_TitleFont=m_LogFont;
	m_TitleFont.lfHeight = (int) (m_TitleFont.lfHeight / 0.72 - 0.5);

	LOGFONT m_HeadLogFont={-16,0,0,0,FW_BOLD,0,0,0,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH | FF_ROMAN,"楷体_GB2312"};
	m_HeadFont=m_HeadLogFont;
	m_HeadFont.lfHeight = (int) (m_HeadFont.lfHeight / 0.72 - 0.5);	
	m_HeadFontColor=RGB(0,0x80,0);
	LOGFONT m_BodyLogFont= {-14,0,0,0,FW_BOLD,0,0,0,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH | FF_ROMAN,"仿宋_GB2312"};
	m_BodyFont=m_BodyLogFont;
	m_BodyFont.lfHeight = (int) (m_BodyFont.lfHeight / 0.72 - 0.5);
	
	m_BodyFontColor=RGB(0,0x80,0);
	m_NoteFont=m_BodyLogFont;
	m_NoteFont.lfHeight = (int) (m_NoteFont.lfHeight / 0.72 - 0.5);
	strcpy(m_NoteFont.lfFaceName ,"宋体");
	m_NoteFontColor=RGB(0,0x80,0);
	LOGFONT m_UnitLogFont= {
		-10,							//lfHeight
		0,							//lfWidth
		0,					    	//lfEscapement
		0, 							//lfOrientation
		FW_BOLD,						//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;
		ANSI_CHARSET,				//lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_ROMAN,	//lfPitchAndFamily
		"宋体"			//lfFaceName
	};
	m_UnitFont=m_UnitLogFont;
	m_UnitFontColor=RGB(0,0x80,0);
}

void CResultListObj::Serialize(CArchive& ar)
{
	long l;
	CTextObj::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_TitleText;
		ar << m_NoteText;
		ar << m_TopSpace;
		ar << m_LeftSpace;
		ar << m_RemHight;
		ar << m_HeadCY;
		ar << m_BodyCY;
		ar << m_WellName;
		ar << (LONG)m_TitleFontColor;
		SerializeLogFont(ar,m_TitleFont);
		ar << (LONG)m_HeadFontColor;
		SerializeLogFont(ar,m_HeadFont);
		ar << (LONG)m_UnitFontColor;
		SerializeLogFont(ar,m_UnitFont);
		ar << (LONG)m_BodyFontColor;
		SerializeLogFont(ar,m_BodyFont);
		ar << (LONG)m_NoteFontColor;
		SerializeLogFont(ar,m_NoteFont);
	}
	else
	{
		ar >> m_TitleText;
		ar >> m_NoteText;
		ar >> m_TopSpace;
		ar >> m_LeftSpace;
		ar >> m_RemHight;
		ar >> m_HeadCY;
		ar >> m_BodyCY;
		ar >> m_WellName;
		ar >> l; m_TitleFontColor = (COLORREF) l;
		SerializeLogFont(ar,m_TitleFont);
		ar >> l; m_HeadFontColor = (COLORREF) l;
		SerializeLogFont(ar,m_HeadFont);
		ar >> l; m_UnitFontColor = (COLORREF) l;
		SerializeLogFont(ar,m_UnitFont);
		ar >> l; m_BodyFontColor = (COLORREF) l;
		SerializeLogFont(ar,m_BodyFont);
		ar >> l; m_NoteFontColor = (COLORREF) l;
		SerializeLogFont(ar,m_NoteFont);
		ReadFile();
	}
}

void CResultListObj::SetRect(const CRect& rect)
{
	CRemDrawObj::SetRect(rect);
	m_Rect = rect;
	int h = ComputerTextHeight();
	m_Rect.bottom = m_Rect.top + m_TopSpace + h;
	m_Rect.left = pDoc->m_TrackRect.left;
	m_Rect.right = pDoc->m_TrackRect.right;
}                             

void CResultListObj::ReadFile()
{
	TCHAR *token, *end;
    TCHAR buffer[1024];
    CStdioFile File;
    CFileException ex;

    if(!File.Open(m_WellName, CFile::modeRead | CFile::typeText))
        return;

	int i;
    for(i=0;i<200;i++)
	{
		m_TitleNum[i]=0;
		m_BodyNum[i]=0;
	}

    // Read Header off file
    File.ReadString(buffer, 1024);
    // Get first token
    for (token=buffer, end=buffer; 
            *end && (*end != _T(',')) && (*end != _T('\n')); end++);
    if ((*end == _T('\0')) && (token == end)) token = NULL;
    *end = _T('\0');
	i=0;
	int len,CN;
	CString ConStr,str;
    while (token) 
    {
        //InsertColumn(token);
		str=token;
		while(str.Left(1)==" ")
			str.TrimLeft (" ");
		while(str.Right(1)==" ")
			str.TrimRight (" ");
		
		ConStr=str;
		CN=ConStr.Find(" ");
		while(CN>0)
		{
			str=ConStr.Left(CN);
			len=(int)strlen(str);
			if(len>m_TitleNum[i])
				m_TitleNum[i]=len;
			ConStr=ConStr.Mid(CN+1);
			CN=ConStr.Find(" ");
		}
		len=(int)strlen(ConStr);
		if(len>m_TitleNum[i])
            m_TitleNum[i]=len;
        // Get next token
        for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
                end++);
        if ((*end == _T('\0')) && (token == end)) token = NULL;
        *end = _T('\0');
		i++;
    }

    // Read in rest of data
    ResultSum = 0;m_RowNum=0;
    while (File.ReadString(buffer, 1024)) 
	{
        // Get first token
        for (token=buffer, end=buffer; 
                *end && (*end != _T(',')) && (*end != _T('\n')); end++);
        if ((*end == _T('\0')) && (token == end)) token = NULL;
        *end = _T('\0');
		
        int nSubItem = 0;
        while(token) 
		{
			str=token;
			str.TrimLeft();
			str.TrimRight();
			ConStr=str;
			CN=ConStr.Find("^");
			while(CN>0)
			{
				str=ConStr.Left(CN);
				len=(int)strlen(str);
				if(len>m_BodyNum[nSubItem])
					m_BodyNum[nSubItem]=len;
				ConStr=ConStr.Mid(CN+1);
				CN=ConStr.Find("^");
			}
			len=(int)strlen(ConStr);
			if(len>m_BodyNum[nSubItem])
				m_BodyNum[nSubItem]=len;
            // Get next token
            for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
                    end++);
            if ((*end == _T('\0')) && (token == end)) token = NULL;
            *end = _T('\0');
            nSubItem++;
        }
		if(m_RowNum<nSubItem)
			m_RowNum=nSubItem;
        ResultSum++;
    }
    File.Close();
}

BOOL CResultListObj::Properties(CView* pView)
{
	/*CString szHT;
	CPropertySheet m_Sheet("储层解释表属性");
	CResultListPage m_DataPage;
	BOOL bFirst = (m_WellName == "");
	m_DataPage.m_FileName= m_WellName;	
	m_DataPage.m_NoteText = m_NoteText;
	m_Sheet.AddPage(&m_DataPage);

	CResultListOut m_BasePage;
	m_BasePage.m_TopSpace = (int)(m_TopSpace*2.54/10+0.5);
	m_BasePage.m_LeftSpace = (int)(m_LeftSpace*2.54/10+0.5);
	m_BasePage.m_RemHight = (int)(m_RemHight*2.54/10+0.5);
	m_BasePage.m_HeadCY = (int)(m_HeadCY*2.54/10+0.5);
	m_BasePage.m_BodyCY = (int)(m_BodyCY*2.54/10+0.5);
	m_BasePage.m_TitleText =m_TitleText;
	m_Sheet.AddPage(&m_BasePage);
		
 	CLineListPage m_LinePage;
	m_LinePage.AddPen(&m_LogPen,m_ShapeName);
 	CFillListPage m_FillPage;
	m_FillPage.AddFill(&m_LogBrush,m_ShapeName);
	m_Sheet.AddPage(&m_FillPage);
	m_Sheet.AddPage(&m_LinePage);

	CMinFontPage m_FontPage;

	TEXTFONT m_TextFont[5];

	m_TextFont[0].m_ItemName = "标题";
	m_TextFont[0].m_Text = "标题";
	m_TextFont[0].m_ForeGround = m_TitleFontColor;
	m_TextFont[0].m_BackGround = RGB(255,255,255);
	m_TextFont[0].lf = m_TitleFont;
	m_FontPage.AddFont(&m_TextFont[0]);  

	m_TextFont[1].m_ItemName = "表头";
	m_TextFont[1].m_Text = "表头";
	m_TextFont[1].m_ForeGround = m_HeadFontColor;
	m_TextFont[1].m_BackGround = RGB(255,255,255);
	m_TextFont[1].lf = m_HeadFont;
	m_FontPage.AddFont(&m_TextFont[1]);

	m_TextFont[2].m_ItemName = "单位";
	m_TextFont[2].m_Text = "单位";
	m_TextFont[2].m_ForeGround = m_UnitFontColor;
	m_TextFont[2].m_BackGround = RGB(255,255,255);
	m_TextFont[2].lf = m_UnitFont;
	m_FontPage.AddFont(&m_TextFont[2]);

	m_TextFont[3].m_ItemName = "表体";
	m_TextFont[3].m_Text = "表体";
	m_TextFont[3].m_ForeGround = m_BodyFontColor;
	m_TextFont[3].m_BackGround = RGB(255,255,255);
	m_TextFont[3].lf = m_BodyFont;
	m_FontPage.AddFont(&m_TextFont[3]);

	m_TextFont[4].m_ItemName = "备注";
	m_TextFont[4].m_Text = "备注";
	m_TextFont[4].m_ForeGround = m_NoteFontColor;
	m_TextFont[4].m_BackGround = RGB(255,255,255);
	m_TextFont[4].lf = m_NoteFont;
	m_FontPage.AddFont(&m_TextFont[4]);

	m_Sheet.AddPage(&m_FontPage);
	if (m_Sheet.DoModal() == IDOK)
	{
		m_FontPage.OnOK();
		m_TitleFontColor = m_TextFont[0].m_ForeGround;
		m_TitleFont = m_TextFont[0].lf;
		m_TitleFont.lfEscapement = 0;
		m_HeadFontColor = m_TextFont[1].m_ForeGround;
		m_HeadFont = m_TextFont[1].lf;
		m_UnitFontColor = m_TextFont[2].m_ForeGround;
		m_UnitFont = m_TextFont[2].lf;
		m_BodyFontColor = m_TextFont[3].m_ForeGround;
		m_BodyFont = m_TextFont[3].lf;
		m_NoteFontColor = m_TextFont[4].m_ForeGround;
		m_NoteFont = m_TextFont[4].lf;

		if (m_TopSpace != (int)m_BasePage.m_TopSpace)
			m_TopSpace = (unsigned int)(m_BasePage.m_TopSpace * 10 / 2.54 - 0.5);
		if (m_LeftSpace!= (int)m_BasePage.m_LeftSpace)
			m_LeftSpace = (unsigned int)(m_BasePage.m_LeftSpace * 10 / 2.54 - 0.5);
		if (m_RemHight!= (int)m_BasePage.m_RemHight)
			m_RemHight = (unsigned int)(m_BasePage.m_RemHight * 10 / 2.54 - 0.5);
		if (m_HeadCY!= (int)m_BasePage.m_HeadCY)
			m_HeadCY = (unsigned int)(m_BasePage.m_HeadCY * 10 / 2.54 - 0.5);
		if (m_BodyCY!= (int)m_BasePage.m_BodyCY)
			m_BodyCY = (unsigned int)(m_BasePage.m_BodyCY * 10 / 2.54 - 0.5);
		m_NoteText = m_DataPage.m_NoteText;
		m_TitleText = m_BasePage.m_TitleText;
		m_WellName=m_DataPage.m_FileName ;
		ReadFile();	
		pDoc->SetResultListRect();
		pDoc->SetModifiedFlag();
		Invalidate(pView);
		return TRUE;
	}*/
	return FALSE;
}

int CResultListObj::ComputerTextHeight()
{
	CClientDC dc(NULL);
	dc.SetMapMode(MM_ANISOTROPIC);         
	CPoint pt;
	pt.x = dc.GetDeviceCaps(LOGPIXELSX);
	pt.y = dc.GetDeviceCaps(LOGPIXELSY);
	dc.SetViewportExt(pt.x,pt.y),
	dc.SetWindowExt(100, 100);              
	CFont m_Font;
	m_Font.CreateFontIndirect(&m_TextFont);
	CFont* pOldFont = dc.SelectObject(&m_Font);
	CSize m_CharSize = dc.GetTextExtent("A",1);
	int h = 100+m_HeadCY+ResultSum*m_BodyCY+m_RemHight;
	dc.SelectObject(pOldFont);
	return h;
}

void CResultListObj::Draw(CView* pView,CDC* pDC)
{
	if(!pDoc->m_bPlotResultList) return;
	CRect rect= m_Rect;
	pDoc->LPtoPP(rect);
	CPen pen,*pOldPen;
	if(m_LogPen.lopnWidth.x > 0)
	{
		if (!pen.CreatePenIndirect(&m_LogPen))
			return;
		pOldPen = pDC->SelectObject(&pen);
	}
	else  pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
	
	CSize size(10,10);
	int y = rect.top + m_TopSpace;
	pDC->SetTextAlign(TA_LEFT | TA_TOP);
	pDC->SetBkMode(TRANSPARENT);
		
	CFont* pOldFont;
	CFont m_Font1;
	if(m_Font1.CreateFontIndirect(&m_TitleFont))
	{
		pOldFont = pDC->SelectObject(&m_Font1);
		size = pDC->GetTextExtent(m_TitleText);
		pDC->SetTextColor(m_TitleFontColor);
		pDC->TextOut((m_Rect.left+m_Rect.right -m_LeftSpace-size.cx )/2,y,m_TitleText);
		pDC->SelectObject(pOldFont);
		m_Font1.DeleteObject();
	}

	y+=size.cy+20;
	CRect ListRect(m_Rect.left,y,m_Rect.right-m_LeftSpace ,y+m_HeadCY+ResultSum*m_BodyCY+m_RemHight);
	pDC->Rectangle (ListRect);
	y=ListRect.top+m_HeadCY;
	int i;
	for(i=0;i<ResultSum;i++)
	{
		pDC->MoveTo (ListRect.left+1 ,y);
		pDC->LineTo (ListRect.right-1 ,y);
		y+=m_BodyCY;
	}
	if(m_RemHight>1)
	{
		pDC->MoveTo (ListRect.left+1 ,y);
		pDC->LineTo (ListRect.right-1 ,y);
	}
	TCHAR *token, *end;
    TCHAR buffer[1024];
    CStdioFile File;
 	
    if(!File.Open(m_WellName, CFile::modeRead | CFile::typeText)) 
        return;
    // Read Header off file
    File.ReadString(buffer, 1024);

    // Get first token
    for (token=buffer, end=buffer; 
            *end && (*end != _T(',')) && (*end != _T('\n')); end++);
    if ((*end == _T('\0')) && (token == end)) token = NULL;
    *end = _T('\0');
	
	CFont m_Font3;
	m_Font3.CreateFontIndirect(&m_BodyFont);
	pOldFont=pDC->SelectObject(&m_Font3);
	CSize size3 = pDC->GetTextExtent("备");
	CFont m_Font2;
	m_Font2.CreateFontIndirect(&m_HeadFont);
	pDC->SelectObject(&m_Font2);
	pDC->SetTextColor(m_HeadFontColor);
	size = pDC->GetTextExtent("备");
	int x,cx,UnitY;
	int xx=0;
	for(i=0;i<m_RowNum;i++)
	{	
		if(m_BodyNum[i]*size3.cx>m_TitleNum[i]*size.cx)
			xx+=m_BodyNum[i]*size3.cx;
		else
			xx+=m_TitleNum[i]*size.cx;
	}
	
	for(i=0;i<m_RowNum;i++)
	{	
		if(m_BodyNum[i]*size3.cx>m_TitleNum[i]*size.cx)
			m_TitleNum[i]=ListRect.Width()*m_BodyNum[i]*size3.cx/xx;
		else
			m_TitleNum[i]=ListRect.Width()*m_TitleNum[i]*size.cx/xx;
	}

	CString str;		
	x=ListRect.left;
	int y1=ListRect.top;
	int cxx=size.cx+10*pDoc->m_ScaleFactor/100;
	if(m_RemHight>size.cy*4)
	{
		pDC->MoveTo (x+size.cx+10,y);
		pDC->LineTo (x+size.cx+10,ListRect.bottom );
		int zx=(ListRect.bottom-y-4*size.cy)/5;
		pDC->TextOut (x+5,y+zx,"解");
		pDC->TextOut (x+5,y+2*zx+size.cy,"释");
		pDC->TextOut (x+5,y+3*zx+2*size.cy,"说");
		pDC->TextOut (x+5,y+4*zx+3*size.cy,"明");
	}
	i=0;
	UnitY=y1+size.cy+6;
	while(token) 
	{
		if(i!=0)
		{
			pDC->MoveTo (x,ListRect.top+1 );
			pDC->LineTo (x,y);
		}	
		cx=m_TitleNum[i];
		str=token;
		str.TrimLeft();
		str.TrimRight();
		size=pDC->GetTextExtent(str);
		pDC->TextOut(x+(cx-size.cx)/2,y1+5,str);
		x+=cx;i++;				
		// Get next token
		for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
				end++);
		if ((*end == _T('\0')) && (token == end)) token = NULL;
		*end = _T('\0');
	}
	pDC->SelectObject(pOldFont);
	m_Font2.DeleteObject();
	
	CFont m_Font20;
	m_Font20.CreateFontIndirect(&m_UnitFont);					
	pDC->SelectObject(&m_Font20);		
	pDC->SetTextColor(m_UnitFontColor);
	x=ListRect.left;
	File.ReadString(buffer, 1024);
	for(token=buffer, end=buffer; 
		*end && (*end != _T(',')) && (*end != _T('\n')); end++);
	if ((*end == _T('\0')) && (token == end)) token = NULL;
	*end = _T('\0');
	i=0;
	while(token) 
	{
		cx=m_TitleNum[i];
		str=token;
		str.TrimLeft();
		str.TrimRight();
		size = pDC->GetTextExtent(str);
		if(i!=0)
			pDC->TextOut (x+(cx-size.cx )/2,UnitY,str);
		x+=cx;i++;				
		// Get next token
		for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
				end++);
		if ((*end == _T('\0')) && (token == end)) token = NULL;
		*end = _T('\0');
	}
	m_Font20.DeleteObject();		

	y=ListRect.top+m_HeadCY;
	pDC->SelectObject(&m_Font3);
	pDC->SetTextColor(m_BodyFontColor);
	CString ConStr,ConStr0;
	int CN,CW,CI;
	while(File.ReadString(buffer,1024)) 
	{
		// Get first token
		for (token=buffer, end=buffer; 
				*end && (*end != _T(',')) && (*end != _T('\n')); end++);
		if ((*end == _T('\0')) && (token == end)) token = NULL;
		*end = _T('\0');
		
		i = 0;
		x=ListRect.left;
		while (token) 
		{
			cx=m_TitleNum[i];
			str=token;
			str.TrimLeft();str.TrimRight();
			ConStr=str;
			ConStr0=str;
			CN=ConStr.Find("^");
			size = pDC->GetTextExtent(str);
			CW=1;
			while(CN>0)
			{
				str=ConStr.Left(CN);
				ConStr=ConStr.Mid(CN+1);
				CW++;
				CN=ConStr.Find("^");
			}
			CW=(m_BodyCY-CW*(size.cy))/2;
			CI=0;
			ConStr=ConStr0;
			CN=ConStr.Find("^");
			while(CN>0)
			{
				str=ConStr.Left(CN);
				ConStr=ConStr.Mid(CN+1);
				size = pDC->GetTextExtent(str);
				pDC->TextOut (x+(cx-size.cx )/2,y+CW+CI*size.cy,str);
				CI++;
				CN=ConStr.Find("^");
			}
			size = pDC->GetTextExtent(ConStr);
			pDC->TextOut (x+(cx-size.cx )/2,y+CW+CI*size.cy,ConStr);

			x+=cx;i++;		
			// Get next token
			for(token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
					end++);
			if((*end == _T('\0')) && (token == end)) token = NULL;
			*end = _T('\0');
		}
		y+=m_BodyCY;
	}
	pDC->SelectObject(pOldFont);
	m_Font3.DeleteObject();

    File.Close();
	if(m_RemHight>0)
	{
		CFont m_Font4;
		m_Font4.CreateFontIndirect(&m_NoteFont);
		pOldFont = pDC->SelectObject(&m_Font4);
		pDC->SetTextColor(m_NoteFontColor);
		CString DataListStr=m_NoteText;
		DataListStr.TrimLeft ();
		DataListStr.TrimRight ();
		size = pDC->GetTextExtent("备");
		int Dy=size.cy +10;
		y=ListRect.top+m_HeadCY+ResultSum*m_BodyCY+Dy+5;
			 	
		float kWx=1;
		int Dx=ListRect.Width ()-35-cxx;
		int Dx0=2*size.cx+3;
		CString Str,ss;	
		int m,n,j,StrNum;
		char CV;
		if(pDoc->m_ScaleFactor==100)
		{
			m_OkText="";
			for(;;)
			{
				m=DataListStr.Find(char(13));
				if(m==-1)	ss=DataListStr;
				else		ss=DataListStr.Left(m);	
				ss.TrimLeft ();
				ss.TrimRight ();
				StrNum=ss.GetLength ();
				j=0;Str="";n=0;
				m_OkText+=char(10);
				while(j<StrNum)
				{
					CV=ss.GetAt  (j);
					Str+=CV;						
					if(CV<0)
					{
						j++;
						Str+=ss.GetAt  (j);
					}
					size=pDC->GetTextExtent(Str);
					if(n==0)
					{
						if(size.cx>=Dx-Dx0)
						{
							pDC->TextOut(ListRect.left +cxx+10+Dx0,y,Str);
							m_OkText+=Str+char(13);
							y+=Dy;
							Str="";
							n++;
						}
					}
					else
					{
						if(size.cx>=Dx)
						{
							pDC->TextOut(ListRect.left +cxx+10,y,Str);
							m_OkText+=Str+char(13);
							y+=Dy;
							Str="";
						}
					}
					j++;
				}
				if(n==0)
				{
					pDC->TextOut(ListRect.left +cxx+10+Dx0,y,Str);
					m_OkText+=Str+char(13);
				}
				else
				{
					pDC->TextOut(ListRect.left +cxx+10,y,Str);
					m_OkText+=Str+char(13);
				}
				y+=Dy;
				if(m==-1) break;
				DataListStr=DataListStr.Mid(m+2);				
			}
		}
		else
		{
			DataListStr=m_OkText;
			n=0;CRect rect;
			for(;;)
			{
				if(DataListStr.GetAt (0)==char(10))
				{
					n=0;
					DataListStr=DataListStr.Mid (1);
				}
				m=DataListStr.Find(char(13));
				if(m==-1) break;
				Str=DataListStr.Left(m);
				
				rect.top =y;
				rect.right =ListRect.right -10;
				rect.bottom =y+Dy;
				if(n==0)
				{
					rect.left=ListRect.left +cxx+Dx0+10;
					n++;
				}
				else
					rect.left=ListRect.left +cxx+10;
				pDC->DrawText (Str,-1,&rect,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
				DataListStr=DataListStr.Mid(m+1);
				y+=Dy;
			}
		}
		pDC->SelectObject(pOldFont);
		m_Font4.DeleteObject();
	}
	pDC->SelectObject(pOldPen);
	CDrawObj::Draw(pView,pDC);
}

void CResultListObj::DrawTracker(CView* pView, CDC* pDC, TrackerState state)
{
	CRect rect = m_Rect;	
	pDoc->LPtoPP(rect);
	CPen m_Pen;
	if (m_LogPen.lopnWidth.x <= 0)
		m_Pen.CreatePen(0,2,RGB(0xff,0x0,0x0));
	else
		m_Pen.CreatePen(m_LogPen.lopnWidth.x,2,RGB(0xff,0x0,0x0));
	CPen* pOldPen = pDC->SelectObject(&m_Pen);
	CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	pDC->Rectangle(rect); 
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

BOOL CResultListObj::OnSetCursor(CView* pView,const CPoint& point)
{
	return FALSE;
}

int CResultListObj::HitTest(CView* pView,const CPoint& point)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);
	CRect rect  = m_Rect;
	rect.top = m_Rect.top + m_TopSpace;
	if (m_Rect.PtInRect(point)) return 0;
    return -1;
}

BOOL CResultListObj::OnBrowseWell() 
{
	static char BASED_CODE szFilter[] = 
	"数据表文件(*.List)|*.List|所有文件(*.*)|*.*||";
	CFileDialog  Browse(TRUE, "数据表文件(*.List)", "*.List", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "打开文件名";
	Browse.m_ofn.lpstrInitialDir=GetCurrentUserPath();
	if (Browse.DoModal()==IDOK)
	{
		m_WellName=Browse.GetPathName ();
		return TRUE;
	}
	return FALSE;
}

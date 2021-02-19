// 3DContourView.cpp : implementation file

#include "stdafx.h"
#include "Resource.h"
#include "3DContourView.h"
#include <algorithm>
#include "Dib.h"
#include "InverseDist.h"
#include "Kriging.h"
#include "Gradient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C3DContourView

IMPLEMENT_DYNCREATE(C3DContourView, CGLSurfaceView)

C3DContourView::C3DContourView() : m_dThickMin(0), m_dThickMax(0)
{
	m_bSmooth=true;
}

C3DContourView::~C3DContourView()
{
}


BEGIN_MESSAGE_MAP(C3DContourView, CGLSurfaceView)
	//{{AFX_MSG_MAP(C3DContourView)
	ON_COMMAND(ID_SHOWLEGEND, OnShowlegend)
	ON_UPDATE_COMMAND_UI(ID_SHOWLEGEND, OnUpdateShowlegend)
	ON_COMMAND(ID_CULLFACE, OnCullface)
	ON_UPDATE_COMMAND_UI(ID_CULLFACE, OnUpdateCullface)
	ON_COMMAND(ID_SMOOTH, OnSmooth)
	ON_UPDATE_COMMAND_UI(ID_SMOOTH, OnUpdateSmooth)
	ON_COMMAND(ID_BGCOLOR, OnBgcolor)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_SHOWWALL, OnShowwall)
	ON_UPDATE_COMMAND_UI(ID_SHOWWALL, OnUpdateShowwall)
	ON_COMMAND(ID_SHOWBOUNDARY, OnShowboundary)
	ON_UPDATE_COMMAND_UI(ID_SHOWBOUNDARY, OnUpdateShowboundary)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DContourView drawing

void C3DContourView::OnDraw(CDC* pDC)
{
	// TODO: add draw code here
	CGLSurfaceView::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// C3DContourView diagnostics

#ifdef _DEBUG
void C3DContourView::AssertValid() const
{
	CGLSurfaceView::AssertValid();
}

void C3DContourView::Dump(CDumpContext& dc) const
{
	CGLSurfaceView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// C3DContourView message handlers

void C3DContourView::OnInitialUpdate() 
{	
	// TODO: Add your specialized code here and/or call the base class
	CWaitCursor wait;
	int nInterpolater = m_nInterpolater;
	if(m_vecNum>110)
	{
		nInterpolater=0;
		m_nInterpolater=nInterpolater;
	}
	if(m_vecNum<2)
		return;
	vector<Point3D>& input = const_cast<vector<Point3D>&>(m_ir.Get3DPoints());

	Interpolater* pInterpolater = NULL;
	if(nInterpolater == 0)
		pInterpolater = new InverseDist(m_nDiameter3D, 4);
	else if(nInterpolater == 1)
		pInterpolater = new Kriging((Point3D*)&(*input.begin()),(Point3D*)&(*input.end()),4.0f);

	int nRadius=m_nDiameter3D/2;
	vector<double> vecZs;
	int i,j;double z;
	for(j=0;j<=m_nDiameter3D; j++)
	{
		for(i=0; i<=m_nDiameter3D; i++)
		{
			z=pInterpolater->GetInterpolatedZ(j-nRadius,i-nRadius,(Point3D*)&(*input.begin()), 
				(Point3D*)&(*input.end()));
			vecZs.push_back(z);
		}
	}
	delete pInterpolater;
	vector<double>::iterator iter2;
	iter2 = max_element(vecZs.begin(),vecZs.end());
	m_dThickMax = *iter2;
	iter2 = min_element(vecZs.begin(),vecZs.end());
	m_dThickMin = *iter2;

	double correct=(m_dThickMax-m_dThickMin)/(m_ZMax-m_ZMin);

	// These 3 lines are for determining thickness range. Typical thickness range 4000~7000 is too big
	// compared to x, y range of 200 so I had to reduce it. 
	double MinMaxDiff = m_dThickMax - m_dThickMin;
	double widthRatio = m_nDiameter3D;
	double ratio = MinMaxDiff/widthRatio*4;

	int dataSize =(m_nDiameter3D+1)*(m_nDiameter3D+1);
	C3DData* pdata = C3DData::GetInstance();
	pdata->SetSize(dataSize);

	int ii=0;
	for(j=0;j<=m_nDiameter3D; j++)
	{
		for(i=0; i<=m_nDiameter3D; i++) 
		{
			z=((vecZs[ii]-m_dThickMin)-(m_ZMin-m_zDataMin))/ratio*correct;
			if(z<0.) z=0.;
			if(z>(m_ZMax-m_ZMin)/ratio) 
				z=(m_ZMax-m_ZMin)/ratio;
			pdata->SetAt(ii,i,j,z);
			++ii;
		}
	}
	GetGrapher().Clear();
	GetGrapher().Add3DData(pdata);
	GetGrapher().SetAxisRange(m_nDiameter3D+1,(m_ZMax-m_ZMin)/ratio,m_nDiameter3D);
	GetGrapher().SetMinValue(0);
	GetGrapher().SetTitles(_T(""), _T("Y"),_T("Z"),_T("X"));
	GetGrapher().SetCullFace(false);
	GetGrapher().SetLegend(true);
	GetGrapher().m_SchemeFile=m_SchemeFile;
	GetGrapher().m_nLegend=m_nLegend;
	GetGrapher().m_nNumOfLevels=m_ZScaleNum;
	GetGrapher().m_ZMin=m_ZMin;
	GetGrapher().m_ZMax=m_ZMax;
	CGLSurfaceView::OnInitialUpdate();
	CGLSurfaceView::m_bResize=true;
}

void C3DContourView::UpdateIption()
{
	GetGrapher().m_nLegend=m_nLegend;
	CGLSurfaceView::m_bResize=true;
	Invalidate();
}

void C3DContourView::OnShowlegend() 
{
	// TODO: Add your command handler code here
	GetGrapher().SetLegend(!GetGrapher().IsLegend());
	Invalidate();
}

void C3DContourView::OnUpdateShowlegend(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(GetGrapher().IsLegend());
	Invalidate();
}

void C3DContourView::OnShowwall() 
{
	// TODO: Add your command handler code here
	GetGrapher().SetWall(!GetGrapher().IsWall());
	Invalidate();
}

void C3DContourView::OnUpdateShowwall(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(GetGrapher().IsWall());
}

void C3DContourView::OnShowboundary() 
{
	// TODO: Add your command handler code here
	GetGrapher().SetBoundary(!GetGrapher().IsBoundary());
	Invalidate();
}

void C3DContourView::OnUpdateShowboundary(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(GetGrapher().IsBoundary());
}

void C3DContourView::OnCullface() 
{
	// TODO: Add your command handler code here
	GetGrapher().SetCullFace(!GetGrapher().IsCullFace());
	Invalidate();
}

void C3DContourView::OnUpdateCullface(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(GetGrapher().IsCullFace());
}

void C3DContourView::OnSmooth() 
{
	// TODO: Add your command handler code here
	m_bSmooth=!m_bSmooth;
	if(m_bSmooth)
		GetGrapher().SetShadeModel(C3DGrapher::SMOOTH);
	else
		GetGrapher().SetShadeModel(C3DGrapher::FLAT);
	Invalidate();
}

void C3DContourView::OnUpdateSmooth(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bSmooth);
}

void C3DContourView::OnBgcolor() 
{
	// TODO: Add your command handler code here
	CColorDialog dlg;
	if(dlg.DoModal() == IDOK) {
		GetGrapher().SetBkGndColor(dlg.GetColor());
		Invalidate();
	}
}

void C3DContourView::OnEditCopy() 
{
	// TODO: Add your command handler code here
	CWaitCursor wait;

	CClientDC scrDC(this);
	CDC memDC;           // screen DC and memory DC
	CBitmap bitmap;
	CBitmap* pOldBitmap;  // handles to deice-dependent bitmaps

	CRect rect;
	GetClientRect(&rect);

	memDC.CreateCompatibleDC(&scrDC);
	/* create a bitmap compatible with the screen DC */
	bitmap.CreateCompatibleBitmap(&scrDC, rect.Width(), rect.Height());

	pOldBitmap = memDC.SelectObject(&bitmap);// select new bitmap into memory DC

	memDC.BitBlt(0, 0, rect.Width(), rect.Height(), &scrDC, 0, 0, SRCCOPY);
	HBITMAP hBitmap = (HBITMAP)bitmap.Detach();

	memDC.SelectObject(pOldBitmap);

	// Place the copy on the clipboard.
	::OpenClipboard(m_hWnd);
    ::EmptyClipboard();
    ::SetClipboardData(CF_BITMAP, hBitmap);
    ::CloseClipboard();

	// clean up 
	memDC.DeleteDC();
}

void C3DContourView::Init(LPCTSTR RootName,DWORD RootID)
{
	m_ir.m_nDiameter=m_nDiameter; //图形宽度
	m_ir.m_nDiameter3D=m_nDiameter3D;//图形宽度
	m_ir.Read(RootName,RootID,false);

	m_XMin=m_ir.m_XMin;					
	m_XMax=m_ir.m_XMax;
	m_YMin=m_ir.m_YMin;
	m_YMax=m_ir.m_YMax;
	m_ZMin=m_ir.m_ZMin;
	m_ZMax=m_ir.m_ZMax;
	m_nDiameter=m_ir.m_nDiameter;              
	m_nDiameter3D=m_ir.m_nDiameter3D;
	m_PointNum=m_ir.m_PointNum;
	m_vecNum=m_ir.m_vecNum;
	m_zDataMin=m_ir.m_ZMin;
	m_zDataMax=m_ir.m_ZMax;
}


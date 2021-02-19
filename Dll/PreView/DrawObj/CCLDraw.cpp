/******************************************************************
*   CCLDrw.cpp - implementation of the CCL draw object class  *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "CurveDraw.h"
#include "PreView.h"
#include "LogPlotDoc.h"
#include "GeoFIDio.h"
#include "TrackDraw.h"
#include "CCLDraw.h"

//#include "styledialog.h"
//#include "CCLPage.h"
//#include "CCLBasePage.h"
//#include "LineListPage.h"
//#include "FillListPage.h"
//#include "MinFontDialog.h"
//#include "CCLFillPage.h"
//#include "MainFrm.h"

IMPLEMENT_SERIAL(CCCLFill, CObject, 0)

CCCLFill::CCCLFill()
{
	m_Color = RGB(0,0,0);
	m_LthNo = 0;
	m_FillMode = FILLRIGHT;
	pSourceCurve = NULL;
	pTargetCurve = NULL;
}

CCCLFill::CCCLFill(CCCLFill& other)
{
	m_Color = other.m_Color;
	m_LthNo = other.m_LthNo;
	m_FillMode = other.m_FillMode;
	pSourceCurve = other.pSourceCurve;
	pTargetCurve = other.pTargetCurve;
	m_SourceCurveName = other.m_SourceCurveName;
	m_TargetCurveName = other.m_TargetCurveName;
	m_LthCurveName = other.m_LthCurveName;
	m_Text = other.m_Text;
}

void CCCLFill::Serialize( CArchive& ar )
{
	if(ar.IsStoring())
	{
		ar << (LONG)m_Color;
		ar << m_LthNo;
		ar << m_FillMode;
		ar << m_Text;
		ar << m_LthCurveName;
		ar << m_SourceCurveName;
		ar << m_TargetCurveName;
	}
	else
	{
		long l;
		ar >> l; m_Color = (COLORREF)l;
		ar >> m_LthNo;
		ar >> m_FillMode;
		ar >> m_Text;
		ar >> m_LthCurveName;
		ar >> m_SourceCurveName;
		ar >> m_TargetCurveName;
		if (m_TargetCurveName == "左边界") m_TargetCurveName = "左边界";
		if (m_TargetCurveName == "右边界") m_TargetCurveName = "右边界";
	}
}


IMPLEMENT_SERIAL(CCCLObj, CFIDDrawObj, 0)
        
CCCLObj::CCCLObj()
{
	m_Point = NULL;
	m_Value = NULL;
	m_pCCLdep=NULL;
	m_pCCLValue=NULL;
	m_CCLNum=0;
	m_bHeadShowFill = TRUE;
	m_FilterNum = 0;
	pFillCCLObj = NULL;
	m_DrawShape = CCLobj;
	m_bDataEdit = FALSE;
	m_LinePen.CreateLinePen(m_CCLStyleNo,m_CCLWidth,m_CCLColor);
	bErrorLthCCL = FALSE;
	SecondDraw=0;
	m_DisCCLDep=m_DisCCLLong=FALSE;
	m_HighValue=-9999.f;
	m_LowValue=-9999.f;
	flag = 1;
	nBufferPointNum = 5000;
}
    
CCCLObj::CCCLObj(CString szPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
			:CCurveObj(szPathName,rect,pParentObj,pDoc)
{
	m_Point = NULL;
	m_Value = NULL;
	m_pCCLdep=NULL;
	m_pCCLValue=NULL;
	m_CCLNum=0;
	m_bHeadShowFill = TRUE;	
	m_FilterNum = 0;
	m_bDataEdit = FALSE;
	m_Factor = 0; m_CalType = 0;
	m_CCLColor = RGB(0,0,0xff);
	m_CCLStyleNo = 1;
	m_CCLWidth = 2;
	pFillCCLObj = NULL;
	m_DrawShape = CCLobj;
	m_ScaleFont = m_NameFont; 
	m_ScaleFont.lfHeight = -10;
	m_ScaleFont.lfWeight = 300;
	bErrorLthCCL = FALSE;
	m_LinePen.CreateLinePen(m_CCLStyleNo,m_CCLWidth,m_CCLColor);
	SecondDraw=0;
	m_DisCCLDep=m_DisCCLLong=FALSE;
	m_HighValue=-9999.f;
	m_LowValue=-9999.f;
	flag = 1;
	nBufferPointNum = 5000;
}


CCCLObj::~CCCLObj()
{
	int k = (int)m_FillList.GetSize();
	if (k>0)
	{
		for (int i=0; i<k; i++)
			delete m_FillList[i];
		m_FillList.RemoveAll();
	}
	if (m_Point != NULL) delete m_Point;
	if (m_Value != NULL) delete m_Value;
	if (m_pCCLdep!=NULL) delete m_pCCLdep;
	if (m_pCCLValue!=NULL) delete m_pCCLValue;
}    

void CCCLObj::Serialize( CArchive& ar )
{
	long count;
	CFIDDrawObj::Serialize( ar );
	if(ar.IsStoring())
	{
		count = (long)m_FillList.GetSize();
		ar << count;
		for (int i=0; i<count; i++)
			m_FillList[i]->Serialize(ar);
		ar << m_bHeadShowFill;
		ar << m_FilterNum;
		ar << m_bDataEdit;
		ar << m_CalType;
		ar << m_Factor;
		ar << m_CCLStyleNo;
		ar << m_CCLWidth;
		ar << m_CCLColor;
		ar << m_DisCCLDep;
		ar << m_DisCCLLong;
		ar << m_HighValue;
		ar << m_LowValue;
		SerializeLogFont(ar,m_ScaleFont);
	}
	else
	{
		pDoc = (CLogPlotDoc*)ar.m_pDocument;
		ar >> count;
		for (int i=0; i<count; i++)
		{
			CCCLFill* pCCLFill = new CCCLFill;
			pCCLFill->Serialize( ar );
			m_FillList.Add(pCCLFill);
		}
		ar >> m_bHeadShowFill;
		ar >> m_FilterNum;
		ar >> m_bDataEdit;
		ar >> m_CalType;
		ar >> m_Factor;
		ar >> m_CCLStyleNo;
		ar >> m_CCLWidth;
		ar >> m_CCLColor;
		ar >> m_DisCCLDep;
		ar >> m_DisCCLLong;
		ar >> m_HighValue;
		ar >> m_LowValue;

		SerializeLogFont(ar,m_ScaleFont);
		m_LinePen.CreateLinePen(m_CCLStyleNo,m_CCLWidth,m_CCLColor);
		CreatCCLPoint();
	}
}

BOOL CCCLObj::Properties(CView* pView)
{
	//CString szHT;
	//CPropertySheet m_Sheet("曲线属性");
	//CCCLBasePage m_BasePage(this);
	//CCCLPage m_Page(this);
	//CCCLFillPage m_Fill(this);
	//CLineListPage m_LineList;
	//CFillListPage m_FillList;
	//
	//m_Sheet.AddPage(&m_BasePage);
	//m_Sheet.AddPage(&m_Page);

	//if (m_ParentType != 0)
	//{
	//	szHT.LoadString(IDS_FRAMELINE);
	//	m_LineList.AddPen(&m_FramePen,szHT);
	//	szHT.LoadString(IDS_FRAMEFILL);
	//	m_FillList.AddFill(&m_Brush,szHT);
	//	m_Sheet.AddPage(&m_LineList);
	//	m_Sheet.AddPage(&m_FillList);
	//}
	//else
	//{
	//	m_Sheet.AddPage(&m_Fill);
	//}
	//
	//CMinFontPage m_FontPage;
	//TEXTFONT m_TextFont[2];
	//szHT.LoadString(IDS_ALIASFONT);
	//m_TextFont[0].m_ItemName = szHT.GetBuffer(128);
	//szHT.ReleaseBuffer();
	//m_TextFont[0].m_Text = m_AliasName;
	//m_TextFont[0].m_ForeGround = m_NameColor;
	//m_TextFont[0].m_BackGround = RGB(255,255,255);
	//m_TextFont[0].lf = m_NameFont;
	//m_FontPage.AddFont(&m_TextFont[0]);
	//szHT.LoadString(IDS_ZERFONT);
	//m_TextFont[1].m_ItemName = szHT.GetBuffer(128);
	//szHT.ReleaseBuffer();
	//m_TextFont[1].m_Text = "012345";
	//m_TextFont[1].m_ForeGround = m_NameColor;
	//m_TextFont[1].m_BackGround = RGB(255,255,255);
	//m_TextFont[1].lf = m_ScaleFont;
	//m_FontPage.AddFont(&m_TextFont[1]);

	//m_Sheet.AddPage(&m_FontPage);

	//double m_PlotSdep1 = m_PlotSdep;
	//double m_PlotEdep1 = m_PlotEdep;

	//if (m_Sheet.DoModal() == IDOK)
	//{
	//	m_FontPage.OnOK();
	//	m_NameFont = m_TextFont[0].lf;
	//	m_ScaleFont = m_TextFont[1].lf;
	//	m_NameFont.lfEscapement = 0;
	//	m_ScaleFont.lfEscapement = 0;
	//	bFlush = TRUE;
	//	m_LinePen.CreateLinePen(m_CCLStyleNo,m_CCLWidth,m_CCLColor);
	//	CGeoFIDIO* pFIDIO = new CGeoFIDIO(m_strPathName);
	//	m_Rlev = m_SDep = m_EDep = 0;
	//	CRect m_SaveRect = m_Rect;
	//	m_SaveRect.top = m_HeadRect.top;
	//	bErrorFID = TRUE;
	//	bErrorLthCCL = FALSE;
	//	if (pFIDIO->Open(pDoc->m_szSever)&& IsDepthUnitOK(pFIDIO))
	//	{
	//		m_Rlev = pFIDIO->GetDepthLevel();
	//		if (m_Rlev > 0.001)
	//		{
	//			m_SDep = pFIDIO->GetStartDepth();
	//			m_EDep = pFIDIO->GetEndDepth();
	//			if (m_ParentType != 0) 
	//			{
	//				if ((m_PlotSdep != m_PlotSdep1) || (m_PlotEdep != m_PlotEdep1))
	//				{
	//					double m_PlotSdep2 = m_PlotSdep;
	//					double m_PlotEdep2 = m_PlotEdep;
	//					m_PlotSdep = m_PlotSdep1;
	//					m_PlotEdep = m_PlotEdep1;
	//					SetDepth(m_PlotSdep2,m_PlotEdep2);
	//					if (m_SaveRect.bottom<m_Rect.bottom) m_SaveRect.bottom = m_Rect.bottom;
	//				}
	//			}
	//			bErrorFID = FALSE;
	//		}
	//	}
	//	if (bErrorFID)
	//	{
	//		m_SDep = m_EDep = m_SSdep = m_EEdep = 0;
	//		bFlush = FALSE;
	//	}
	//	else
	//		CreatCCLPoint();
	//	delete pFIDIO;
	//	pDoc->SetModifiedFlag();
	//	InvalidateRect(m_SaveRect);
	//	return TRUE;
	//}
	return FALSE;
}


void CCCLObj::Line(int x1, int y1,int x2, int y2, CDC* pDC)
{     
	if((x1<-9000)||(x2<-9000))
	{
		pDC->MoveTo(x2,y2);
		return;
	}
	pDC->MoveTo(x1,y1);
	pDC->LineTo (x2,y2);
}

void CCCLObj::DrawHead(CDC *pDC)
{
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	char ss[10];      
	int k = (int)m_FillList.GetSize();
	if ((k > 0) && (m_bHeadShowFill))
	{
		CRect m_FillRect = rect;	
		CFont m_Font;
		m_Font.CreateFontIndirect(&m_NameFont);
	    CFont* pOldFont = pDC->SelectObject(&m_Font);
		CCCLFill* pFill = m_FillList[0];
		if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
			LithFillBox(m_FillRect, pFill->m_LthNo,pFill->m_Color,m_FillRect.TopLeft(),pDC,m_CCLColor);
		else
			LithFillBox(m_FillRect, pFill->m_LthNo,pFill->m_Color,m_FillRect.TopLeft(),pDC);
		CPen m_Pen;
		m_Pen.CreatePen(0,1,pFill->m_Color);
		CPen* pOldPen = pDC->SelectObject(&m_Pen);
		if (!(pFill->m_Text.IsEmpty()))
		{
			CRect rect;
		    CSize textsize = pDC->GetTextExtent(pFill->m_Text,pFill->m_Text.GetLength());   
			rect.top = m_FillRect.top + (m_FillRect.Height() - textsize.cy) / 2 - 2;
			rect.bottom = m_FillRect.bottom - (m_FillRect.Height() - textsize.cy) / 2;
			rect.left = m_FillRect.left + (m_FillRect.Width() - textsize.cx) / 2 - 2;
			rect.right = m_FillRect.right - (m_FillRect.Width() - textsize.cx) / 2 + 2;
			CBrush TextBoxBrush;
			TextBoxBrush.CreateSolidBrush(RGB(255,255,255));
			CBrush* pOldBrush=pDC->SelectObject (&TextBoxBrush);
			pDC->RoundRect (rect.left ,rect.top ,rect.right ,rect.bottom ,7,7);
			pDC->SelectObject (pOldBrush);
			if (IsGridCCL())
				pDC->SetTextColor(m_CCLColor);
			else
				pDC->SetTextColor(pFill->m_Color);  
	    	pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
			pDC->TextOut(rect.left + 2, rect.bottom, pFill->m_Text);
		}		
		pDC->MoveTo(m_FillRect.left,m_FillRect.top);
		pDC->LineTo(m_FillRect.left,m_FillRect.bottom);
		pDC->MoveTo(m_FillRect.right,m_FillRect.top);
		pDC->LineTo(m_FillRect.right,m_FillRect.bottom);

		pDC->MoveTo(rect.left,m_FillRect.top);
		pDC->LineTo(rect.right,m_FillRect.top);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldFont);
		return;
	}
	CFont* pOldFont;
	BOOL bGridCCL = IsGridCCL();
	if (m_LeftVal != m_RightVal)
	{
		CFont m_Font;
		m_Font.CreateFontIndirect(&m_ScaleFont);
	    pOldFont = pDC->SelectObject(&m_Font);
		pDC->SetTextColor(m_CCLColor);
	    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		pDC->TextOut((rect.left + rect.right)/2, rect.bottom - 7, m_UnitText, (int)strlen(m_UnitText));
	    pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
		dtos(m_LeftVal,ss);
	    pDC->TextOut(rect.left+2,rect.bottom-7,ss,(int)strlen(ss));
		dtos(m_RightVal,ss);
	    pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
		pDC->TextOut(rect.right-2,rect.bottom-7,ss,(int)strlen(ss));
		pDC->SelectObject(pOldFont);
	}

	CFont m_Font1;
	m_Font1.CreateFontIndirect(&m_NameFont);
    pOldFont = pDC->SelectObject(&m_Font1);
    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
    CSize textsize;
	if (m_UnitText == "") textsize.cy = 0;
	else textsize = pDC->GetTextExtent("文本");  
    pDC->TextOut((rect.left + rect.right)/2, rect.bottom - textsize.cy -10, m_AliasName,(int)strlen(m_AliasName));
	pDC->SelectObject(pOldFont);
	if (!bGridCCL) 
		DrawHorLine(rect.left,rect.right,rect.bottom - 5,&m_LinePen,pDC);
}

int CCCLObj::GetPoint(double& sdep,double& edep, CPoint*& pt)
{
	if (bErrorFID) return 0;
	if (bFlush)
	{
		CGeoFIDIO m_FIDIO(m_strPathName);
		bErrorFID = TRUE;
		if (m_FIDIO.Open(pDoc->m_szSever) && IsDepthUnitOK(&m_FIDIO))
		{
			m_Rlev = m_FIDIO.GetDepthLevel();
			if (m_Rlev > 0.001) 
			{
				m_SSdep = m_FIDIO.GetStartDepth();
				m_EEdep = m_FIDIO.GetEndDepth();
				bErrorFID = FALSE;
			}
		}
	}
	if (bErrorFID) 
	{
		m_SDep = m_EDep = m_SSdep = m_EEdep = 0;
		bFlush = FALSE;
		return 0;
	}
	double eps = fabs(m_Rlev/4);
	ASSERT(sdep <= edep);
	if ((sdep > m_EEdep) || (edep < m_SSdep)) 
		if (!bFlush) return 0;
	else
	{
		if (sdep < m_SSdep) sdep = m_SSdep;
		if (edep > m_EEdep) edep = m_EEdep;
	}

	if (bFlush || (sdep+eps <m_SDep) || (edep-eps > m_EDep))
	{
		double dep1 = sdep - nBufferPointNum*m_Rlev;
		if (dep1 < m_PlotSdep) dep1 = m_PlotSdep;
		double dep2 = edep + nBufferPointNum*m_Rlev;
		if (dep2 > m_PlotEdep) dep2 = m_PlotEdep;
		if (dep1 > dep2) return 0;
		CreatePoint(dep1,dep2);
	}
	if (m_PointNum == 0) return 0;
	if ((sdep > m_EDep) || (edep < m_SDep)) return 0;

	if (sdep < m_SDep) sdep = m_SDep;
	if (edep > m_EDep) edep = m_EDep;

	int k = Round((sdep - m_SDep) / m_Rlev);
	int num = Round((edep - sdep) / m_Rlev )+ 1;
	pt = m_Point + k;		
	if (num+k > m_PointNum) 
		num = m_PointNum-k;
	return num;
}

int CCCLObj::GetValue(double& sdep,double& edep, float*& value)
{
	if (bErrorFID) return 0;
	if (bFlush)
	{
		bErrorFID = TRUE;
		CGeoFIDIO m_FIDIO(m_strPathName);
		if (m_FIDIO.Open(pDoc->m_szSever) && IsDepthUnitOK(&m_FIDIO))
		{
			m_Rlev = m_FIDIO.GetDepthLevel();
			if (m_Rlev > 0.001) 
			{
				m_SSdep = m_FIDIO.GetStartDepth();
				m_EEdep = m_FIDIO.GetEndDepth();
				bErrorFID = FALSE;
			}
		}
	}
	if (bErrorFID) 
	{
		m_SDep = m_EDep = m_SSdep = m_EEdep = 0;
		bFlush = FALSE;
		return 0;
	}
	double eps = fabs(m_Rlev/4);
	ASSERT(sdep <= edep);
	if ((sdep > m_EEdep) || (edep < m_SSdep)) 
		if (!bFlush) return 0;
	else
	{
		if (sdep < m_SSdep) sdep = m_SSdep;
		if (edep > m_EEdep) edep = m_EEdep;
	}

	if (bFlush || (sdep+eps <m_SDep) || (edep-eps > m_EDep))
	{
		double dep1 = sdep - nBufferPointNum*m_Rlev;
		if (dep1 < m_PlotSdep) dep1 = m_PlotSdep;
		double dep2 = edep + nBufferPointNum*m_Rlev;
		if (dep2 > m_PlotEdep) dep2 = m_PlotEdep;
		if (dep1 > dep2) return 0;
		CreatePoint(dep1,dep2);
	}
	if (m_PointNum == 0) return 0;
	if ((sdep > m_EDep) || (edep < m_SDep)) return 0;

	if (sdep < m_SDep) sdep = m_SDep;
	if (edep > m_EDep) edep = m_EDep;

	int k = Round((sdep - m_SDep) / m_Rlev);
	int num = Round((edep - sdep) / m_Rlev )+ 1;
	value= m_Value + k;		
	if (num+k > m_PointNum) 
		num = m_PointNum-k;
	return num;
}

void CCCLObj::SaveCCLData()
{
	if((m_pCCLdep==NULL)
		||(m_pCCLValue==NULL))
		return;
	CString newName;
	static char BASED_CODE szFilter[] = 
	"数据文件(*.List)|*.List|所有文件(*.*)|*.*||";
	CFileDialog  Browse(FALSE, "数据表文件(*.List)", "CCL.List", 
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "保存文件";
	Browse.m_ofn.lpstrInitialDir=GetCurrentUserPath();

	if (Browse.DoModal()==IDOK)
	{
		CString filename =Browse.GetPathName();
		CString str;
		CStdioFile File;

		if (!File.Open(filename, CFile::modeWrite | CFile::modeCreate| CFile::typeText)) 
		{
	        AfxMessageBox(_T("保存数据表文件失败    "));
			return;
		}
		str="序号,接箍深度,接箍长度\n";
		File.WriteString(str);
		str.Format("1,%.2f, \n",m_pCCLdep[0]);
		File.WriteString(str);
		int i;
		for (i = 1; i < m_CCLNum; i++) 
		{
			str.Format("%d,%.2f,%.2f\n",i+1,m_pCCLdep[i*2],m_pCCLdep[i*2]-m_pCCLdep[i*2-2]);
			File.WriteString(str);
		}
		File.Close();

		filename=filename.Left (filename.ReverseFind('\\')+1)+"Scrip.List";
		if (!File.Open(filename, CFile::modeWrite | CFile::modeCreate| CFile::typeText)) 
		{
	        AfxMessageBox(_T("保存描述表文件失败    "));
			return;
		}
		str="序号,深度,曲线值,备注\n";
		File.WriteString(str);
		for(i=0;i<m_CCLNum*2;i++)
		{
			if(i%2==0)
				str.Format ("%d,%.2f,%.2f,%.2f\n",i+1,m_pCCLdep[i],m_pCCLValue[i],m_pCCLdep[i]);
			else
				str.Format ("%d,%.2f,%.2f,%.2f\n",i+1,m_pCCLdep[i],m_pCCLValue[i],
				                      m_pCCLdep[i+1]-m_pCCLdep[i-1]);
			File.WriteString(str);
		}
		File.Close();
	}
}

void CCCLObj::CreatCCLPoint()
{
if (bErrorFID) return;
	if (m_pCCLdep != NULL) delete m_pCCLdep;
	m_pCCLdep = NULL;
	m_CCLNum=0;
	if (m_pCCLValue != NULL) delete m_pCCLValue;
	m_pCCLValue=NULL;
	float sdep,edep,rlev;
	CGeoFIDIO* pFIDIO = new CGeoFIDIO(m_strPathName);
	if (pFIDIO->Open(pDoc->m_szSever) && IsDepthUnitOK(pFIDIO))
	{
		sdep= pFIDIO->GetStartDepth();
		if(sdep<m_PlotSdep) sdep=(float)m_PlotSdep;
		edep=pFIDIO->GetEndDepth();
		if(edep>m_PlotEdep) edep=(float)m_PlotEdep;
		rlev = pFIDIO->GetDepthLevel();
		if (rlev > 0.001)
		{
			int Num = (int)((edep - sdep)/ rlev) + 1 ;
			float *Val;
			Val= new float[Num];
			memset(Val,-9999,Num);
			float *temp,*temp1;
			temp=new float[Num];
			temp1=new float[Num];
			memset(temp,-9999,Num);
			memset(temp1,-9999,Num);
			pFIDIO->SeekToDepth((float)sdep);     
			Num = (int)pFIDIO->Read(2,(unsigned int )Num,Val);

			int i,k;
			float dep=0.f;
			float dep0=0.f;
			float data;
			for(i=0;i<Num;i++)
			{
				if((m_HighValue!=-9999.f)
					&&(Val[i]>m_HighValue))
				{
					data=0.f;
					while(Val[i]>data)
					{
						data=Val[i];
						i++;
					}
					dep0=sdep+i*rlev;
					if((m_CCLNum==0)
						||((m_CCLNum>0)&&(dep0-temp1[m_CCLNum-1]>0.5)))
					{
						temp1[m_CCLNum]=dep0;
						temp[m_CCLNum]=Val[i-1];
						m_CCLNum++;
					}
				}
				if((m_LowValue!=-9999.f)
					&&(Val[i]!=-9999.f)
					&&(Val[i]!=-999.25f)
					&&(Val[i]<m_LowValue))
				{
					data=9999999999999.f;
					while(Val[i]<data)
					{
						data=Val[i];
						i++;
					}
					dep=(float)(sdep+i*rlev);
					if((m_CCLNum==0)
						||((m_CCLNum>0)&&(dep-temp1[m_CCLNum-1]>0.5)))
					{
						if(fabs(dep-dep0)>0.5)
						{
							temp1[m_CCLNum]=dep;
							temp[m_CCLNum]=Val[i-1];
							m_CCLNum++;
						}
					}
				}				
			}
			if(m_CCLNum>0)
			{
				m_pCCLdep=new float[m_CCLNum*2];
				m_pCCLValue=new float[m_CCLNum*2];
				
				m_pCCLdep[0]=temp1[0];
				m_pCCLValue[0]=temp[0];
				for(i=1;i<m_CCLNum;i++)
				{
					m_pCCLdep[i*2]=temp1[i];
					m_pCCLValue[i*2]=temp[i];

					m_pCCLdep[i*2-1]=(m_pCCLdep[i*2]+m_pCCLdep[i*2-2])/2;
					k=(int)((m_pCCLdep[i*2-1]-m_PlotSdep)/rlev)+1;
					data=0.f;
					for(int j=0;j<20;j++)
						data+=Val[k-j];
					m_pCCLValue[i*2-1]=data/20.f;
				}
			}
			delete Val;
			delete temp;
			delete temp1;
		}
	}
	delete pFIDIO;
}

int CCCLObj::CreatePoint(double& sdep, double& edep)
{
	if (m_Point != NULL) delete m_Point;
	m_Point = NULL;
	if (m_Value != NULL) delete m_Value;
	m_Value = NULL;
	m_EDep = edep;
	m_SDep = sdep;
	m_PointNum = 0;
	if (bErrorFID) return 0;
	bErrorFID = TRUE;
	CGeoFIDIO* pFIDIO = new CGeoFIDIO(m_strPathName);
	if (pFIDIO->Open(pDoc->m_szSever) && IsDepthUnitOK(pFIDIO))
	{
		m_Rlev = pFIDIO->GetDepthLevel();
		if (m_Rlev > 0.001) bErrorFID = FALSE;
	}
	if (bErrorFID) 
	{
		m_SDep = m_EDep = m_SSdep = m_EEdep = 0;
		goto rtn;
	}
	m_SSdep = pFIDIO->GetStartDepth();
	m_EEdep = pFIDIO->GetEndDepth();
	if ((m_SDep > m_EEdep) || (m_EDep < m_SSdep)) goto rtn;
	if (m_SDep < m_SSdep) m_SDep = m_SSdep;
	if (m_EDep > m_EEdep) m_EDep = m_EEdep;
	m_PointNum = (int)((m_EDep - m_SDep) / m_Rlev) + 1 ;
	if (m_PointNum < 0) m_PointNum = 0;
	m_xMin = pDoc->GetRect().left;
	m_xMax = pDoc->GetRect().right;
	if (m_PointNum > 0)
	{
		float tmp;int i;
		m_Value = new float[m_PointNum];
		memset(m_Value,-9999,m_PointNum);

		m_Point = new CPoint[m_PointNum];
		pFIDIO->SeekToDepth((float)m_SDep);     
		m_PointNum = (int)pFIDIO->Read(2,(unsigned int )m_PointNum,m_Value);
		m_EDep = m_SDep + (m_PointNum-1) * m_Rlev; 
		if (m_CalType == 1) 
		{
			ASSERT(m_LeftVal > 0.000000001);
			ASSERT(m_RightVal > 0.000000001);
			tmp = (float) (m_Rect.Width() / (log10(m_RightVal)-log10(m_LeftVal)));
		}
		else tmp = (float)(m_Rect.Width() / (m_RightVal - m_LeftVal)); 
		int width = m_Rect.Width();
		double yy0=0;
		long XXX;
		for (i=0; i<m_PointNum; i++)
		{   
			m_Point[i].y = DepthToYCoor(m_SDep +i*m_Rlev); 
			if ((m_Value[i] ==-9999.f)||(m_Value[i]==-999.25))
			{
				if(m_bDataEdit)
					m_Point[i].x=-9999;
				else
				{
					if(m_LeftVal<m_RightVal)
						m_Point[i].x=m_Rect.left;
					else
						m_Point[i].x=m_Rect.right;
				}
				continue;
			}
			if (m_Factor < 0)
			{
				if (m_Point[i].x > m_xMax) m_Point[i].x = m_xMax;
				if (m_Point[i].x < m_xMin) m_Point[i].x = m_xMin;
			}
			else if (m_Factor == 0)
			{
				if (m_LeftVal > m_RightVal) 
				{
					if (m_Value[i] > m_LeftVal) m_Value[i]  = m_LeftVal;
					else if (m_Value[i] < m_RightVal) m_Value[i]  = m_RightVal;
				}
				else  
				{
					if (m_Value[i] < m_LeftVal) m_Value[i]  = m_LeftVal;
					else if (m_Value[i] > m_RightVal) m_Value[i]  = m_RightVal;
				}        
			}
			if (m_CalType == 1) 
			{
				if (m_Value[i] < 0.0001f) 
				{
					if (i == 0) m_Value[i] = 0.0001f;
					else m_Value[i] = m_Value[i-1];
				}
				XXX=m_Rect.left + Round((log10(m_Value[i])-log10(m_LeftVal))*tmp);
				if(XXX>32760) XXX=32760;
				if(XXX<-32760) XXX=-32760;
				m_Point[i].x = XXX;
			}
			else
			{
				XXX=m_Rect.left + Round((m_Value[i] - m_LeftVal)*tmp);			
				if(XXX>32760) XXX=32760;
				if(XXX<-32760) XXX=-32760;
				m_Point[i].x = XXX;
			}
		}
	}
rtn:
	delete(pFIDIO);    
	bFlush = FALSE;
	edep = m_EDep;
	sdep = m_SDep;
	return m_PointNum;
}

int CCCLObj::GetGridValue()
{
	char ext[128]; 
	CGeoFIDIO::GetFIDName(m_strPathName,ext); 
	int v = atoi(ext);
	if (v>100) v = 100;
	if (v < 0) v = 0;
	return m_Rect.left + m_Rect.Width() * v / 100;
}

BOOL CCCLObj::IsGridCCL()
{
	char ext[128]; 
	CGeoFIDIO::GetFIDName(m_strPathName,ext); 
	int len = (int)strlen(ext);
	if ((len > 3) || (len < 1))return FALSE;
	for (int i=0; i<len; i++)
		if (!isdigit(ext[i])) return FALSE;
	int v = atoi(ext);
	if (v>100) return FALSE;
	return TRUE;
}

void CCCLObj::DrawFill(CView* pView, CDC* pDC)
{
	if (IsGridCCL()) bErrorFID = FALSE;
	if (bErrorFID) return;
	int k=(int)m_FillList.GetSize();
	if(k<1) return;
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;		
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	m_ClipRect &= rect;
	double sdep;
	double edep;
	sdep = YCoorToDepth(m_ClipRect.top + pDoc->m_WinOrg.y) - m_Rlev;
	edep = YCoorToDepth(m_ClipRect.bottom  + pDoc->m_WinOrg.y) + m_Rlev;
	if (sdep < m_PlotSdep) sdep = m_PlotSdep;
	if (edep > m_PlotEdep) edep = m_PlotEdep;
	CPoint* point;
	int PointNum;
	CCCLFill* pFill = m_FillList[0];
	//原曲线、目标曲线深度不在填充范围
	float dep1,dep2;
	if (pFill->pTargetCurve != NULL)
	{
		CGeoFIDIO m_FID1(pFill->pTargetCurve->m_strPathName);
		if (m_FID1.Open(pDoc->m_szSever) && IsDepthUnitOK(&m_FID1))
		{
			m_Rlev = m_FID1.GetDepthLevel();
			if (m_Rlev > 0.001) 
			{
				dep1 = m_FID1.GetStartDepth();
				dep2 = m_FID1.GetEndDepth();
				if(sdep<dep1) sdep=dep1;
				if(edep>dep2) edep=dep2;
			}
			m_FID1.Close();
		}
		else
			return;
	}
	if (pFill->pSourceCurve != NULL)
	{
		CGeoFIDIO m_FID2(pFill->pSourceCurve->m_strPathName);
		if (m_FID2.Open(pDoc->m_szSever) && IsDepthUnitOK(&m_FID2))
		{
			m_Rlev = m_FID2.GetDepthLevel();
			if (m_Rlev > 0.001) 
			{
				dep1 = m_FID2.GetStartDepth();
				dep2 = m_FID2.GetEndDepth();
				if(sdep<dep1) sdep=dep1;
				if(edep>dep2) edep=dep2;
			}
			m_FID2.Close();
		}
		else
			return;
	}
	////////////////////////////////////////
	PointNum = GetPoint(sdep,edep,point);
	CPoint BrushOrg(0,0);
	if (PointNum > 1)
	{ 
		pDoc->LPtoPP(point,PointNum);                
		if (pFill->m_TargetCurveName == "左边界")
		{
			if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
				FillCurve(m_Rect.left,point,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,FILLLEFTRIGHT,m_CCLColor);
			else
				FillCurve(m_Rect.left,point,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,FILLLEFTRIGHT);
		}
		else if (pFill->m_TargetCurveName == "右边界")
		{
			if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
				FillCurve(m_Rect.right,point,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,FILLLEFTRIGHT,m_CCLColor);
			else
				FillCurve(m_Rect.right,point,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,FILLLEFTRIGHT);
		}
		else if (pFill->m_TargetCurveName == m_AliasName)
		{
			CPoint* point2 = new CPoint[PointNum];
			for (int i=0; i<PointNum; i++)
			{
				point2[i].x = m_Rect.left + m_Rect.right - point[i].x;
				point2[i].y = point[i].y;
			}
			if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
				FillCurve(point,point2,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,pFill->m_FillMode,m_CCLColor);
			else
				FillCurve(point,point2,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,pFill->m_FillMode);
			delete point2;
		}
		else 
		{
			if (pFill->pTargetCurve == NULL)
				pFill->pTargetCurve = ((CTrackObj*)m_pParentObj)->FindAliasCurve(pFill->m_TargetCurveName);
			if (pFill->pTargetCurve == NULL) return;
			if (pFill->pTargetCurve->IsGridCurve())
			{
				int x = pFill->pTargetCurve->GetGridValue();
				int m_FillMode = pFill->m_FillMode;
				if (m_FillMode == FILLLEFT) m_FillMode = FILLRIGHT;
				else if (m_FillMode == FILLRIGHT) m_FillMode = FILLLEFT;
				if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
					FillCurve(x,point,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,m_FillMode,m_CCLColor);
				else
					FillCurve(x,point,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,m_FillMode);
			}
			else
			{
				CPoint* point1;
				double sdep1 = sdep; double edep1 = edep;
				int PointNum1 = pFill->pTargetCurve->GetPoint(sdep1,edep1,point1);			
				if (PointNum1 > 1) 
				{
					int x;
					CPoint* point0=new CPoint[PointNum];
					for(x=0;x<PointNum;x++)
						point0[x].x=point[x].x;

					pDoc->LPtoPP(point1,PointNum1);
					CPoint* point10=new CPoint[PointNum1];
					for(x=0;x<PointNum1;x++)
						point10[x].x=point1[x].x;								
					if((m_Factor>0)&&(m_Factor<3))
					{						
						for(x=0;x<PointNum1;x++)
						{
							if(point1[x].x < m_Rect.left ) 
								point1[x].x = m_Rect.left;
							if(point1[x].x > m_Rect.right )
								point1[x].x = m_Rect.right;
						}
						for(x=0;x<PointNum;x++)
						{
							if(point[x].x < m_Rect.left ) 
								point[x].x = m_Rect.left;
							if(point[x].x > m_Rect.right )
								point[x].x = m_Rect.right;
						}
					}

					if(PointNum!=PointNum1)
					{
						CPoint* pointX=new CPoint[PointNum];
						int j;
						double XK=m_Rlev/pFill->pTargetCurve->m_Rlev;
						int K=0;
						for(j=0;j<PointNum;j++)
						{
							pointX[j].y=point[j].y;
							k=(int)(j*XK);
							if(k>PointNum1)
								pointX[j].x=point[j].x;
							else
								pointX[j].x=point1[k].x;
						}
						if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
							FillCurve(point,pointX,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,pFill->m_FillMode,m_CCLColor);
						else
							FillCurve(point,pointX,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,pFill->m_FillMode);
						delete pointX;
					}
					else
					{
						if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
							FillCurve(point,point1,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,pFill->m_FillMode,m_CCLColor);
						else
							FillCurve(point,point1,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,pFill->m_FillMode);
					}
					for(x=0;x<PointNum;x++)
						point[x].x=point0[x].x;
					delete point0;
					for(x=0;x<PointNum1;x++)
						point1[x].x=point10[x].x;
					delete point10;
					pDoc->PPtoLP(point1,PointNum1);                
				}
			}
		}
		pDoc->PPtoLP(point,PointNum);                
	}
}

void CCCLObj::DrawClient(CDC *pDC)
{                     
	if (IsGridCCL()) bErrorFID = FALSE;
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	if ((m_pParentObj == NULL) || (m_ParentType != 0)) 
		CFIDDrawObj::DrawClient(pDC);
	if (bErrorFID) return;	
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	m_ClipRect &= rect;
	if (IsGridCCL())
	{
		int x = GetGridValue();
		LINE m_Line;
		m_Line.pPen = &m_LinePen;
		flag = 1;
		m_LinePen.org = 0;
		m_Line.pDC = pDC;
		Line(x,m_ClipRect.top,x,m_ClipRect.bottom,pDC);
		return;
	}
	double sdep;
	double edep;
	sdep = YCoorToDepth(m_ClipRect.top + pDoc->m_WinOrg.y) - m_Rlev;
	edep = YCoorToDepth(m_ClipRect.bottom  + pDoc->m_WinOrg.y) + m_Rlev;
	if (sdep < m_PlotSdep) sdep = m_PlotSdep;
	if (edep > m_PlotEdep) edep = m_PlotEdep;
	CPoint* point;
	int PointNum = 0;
	PointNum = GetPoint(sdep,edep,point);
	if (PointNum > 2) 
	{ 
		pDoc->LPtoPP(point,PointNum);                
		CPoint* point2 = NULL;
		for (int i=0; i<m_FillList.GetSize(); i++)
		{
			if (m_FillList[i]->m_TargetCurveName == m_AliasName)
			{
				point2 = new CPoint[PointNum];
				for (int i=0; i<PointNum; i++)
				{
					point2[i].x = m_Rect.left + m_Rect.right - point[i].x;
					point2[i].y = point[i].y;
				}
				break;
			}
		}
		CPen m_Pen;
		m_Pen.CreatePen(0,m_CCLWidth,m_CCLColor);
		CPen* pOldPen = pDC->SelectObject(&m_Pen); 
		LINE m_Line;
		m_Line.pPen = &m_LinePen;
		flag = 1;
		m_LinePen.org = 0;
		m_Line.pDC = pDC;
	/*	int nSaveDC;
		if(pDoc->m_ScaleNo==7&&pDoc->m_bPrint)
		{
			nSaveDC = pDC->SaveDC();
			pDC->SetMapMode(MM_TEXT);
		}
		for (i=1;i<PointNum; i++)
			Line(point[i-1].x,point[i-1].y,point[i].x,point[i].y,pDC);*/
		int ww=rect.Width();		
		int Factor=m_Factor;
		m_Factor=1;
		if(Factor>1)
		{
			flag=1;
			for (i=1; i<PointNum; i++)
				Line(point[i-1].x-ww,point[i-1].y,point[i].x-ww,point[i].y,pDC);
			flag=1;
			for (i=1; i<PointNum; i++)
				Line(point[i-1].x+ww,point[i-1].y,point[i].x+ww,point[i].y,pDC);
		}
		flag=1;
		for (i=1; i<PointNum; i++)
			Line(point[i-1].x,point[i-1].y,point[i].x,point[i].y,pDC);
		m_Factor=Factor;
//		if(pDoc->m_ScaleNo==7&&pDoc->m_bPrint>=0)
//				pDC->RestoreDC(nSaveDC);
		pDC->SelectObject(pOldPen);
		if (point2 != NULL)	delete point2;
		pDoc->PPtoLP(point,PointNum);
		if(((m_DisCCLDep||m_DisCCLLong))
			&&(m_pCCLdep!=NULL))
			DrawCCLDep(pDC,(float)sdep,(float)edep);
	}
}

void CCCLObj::DrawCCLDep(CDC *pDC,float sdep,float edep)
{     
	if((m_pCCLdep==NULL)
		||(m_pCCLValue==NULL))
		return;

	int i;
	CPoint point,point0,point1;
	float tmp;
	if (m_CalType == 1) 
	{
		ASSERT(m_LeftVal > 0.000000001);
		ASSERT(m_RightVal > 0.000000001);
		tmp = (float) (m_Rect.Width() / (log10(m_RightVal)-log10(m_LeftVal)));
	}
	else 
		tmp = (float)(m_Rect.Width() / (m_RightVal - m_LeftVal)); 
	
	CFont hTextFont;
    hTextFont.CreateFont(
		-12,					        //lfHeight
		0,							//lfWidth
		0,							//lfEscapement
		0,							//lfOrientation
		FW_BOLD,					//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;	
		ANSI_CHARSET,				//lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_SWISS,	//lfPitchAndFamily
		"Courier New"               //"MS Serif" "Courier New""Times New Roman"
	);
	CFont *pOldFont = pDC->SelectObject(&hTextFont);
	pDC->SetTextColor(0x0);
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	pDC->SetBkMode(TRANSPARENT);
	CString str;
	for(i=0;i<m_CCLNum*2;i++)
	{
		if((m_pCCLdep[i]>sdep)
			&&(m_pCCLdep[i]<edep))
		{
			point.y=DepthToYCoor(m_pCCLdep[i]);
			if (m_CalType == 1) 
			{
				if (m_pCCLValue[i] < 0.0001f) 
					m_pCCLValue[i] = 0.0001f;
				point.x=m_Rect.left + Round((log10(m_pCCLValue[i])-log10(m_LeftVal))*tmp);
			}
			else
				point.x=m_Rect.left + Round((m_pCCLValue[i] - m_LeftVal)*tmp);	
			if(m_DisCCLDep)
			{
				pDoc->LPtoPP(&point,1);
				if(i%2==0)
					str.Format ("%.2f",m_pCCLdep[i]);
				else
					str.Format ("%.2f",m_pCCLdep[i+1]-m_pCCLdep[i-1]);
				pDC->TextOut(point.x,point.y,str);	
			}
		}
	}
	pDC->SelectObject(pOldFont);	
}
BOOL CCCLObj::IsFillCCL(CCCLObj* pObj)
{
	ASSERT(pObj != NULL);
	for (int i=0; i<m_FillList.GetSize(); i++)
		if ((m_FillList[i]->pTargetCurve != NULL) && (m_FillList[i]->pTargetCurve == pObj))
		   return TRUE;
	return FALSE;
}

BOOL CCCLObj::IsFillCCL(CString name)
{
	for (int i=0; i<m_FillList.GetSize(); i++)
		if (m_FillList[i]->m_TargetCurveName == name) return TRUE;
	return FALSE;
}

void CCCLObj::FindAllFillCCL()
{
	CCurveObj* pObj;
	if (m_pParentObj == NULL) return;
	if ((m_pParentObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (m_ParentType == 0))
	{
		for (int i=(int)m_FillList.GetSize()-1; i>=0; i--)
		{
			if ((m_FillList[i]->m_TargetCurveName == "左边界") || (m_FillList[i]->m_TargetCurveName == "右边界")) continue;
			if ((pObj = ((CTrackObj*)m_pParentObj)->FindAliasCurve(m_FillList[i]->m_TargetCurveName)) != NULL)
				m_FillList[i]->pTargetCurve = pObj;
			else
			{
				delete m_FillList[i];
				m_FillList.RemoveAt(i);
			}

		}
	}
}

int CCCLObj::RemoveFillCCL(CCCLObj* pObj)
{
	ASSERT(pObj != NULL);
	for (int i=0; i<m_FillList.GetSize(); i++)
	{
		if ((m_FillList[i]->pTargetCurve != NULL) && (m_FillList[i]->pTargetCurve == pObj)) 
		{
			delete m_FillList[i];
			m_FillList.RemoveAt(i);
			return 0;
		}
	}
	return -1;
}

int CCCLObj::RemoveFillCCL(CString name)
{
	for (int i=0; i<m_FillList.GetSize(); i++)
	{
		if (m_FillList[i]->m_TargetCurveName == name)
		{
			delete m_FillList[i];
			m_FillList.RemoveAt(i);
			return 0;
		}
	}
	return -1;
}

int CCCLObj::AddFillCCL(CCCLFill* pFill)
{
	ASSERT(pFill != NULL);
	ASSERT(pFill->IsKindOf(RUNTIME_CLASS(CCCLFill)));
	pFill->m_SourceCurveName = m_AliasName;
	pFill->pSourceCurve = this;
	return (int)m_FillList.Add(pFill);
}

void CCCLObj::RemoveAllFillCCL()
{
	for (int i=0; i<m_FillList.GetSize(); i++)
		delete m_FillList[i];
	m_FillList.RemoveAll();
}

void CCCLObj::DrawTracker(CView* pView, CDC* pDC, TrackerState state)
{
	CFIDDrawObj::DrawTracker(pView,pDC,state);
}

int CCCLObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if (bErrorFID) return -1;
	double sdep = YCoorToDepth(point.y);
	if ((sdep < m_SSdep) || (sdep > m_EEdep)) return -1;
	CGeoFIDIO* pFID = new CGeoFIDIO(m_strPathName);
	nHandle = -1;
	if (pFID->Open(pDoc->m_szSever))
	{
		pFID->SeekToDepth((float)(sdep-m_Rlev));
		pFID->Read(2,3,value);
		double tmpc,tmpval;
		m_LeftVal1 = m_LeftVal;	m_RightVal1 = m_RightVal;
		if (m_CalType == 1)
		{
			m_LeftVal1 = log10(m_LeftVal);
			m_RightVal1 = log10(m_RightVal);
		}
		tmpc = (double)(m_Rect.Width() / (m_RightVal1 - m_LeftVal1)); 
		for (int i=0; i<3; i++)
		{ 
			pt[i].y = DepthToYCoor(sdep + (i-1)*m_Rlev) - pDoc->m_WinOrg.y;
			tmpval = value[i];			
			if (m_CalType == 1) 
			{
				if (tmpval < 0.00000001f) tmpval = 0.00000001f;
				tmpval = log10(tmpval);
			}
			pt[i].x = m_Rect.left + Round((tmpval-m_LeftVal1)*tmpc);
			if (m_Factor > 0)	
			{
				if (i == 1) continue;
				while (pt[i].x < m_Rect.left) 
					pt[i].x = m_Rect.right - (m_Rect.left - pt[i].x) / m_Factor;
				while (pt[i].x > m_Rect.right) 
					pt[i].x = m_Rect.left + (pt[i].x - m_Rect.right) / m_Factor;
			}
			else if (m_Factor == 0) 
			{
				if (pt[i].x < m_Rect.left) pt[i].x = m_Rect.left;
				if (pt[i].x > m_Rect.right) pt[i].x = m_Rect.right;
			}
		}
		if (m_Factor > 0)
		{
			double v;
			while (pt[1].x < m_Rect.left) 
			{
				pt[1].x = m_Rect.right - (m_Rect.left - pt[1].x) / m_Factor;
				v = m_RightVal1 - m_LeftVal1;
				m_RightVal1 = m_LeftVal1;
				m_LeftVal1 = m_RightVal1 - v * m_Factor;
			}
			while (pt[1].x > m_Rect.right) 
			{
				pt[1].x = m_Rect.left + (pt[1].x - m_Rect.right) / m_Factor;
				v = m_RightVal1 - m_LeftVal1;
				m_LeftVal1 = m_RightVal1;
				m_RightVal1 = m_LeftVal1 + v * m_Factor;
			}
		}
		if ((point.x < pt[1].x+2) && (point.x >= pt[1].x-2)) nHandle = 9;
	}
	delete pFID;
	return nHandle;
}

void CCCLObj::DrawFocus(CView* pView)
{
	if (nDragHandle < 9) 
	{ 
		CFIDDrawObj::DrawFocus(pView);
		return;
	}
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc,NULL);
	dc.SetROP2(R2_XORPEN);
    CPen m_Pen(PS_DOT,1,RGB(0,0,0));
   	CPen *pOldPen = dc.SelectObject(&m_Pen);
	dc.Polyline(pt,3);
	dc.SelectObject(pOldPen);
	dc.SetROP2(R2_COPYPEN);
}

BOOL CCCLObj::OnToolTip(CView* pView,CRect& rect,CString& text)
{
	CPoint point = rect.TopLeft();
	int nHandle = HitTest(pView,point);
	if (nHandle == 9)
	{
		rect.InflateRect(2,2);
		char str[200];
		double PointDep = YCoorToDepth(point.y);
		PointDep=((int)(PointDep/m_Rlev))*m_Rlev;
		dtos(PointDep,str);
		strcat(str,"  ");
		char s[100];
		dtos((double)value[1],s);
		strcat(str,s);
		text = str;
		return TRUE;
	}
	return FALSE;
}

void CCCLObj::DrawCCLFocuse(CView* pView)
{
	if (IsGridCurve()) bErrorFID = FALSE;	
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	CDC *pDC=pView->GetDC ();
	pView->OnPrepareDC (pDC,NULL);
	if (!pDC->RectVisible(rect)) return;
	if ((m_pParentObj == NULL) || (m_ParentType != 0)) 
		CFIDDrawObj::DrawClient(pDC);
	if (bErrorFID) return;	
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	m_ClipRect &= rect;
	if (IsGridCurve())
	{
		int x = GetGridValue();
		LINE m_Line;
		m_Line.pPen = &m_LinePen;
		flag = 1;
		m_LinePen.org = 0;
		m_Line.pDC = pDC;
		Line(x,m_ClipRect.top,x,m_ClipRect.bottom,pDC);
		return;
	}
	double sdep;
	double edep;
	sdep = YCoorToDepth(m_ClipRect.top + pDoc->m_WinOrg.y) - m_Rlev;
	edep = YCoorToDepth(m_ClipRect.bottom  + pDoc->m_WinOrg.y) + m_Rlev;
	if (sdep < m_PlotSdep) sdep = m_PlotSdep;
	if (edep > m_PlotEdep) edep = m_PlotEdep;
	CPoint* point;
	int m_PointNum = 0;
	m_PointNum = GetPoint(sdep,edep,point);
	if (m_PointNum > 2) 
	{ 
		pDoc->LPtoPP(point,m_PointNum);                
		pDC->SetROP2(R2_XORPEN);
		CPen m_Pen(PS_DOT,1,RGB(0,0,0));
   		CPen *pOldPen = pDC->SelectObject(&m_Pen);
		pDC->Polyline (point,m_PointNum-2);
		pDC->SetROP2(R2_COPYPEN);
		pDC->SelectObject(pOldPen);
		pDoc->PPtoLP(point,m_PointNum);                
	}
}
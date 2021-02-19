#include "stdafx.h"
#include "style.h"
#include <io.h>
#include "GeoFIDio.h"
#include "GeoAdo.h"

//得到岩性符号数据short int[32*32]
//DWORD GetLithSymbol(DWORD iLithID,DWORD nLen,char *pBuf)
//{
//	if(m_szLithDB.IsEmpty())
//		return 0;
//	CGeoAdo LithAdo;
//	if(!LithAdo.InitData(m_szLithDB)) 
//		return 0;
//
//	CString cID;
//	cID.Format("%d",iLithID);
//	//将数据库中LithByte读取到pBuf
//	_RecordsetPtr  m_pRs= LithAdo.GetRS("SELECT * FROM LithSymbol WHERE LithID=" + cID);
//	//读取LithByte字段的实际大小
//	DWORD Size = m_pRs->GetFields()->GetItem("LithByte")->ActualSize;		
//	BSTR HUGEP *Buffer=NULL;
//	HRESULT hr;
//	if(Size>0)
//	{		
//		//从BLOB字段中读取数据到varBLOB中
//		_variant_t varBLOB = m_pRs->GetFields()->GetItem("LithByte")->GetChunk(Size);		
//		if(varBLOB.vt == (VT_ARRAY | VT_UI1))
//		{
//			hr=SafeArrayAccessData(varBLOB.parray,(void HUGEP**)&Buffer); 
//			if(FAILED(hr)) return 0;
//			if(nLen>Size) nLen=Size;
//			memcpy(pBuf,Buffer,nLen);
//			SafeArrayUnaccessData(varBLOB.parray);
//		}
//	}
//	return nLen;
//}
DWORD GetLithSymbol(DWORD iLithID,DWORD nLen,char *pBuf)
{
	CFile fl=NULL;
	TCHAR szPath[MAX_PATH]={0};// 定义路径变量
    GetSystemDirectory(szPath, MAX_PATH);
	CString FileName;
	FileName.Format("%s\\PreLith.dat",szPath);
	if(fl.Open(FileName,CFile::modeRead))
	{
		fl.Seek(iLithID*2048-2048,CFile::begin);
		fl.Read(pBuf,2048);
		fl.Close();
	}
	return 2048;
}

void DrawHorLine(int x1,int x2,int y, CBitmapPen* pPen,CDC* pDC)
{         
	int x;
	int flag=1;
	int k = 0;
	if ((pPen->style == 0) || (pPen->width<= 0)) return; 
	CPen m_Pen(0,pPen->width,pPen->color);
	CPen* pOldPen = pDC->SelectObject(&m_Pen);
	if(pPen->style == 0x0ffffffff) 
	{
		if(pPen->m_LineNo<7)
		{
			pDC->MoveTo(x1,y); 
			pDC->LineTo(x2,y); 		
			pDC->SelectObject(pOldPen);
			return;
		}
		CPen Pen;
		Pen.CreatePen(0,1,pPen->color);
		pDC->SelectObject(&Pen);
		CBrush br;
		br.CreateSolidBrush(pPen->color);
		CBrush* pOldBr= (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		switch (pPen->m_LineNo)
		{
			case 7:  //-----○
				pDC->MoveTo(x1+3,y); 
				pDC->LineTo(x2-3,y);
				pDC->Ellipse(x2-6,y-2,x2-2,y+3);
				break;
			case 8:  //--○--
				for(x=x1;x<x2-9;x+=10)
				{
					pDC->MoveTo(x,y); 
					pDC->LineTo(x+6,y);
					pDC->Ellipse(x+6,y-2,x+10,y+3);
				}
				pDC->MoveTo(x,y); 
				pDC->LineTo(x+6,y);
				break;
			case 9:  //○-----
				pDC->MoveTo(x2-3,y); 
				pDC->LineTo(x1+5,y);
				pDC->Ellipse(x1+3,y-2,x1+7,y+3);
				break;
			case 10:  // ○○○○
				for(x=x1;x<x2-9;x+=10)
					pDC->Ellipse(x+6,y-2,x+10,y+3);
			    break;
			case 11:  // ●●●●
				pDC->SelectObject(&br);
				for(x=x1;x<x2-9;x+=10)
					pDC->Ellipse(x+6,y-2,x+10,y+3);
				break;
			case 12:  // □□□□□
				for(x=x1;x<x2-9;x+=10)
					pDC->Rectangle(x+6,y-2,x+10,y+3);
			    break;
			case 13:  // ■■■■■ 
				pDC->SelectObject(&br);
				for(x=x1;x<x2-9;x+=10)
					pDC->Rectangle(x+6,y-2,x+10,y+3);
			    break;
			default:break;
		}
		pDC->SelectObject(pOldBr);
		pDC->SelectObject(pOldPen);
		return; 
	}
	
	for (x=x1;x<=x2;x++)
	{                              
		if ((pPen->style >> (31-pPen->org.x)) & 1)
		{	
			if (flag==1) 
			{
				pDC->MoveTo(x,y);
				flag=0;
			}
		}	
		else
    	{                                        
    		if (flag == 0) 
    		{
	    		pDC->LineTo(x,y);
    			flag= 1;
    		}
    	}	                           
	if (pPen->org.x < pPen->len-1) pPen->org.x++; 
	else pPen->org.x = 0;   
	}    
	if (flag == 0) pDC->LineTo(x-1,y); 
	pDC->SelectObject(pOldPen);
}

void HWFillCurve(LPPOINT point1, LPPOINT point2, int nPointNum, int nLthNo,COLORREF nColor,
				 CPoint org, CDC* pDC, int nFillMode,COLORREF nBkColor)
{
	LPPOINT point,pl,pr,p1,p2;
	point = new POINT[nPointNum*2];
	int i,j,k,k1;
	if (nFillMode == FILLLEFTRIGHT) 
	{
		memcpy(point,point1,sizeof(POINT)*nPointNum);
		p1 = point+nPointNum; p2 = point2+nPointNum-1;
		for (j=0; j<nPointNum; j++)
		{
			*p1 = *p2; p1++; p2--;
		}
		LithFillPoly(point, 2*nPointNum,nLthNo,nColor,org,pDC,nBkColor);
	}
	else if ((nFillMode == FILLLEFT) || (nFillMode == FILLRIGHT))
	{
		if (nFillMode == FILLRIGHT) 
		{
			pl = point1; pr = point2;
		}
		else 
		{
			pl = point2; pr = point1;
		}
		k1 = 0;
		if ((*pl).x < (*pr).x) k = 1;
		else if ((*pl).x > (*pr).x) k = -1;
		else k = 0;
		pl++; pr++;
		for (i=1; i<nPointNum; i++)
		{
			if (k == -1) 
			{
				k1 = i;
				k = 1;
			}
			pl++; pr++;
		}
		if (k >= 0)
		{
			if (k1 == 0)
			{
				int kk = 0;
			}
			if ((i-k1) > 1)
			{
				memcpy(point,&point1[k1],(i-k1)*sizeof(POINT));
				p1 = point+i-k1; p2 = point2+i-1;
				for (j=0; j<i-k1; j++)
				{
					*p1 = *p2; p1++; p2--;
				}
				LithFillPoly(point, 2*(i-k1),nLthNo,nColor,org,pDC,nBkColor);
			}
		}	
	}
	delete point;
}

void FillCurve(LPPOINT point1, LPPOINT point2, int nPointNum, int nLthNo,COLORREF nColor,
			   CPoint org, CDC* pDC, int nFillMode,COLORREF nBkColor)
{
	LPPOINT point,pl,pr,p1,p2;
	point = new POINT[nPointNum*2];
	int i,j,k,k1;
	if (nFillMode == FILLLEFTRIGHT) 
	{
		memcpy(point,point1,sizeof(POINT)*nPointNum);
		p1 = point+nPointNum; p2 = point2+nPointNum-1;
		for (j=0; j<nPointNum; j++)
		{
			*p1 = *p2; p1++; p2--;
		}
		LithFillPoly(point, 2*nPointNum,nLthNo,nColor,org,pDC,nBkColor);
	}
	else if ((nFillMode == FILLLEFT) || (nFillMode == FILLRIGHT))
	{
		if (nFillMode == FILLRIGHT) 
		{
			pl = point1; pr = point2;
		}
		else 
		{
			pl = point2; pr = point1;
		}
		k1 = 0;
		if ((*pl).x < (*pr).x) k = 1;
		else if ((*pl).x > (*pr).x) k = -1;
		else k = 0;
		pl++; pr++;
		for (i=1; i<nPointNum; i++)
		{
			if ((*pl).x == (*pr).x)
			{
 				if ((k == 1) || ((k==0) && (i-k1>1)))
				{
					memcpy(point,&point1[k1],(i-k1+1)*sizeof(POINT));
					p1 = point+i-k1+1; p2 = point2+i;
					for (j=0; j<i-k1+1; j++)
					{
						*p1 = *p2; p1++; p2--;
					}
					LithFillPoly(point, 2*(i-k1+1),nLthNo,nColor,org,pDC,nBkColor);
				}	
				k1 = i;
				k = 0;
			}
			else if ((*pl).x > (*pr).x)
			{
				if (k >= 0)
				{
					if (i-k1 > 1)
					{
						memcpy(point,&point1[k1],(i-k1)*sizeof(POINT));
						p1 = point+i-k1; p2 = point2+i-1;
						for (j=0; j<i-k1; j++)
						{
							*p1 = *p2; p1++; p2--;
						}
						LithFillPoly(point, 2*(i-k1),nLthNo,nColor,org,pDC,nBkColor);
					}
				}	
				k = -1;
			}
			else 
			{
				if (k == -1) 
				{
					k1 = i;
					k = 1;
				}
			}
			pl++; pr++;
		}
		if (k >= 0)
		{
			if (k1 == 0)
			{
				int kk = 0;
			}
			if ((i-k1) > 1)
			{
				memcpy(point,&point1[k1],(i-k1)*sizeof(POINT));
				p1 = point+i-k1; p2 = point2+i-1;
				for (j=0; j<i-k1; j++)
				{
					*p1 = *p2; p1++; p2--;
				}
				LithFillPoly(point, 2*(i-k1),nLthNo,nColor,org,pDC,nBkColor);
			}
		}	
	}
	delete point;
}

void FillCurve(int x, LPPOINT point1, int nPointNum, int nLthNo,COLORREF nColor,
			   CPoint org, CDC* pDC, int nFillMode,COLORREF nBkColor)
{
	POINT p1,p2;
	LPPOINT p,point;
	point = new POINT[nPointNum+2];
	int i,k,k1;
	p1.x = p2.x = x;
	p1.y = point1[0].y; p2.y = point1[nPointNum-1].y;
	if (nFillMode == FILLLEFTRIGHT) 
	{
		memcpy(point,point1,sizeof(POINT)*nPointNum);
		point[nPointNum] = p2;
		point[nPointNum+1] = p1;
		LithFillPoly(point, nPointNum+2, nLthNo,nColor,org,pDC,nBkColor);
	}
	else if (nFillMode == FILLLEFT) 
	{
		k1 = 0;
		p = point1;
		if (x < p->x) k = -1;
		else if (x > p->x) k = 1;
		else k = 0;
		p++;
		for (i=1; i<nPointNum; i++)
		{
			if (p->x == x)
			{
 				if ((k == 1) || ((k==0) && (i-k1>1)))
				{
					p2.y = point1[i].y;  
					memcpy(point,&point1[k1],(i-k1+1)*sizeof(POINT));
					point[i-k1+1] = p2;
					point[i-k1+2] = p1;
					LithFillPoly(point, i-k1+3,nLthNo,nColor,org,pDC,nBkColor);
				}
				k = 0; k1 = i; p1.y = p->y;
			}
			if (p->x > x)
			{
				if (k >= 0)
				{
					if ((i-k1)>1)				
					{
						p2.y = point1[i-1].y;  
						memcpy(point,&point1[k1],(i-k1)*sizeof(POINT));
						point[i-k1] = p2;
						point[i-k1+1] = p1;
						LithFillPoly(point, i-k1+2,nLthNo,nColor,org,pDC,nBkColor);
					}
				}	
				k = -1;
			}
			else 
			{
				if (k == -1) {k1 = i; p1.y = p->y; k=1;}
			}
			p ++;
		}
		if (k >= 0)
		{
			if ((i-k1) > 1)
			{
				p2.y = point1[i-1].y;  
				memcpy(point,&point1[k1],(i-k1)*sizeof(POINT));
				point[i-k1] = p2;
				point[i-k1+1] = p1;
				LithFillPoly(point, i-k1+2,nLthNo,nColor,org,pDC,nBkColor);
			}
		}	
	}
	else if (nFillMode == FILLRIGHT) 
	{
		k1 = 0;
		p = point1;
		if (x < p->x) 
		{		   
			k = 1;
		}
		else if (x > p->x) k = -1;
		else k = 0;
		p++;
		for (i=1; i<nPointNum; i++)
		{
			if (p->x == x)
			{
 				if ((k == 1) || ((k==0) && (i-k1>1)))
				{
					p2.y = point1[i].y;  
					memcpy(point,&point1[k1],sizeof(POINT)*i-k1+1);
					point[i-k1+1] = p2;
					point[i-k1+2] = p1;
					LithFillPoly(point, i-k1+3,nLthNo,nColor,org,pDC,nBkColor);
				}	
				k = -1;	k1 = i;p1.y = p->y;
			}
			if (p->x < x)
			{
				if (k >= 0)
				{
					if (i-k1>1)
					{
						p2.y = point1[i-1].y;  
						memcpy(point,&point1[k1],sizeof(POINT)*i-k1);
						point[i-k1] = p2;
						point[i-k1+1] = p1;
						LithFillPoly(point, i-k1+2,nLthNo,nColor,org,pDC,nBkColor);
					}
				}	
				k = -1;
			}
			else 
			{
				if (k == -1) {k1 = i ; p1.y = p->y; k=0;}
			}
			p ++;
		}
		if (k >= 0) 
		{
			if ((i-k1) > 1)
			{
				p2.y = point1[i-1].y;  
				memcpy(point,&point1[k1],(i-k1)*sizeof(POINT));
				point[i-k1] = p2;
				point[i-k1+1] = p1;
				LithFillPoly(point, i-k1+2,nLthNo,nColor,org,pDC,nBkColor);
			}
		}	
	}
	delete point;
}

void LithFillPoly(LPPOINT point, int nPointNum,int nLthNo,COLORREF nColor,
				  CPoint org,CDC* pDC,COLORREF nBkColor)
{	
	if (nPointNum < 3) return;	
	if(nLthNo == 1)
	{
		CBrush br(nColor);//nBkColor
		CBrush* pOldBrush = pDC->SelectObject(&br);
		pDC->Polygon(point,nPointNum);
		pDC->SelectObject(pOldBrush);
		return;
	}
	int nSaveDC = pDC->SaveDC();	
	if (nLthNo == 0)
	{
		CBrush br(nBkColor);
		CBrush* pOldBrush = pDC->SelectObject(&br);
		pDC->SetROP2(R2_NOTXORPEN);//
		
		pDC->BeginPath();
		pDC->PolyBezier(point,nPointNum);
		pDC->EndPath();
		pDC->FillPath();

		pDC->SelectObject(pOldBrush);
		pDC->RestoreDC(nSaveDC);
		return;
	}
//	read Lith Color data
	nLthNo--;
	short *pLith=new short[1024];
	if(GetLithSymbol(nLthNo,2048,(char *)pLith)<1)
	{
		delete pLith;
		return;
	}		
	int m,n;
//  Draw Lith
	int cx = (pDC->GetDeviceCaps(LOGPIXELSX)+50) / 100;
	int cy = (pDC->GetDeviceCaps(LOGPIXELSY)+50) / 100;
	if (cx < 1) cx = 1;
	if (cy < 1) cy = 1;
	
	int xlen = cx*32; int ylen = cy*32;


	pDC->LPtoDP(&org,1);
	LPPOINT point3 = new POINT[nPointNum];
	memcpy(point3,point,sizeof(POINT)*nPointNum);
	pDC->LPtoDP(point3,nPointNum);

	pDC->SetViewportOrg(0, 0);
	pDC->SetWindowOrg(0,0);
	pDC->SetMapMode(MM_TEXT);
	
	CRgn rgn; rgn.CreatePolygonRgn(point3,nPointNum,ALTERNATE);
	pDC->SelectClipRgn(&rgn);

	CRect rect;
	rgn.GetRgnBox(&rect);
	pDC->DPtoLP(rect);
	if (!rect.IsRectEmpty()) 
	{
		CPoint point1(rect.TopLeft());
		CPoint point2(rect.BottomRight());
		point1.y = (point1.y - org.y) / ylen *ylen + org.y;
		if (point1.y > rect.top) point1.y -= ylen;
		point2.y = (point2.y - org.y) / ylen * ylen + org.y;
		if (point2.y < rect.bottom) point2.y += ylen;
		point1.x = (point1.x - org.x) / xlen * xlen + org.x;
		if (point1.x > rect.left) point1.x -= xlen;
		point2.x = (point2.x - org.x) / xlen * xlen + org.x;
		if (point2.x < rect.right) point2.x += xlen;			
		CDC MemDC;
		MemDC.CreateCompatibleDC(NULL);
		CBitmap *bmp=new CBitmap;
		int Dx=point2.x-point1.x;
		bmp->CreateCompatibleBitmap(pDC,Dx,ylen);
		CBitmap* pOldBitmap =MemDC.SelectObject(bmp);
		MemDC.FillSolidRect(0,0,Dx,ylen,nBkColor);
		for(n=0;n<32;n++)
			for(m=0;m<32;m++)
				if(pLith[n*32+m]!=0)
					MemDC.FillSolidRect(n*cx,m*cy,cx,cy,nColor);	

		for (int x=0; x<Dx; x+=xlen)
			MemDC.BitBlt(x, 0, xlen, ylen, &MemDC, 0, 0,SRCCOPY);
		for (int y=point1.y; y<point2.y; y+=ylen)
			pDC->BitBlt(point1.x, y, Dx, ylen, &MemDC, 0, 0,SRCAND);//SRCCOPY

		MemDC.SelectObject(pOldBitmap);
		MemDC.DeleteDC ();
		delete bmp;
		bmp=NULL;
	}
	delete point3;
	delete pLith;
	pDC->RestoreDC(nSaveDC);
}

void LithFillBox(CRect rect, int nLthNo,COLORREF nColor,CPoint org,CDC* pDC1,COLORREF nBkColor)
{
	LithFillBox(rect.left,rect.top,rect.right,rect.bottom,nLthNo,nColor,org,pDC1,nBkColor);
}

void LithFillBox(int xl, int yt, int xr, int yb, int nLthNo,COLORREF nColor,
				 CPoint org,CDC* pDC,COLORREF nBkColor)
{
	if(nLthNo == 1)
	{				
		CBrush br(nColor);
		pDC->FillRect(CRect(xl,yt,xr,yb),&br);
		return;
	}
	int nSaveDC = pDC->SaveDC();	
	if (nLthNo == 0)
	{
		CBrush br(nBkColor);
		CBrush* pOldBrush = pDC->SelectObject(&br);
		pDC->SetROP2(R2_NOTXORPEN);//
		pDC->Rectangle(xl,yt,xr,yb);
		pDC->SelectObject(pOldBrush);
		pDC->RestoreDC(nSaveDC);
		return;
	}

//	read Lith Color data
	nLthNo--;
	short *pLith=new short[1024];
	if(GetLithSymbol(nLthNo,2048,(char *)pLith)<1)
	{
		delete pLith;
		return;
	}		
	int m,n;
		
//  Draw Lith
	int cx = (pDC->GetDeviceCaps(LOGPIXELSX)+50)/100;
	int cy = (pDC->GetDeviceCaps(LOGPIXELSY)+50)/100;
	if (cx < 1) cx = 1;
	if (cy < 1) cy = 1;
		
	int xlen = cx*32; int ylen = cy*32;
	CPoint point1(xl,yt);
	CPoint point2(xr,yb);
	CRect rect(xl,yt,xr,yb);
	pDC->LPtoDP(&org,1);
	pDC->LPtoDP(&point1,1);
	pDC->LPtoDP(&point2,1);
	pDC->LPtoDP(rect);
	
	pDC->SetViewportOrg(0, 0);
	pDC->SetWindowOrg(0,0);
	pDC->SetMapMode(MM_TEXT);
	CRgn rgn; 
	rgn.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
	pDC->SelectClipRgn(&rgn);
	pDC->DPtoLP(rect);
	point1.y = (point1.y - org.y) / ylen * ylen + org.y;
	if (point1.y > rect.top) point1.y -= ylen;
	point2.y = (point2.y - org.y) / ylen * ylen + org.y;
	if (point2.y < rect.bottom) point2.y += ylen;
	point1.x = (point1.x - org.x) / xlen * xlen + org.x;
	if (point1.x > rect.left) point1.x -= xlen;
	point2.x = (point2.x - org.x) / xlen * xlen + org.x;
	if (point2.x < rect.right) point2.x += xlen;
	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap bmp;
	int Dx=point2.x-point1.x;
	bmp.CreateCompatibleBitmap(pDC,Dx,ylen);
	CBitmap* pOldBitmap =MemDC.SelectObject(&bmp);
	MemDC.FillSolidRect(0,0,Dx,ylen,nBkColor);
	for(n=0;n<32;n++)
		for(m=0;m<32;m++)
			if(pLith[n*32+m]!=0)
				MemDC.FillSolidRect(n*cx,m*cy,cx,cy,nColor);	

	for (int x=0; x<Dx; x+=xlen)
		MemDC.BitBlt(x, 0, xlen, ylen, &MemDC, 0, 0,SRCCOPY);
	for (int y=point1.y; y<point2.y; y+=ylen)
		pDC->BitBlt(point1.x, y, Dx, ylen, &MemDC, 0, 0,SRCCOPY);

	delete pLith;
	MemDC.SelectObject(pOldBitmap);
	MemDC.DeleteDC ();
	bmp.DeleteObject();
	pDC->RestoreDC(nSaveDC);
}

void FillRoundRect(CRect rect, CPoint roundNess,int nLthNo,COLORREF nColor, CPoint org, CDC* pDC1,COLORREF nBkColor)
{
	FillRoundRect(rect.left,rect.top,rect.right,rect.bottom,roundNess,nLthNo,nColor, org,pDC1,nBkColor);
}

void FillRoundRect(int xl, int yt, int xr, int yb, CPoint roundNess, int nLthNo,COLORREF nColor,
				   CPoint org, CDC* pDC,COLORREF nBkColor)
{
	if (nLthNo == 1)
	{
		CBrush br(nColor);
		CBrush* pOldBrush = pDC->SelectObject(&br);
		pDC->RoundRect(xl,yt,xr,yb,roundNess.x,roundNess.y);
		pDC->SelectObject(pOldBrush);
		return;
	}
	int nSaveDC = pDC->SaveDC();	
	if (nLthNo == 0)
	{
		CBrush br(nBkColor);
		CBrush* pOldBrush = pDC->SelectObject(&br);
		pDC->SetROP2(R2_NOTXORPEN);//
		pDC->RoundRect(xl,yt,xr,yb,roundNess.x,roundNess.y);
		pDC->SelectObject(pOldBrush);
		pDC->RestoreDC(nSaveDC);
		return;
	}
//	read Lith Color data
	nLthNo--;
	short *pLith=new short[1024];
	if(GetLithSymbol(nLthNo,2048,(char *)pLith)<1)
	{
		delete pLith;
		return;
	}		
	int m,n;
		
//  Draw Lith
	int cx = (pDC->GetDeviceCaps(LOGPIXELSX)+50) / 100;
	int cy = (pDC->GetDeviceCaps(LOGPIXELSY)+50) / 100;
	if (cx < 1) cx = 1;
	if (cy < 1) cy = 1;
	
	int xlen = cx*32; int ylen = cy*32;

	CPoint point1(xl,yt);
	CPoint point2(xr,yb);
	CRect rect(xl,yt,xr,yb);
	pDC->LPtoDP(&org,1);
	pDC->LPtoDP(&point1,1);
	pDC->LPtoDP(&point2,1);
	pDC->LPtoDP(rect);

	pDC->SetViewportOrg(0, 0);
	pDC->SetWindowOrg(0,0);
	pDC->SetMapMode(MM_TEXT);

	CRgn rgn; rgn.CreateRoundRectRgn(rect.left,rect.top,rect.right,rect.bottom,roundNess.x,roundNess.y);
	pDC->SelectClipRgn(&rgn);
	pDC->DPtoLP(rect);
	
	point1.y = (point1.y - org.y) / ylen * ylen + org.y;
	if (point1.y > rect.top) point1.y -= ylen;
	point2.y = (point2.y - org.y) / ylen * ylen + org.y;
	if (point2.y < rect.bottom) point2.y += ylen;
	point1.x = (point1.x - org.x) / xlen * xlen + org.x;
	if (point1.x > rect.left) point1.x -= xlen;
	point2.x = (point2.x - org.x) / xlen * xlen + org.x;
	if (point2.x < rect.right) point2.x += xlen;
	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap bmp;
	int Dx=point2.x-point1.x;
	bmp.CreateCompatibleBitmap(pDC,Dx,ylen);
	CBitmap* pOldBitmap =MemDC.SelectObject(&bmp);
	MemDC.FillSolidRect(0,0,Dx,ylen,nBkColor);
	for(n=0;n<32;n++)
		for(m=0;m<32;m++)
			if(pLith[n*32+m]!=0)
				MemDC.FillSolidRect(n*cx,m*cy,cx,cy,nColor);	

	for (int x=0; x<Dx; x+=xlen)
		MemDC.BitBlt(x, 0, xlen, ylen, &MemDC, 0, 0,SRCCOPY);
	for (int y=point1.y; y<point2.y; y+=ylen)
		pDC->BitBlt(point1.x, y, Dx, ylen, &MemDC, 0, 0,SRCCOPY);

	delete pLith;
	MemDC.SelectObject(pOldBitmap);
	MemDC.DeleteDC ();
	bmp.DeleteObject();
	pDC->RestoreDC(nSaveDC);
}

void FillEllipse(CRect rect, int nLthNo,COLORREF nColor, CPoint org, CDC* pDC1,COLORREF nBkColor)
{
	FillEllipse(rect.left,rect.top,rect.right,rect.bottom,nLthNo,nColor,org,pDC1,nBkColor);
}

void FillEllipse(int xl, int yt, int xr, int yb, int nLthNo,COLORREF nColor, 
				 CPoint org, CDC* pDC,COLORREF nBkColor)
{
	if(nLthNo== 1)
	{
		CBrush br(nColor);
		CBrush* pOldBrush = pDC->SelectObject(&br);
		pDC->Ellipse(xl,yt,xr,yb);
		pDC->SelectObject(pOldBrush);
		return;
	}
	int nSaveDC = pDC->SaveDC();	
	if (nLthNo == 0)
	{
		CBrush br(nBkColor);
		CBrush* pOldBrush = pDC->SelectObject(&br);
		pDC->SetROP2(R2_NOTXORPEN);//
		pDC->Ellipse(xl,yt,xr,yb);
		pDC->SelectObject(pOldBrush);
		pDC->RestoreDC(nSaveDC);
		return;
	}

//	read Lith Color data
	nLthNo--;
	short *pLith=new short[1024];
	if(GetLithSymbol(nLthNo,2048,(char *)pLith)<1)
	{
		delete pLith;
		return;
	}		
	int m,n;
		
//  Draw Lith
	int cx = (pDC->GetDeviceCaps(LOGPIXELSX)+50) / 100;
	int cy = (pDC->GetDeviceCaps(LOGPIXELSY)+50) / 100;
	if (cx < 1) cx = 1;
	if (cy < 1) cy = 1;
	
	int xlen = cx*32; int ylen = cy*32;

	CPoint point1(xl,yt);
	CPoint point2(xr,yb);
	CRect rect(xl,yt,xr,yb);
	pDC->LPtoDP(&org,1);
	pDC->LPtoDP(&point1,1);
	pDC->LPtoDP(&point2,1);
	pDC->LPtoDP(rect);

	pDC->SetViewportOrg(0, 0);
	pDC->SetWindowOrg(0,0);
	pDC->SetMapMode(MM_TEXT);

	CRgn rgn; rgn.CreateEllipticRgn(rect.left,rect.top,rect.right,rect.bottom);
	pDC->SelectClipRgn(&rgn);
	pDC->DPtoLP(rect);
	
	point1.y = (point1.y - org.y) / ylen * ylen + org.y;
	if (point1.y > rect.top) point1.y -= ylen;
	point2.y = (point2.y - org.y) / ylen * ylen + org.y;
	if (point2.y < rect.bottom) point2.y += ylen;
	point1.x = (point1.x - org.x) / xlen * xlen + org.x;
	if (point1.x > rect.left) point1.x -= xlen;
	point2.x = (point2.x - org.x) / xlen * xlen + org.x;
	if (point2.x < rect.right) point2.x += xlen;
	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap bmp;
	int Dx=point2.x-point1.x;
	bmp.CreateCompatibleBitmap(pDC,Dx,ylen);
	CBitmap* pOldBitmap =MemDC.SelectObject(&bmp);
	MemDC.FillSolidRect(0,0,Dx,ylen,nBkColor);
	for(n=0;n<32;n++)
		for(m=0;m<32;m++)
			if(pLith[n*32+m]!=0)
				MemDC.FillSolidRect(n*cx,m*cy,cx,cy,nColor);	

	for (int x=0; x<Dx; x+=xlen)
		MemDC.BitBlt(x, 0, xlen, ylen, &MemDC, 0, 0,SRCCOPY);
	for (int y=point1.y; y<point2.y; y+=ylen)
		pDC->BitBlt(point1.x, y, Dx, ylen, &MemDC, 0, 0,SRCCOPY);

	delete pLith;
	MemDC.SelectObject(pOldBitmap);
	MemDC.DeleteDC ();
	bmp.DeleteObject();
	pDC->RestoreDC(nSaveDC);
}


CBitmap* ZoomBitmap(CBitmap* pBitmap,int cx,int cy)
{
	if (cx < 1) cx = 1;
	if (cy < 1) cy = 1;
	CBitmap* m_pBitmap = new CBitmap;
	BITMAP bm;
	if (pBitmap->GetObject(sizeof(bm),&bm) == 0) return pBitmap;
	if (bm.bmPlanes*bm.bmBitsPixel != 1) return pBitmap;
	CDC dc1,dc2;
	dc1.CreateCompatibleDC(NULL);
	dc2.CreateCompatibleDC(NULL);
	CBitmap* pOldBitmap1 = dc1.SelectObject(pBitmap);
	m_pBitmap->CreateBitmap(bm.bmWidth*cx, bm.bmHeight*cy,1,1,NULL);
	CBitmap* pOldBitmap2 = dc2.SelectObject(m_pBitmap);
	dc2.StretchBlt(0, 0, bm.bmWidth*cx, bm.bmHeight*cy,&dc1, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	dc1.SelectObject(pOldBitmap1);
	dc2.SelectObject(pOldBitmap2);
	delete pBitmap;
	return m_pBitmap;
}

CSize GetCoreOGSize(int no)
{
	CSize size(0,0);
	if ((no>0) || (no<257))
	{
		char s1[20],s2[10];  
		CBitmap* pBitmap= new CBitmap;
		BITMAP bm;        
		strcpy(s1,"COREOG");
		ltoa(no,s2,10);
		strcat(s1,s2);  
		if (pBitmap->LoadBitmap(s1))
		{                 
			pBitmap->GetObject(sizeof(bm),&bm);
			size.cx = bm.bmWidth;
			size.cy = bm.bmHeight;
		}         
		delete pBitmap;
	}
	return size;
}

CSize GetCoreLthSize(int no)
{
	CSize size(0,0);
	if ((no>0) || (no<257))
	{
		char s1[20],s2[10];  
		CBitmap* pBitmap= new CBitmap;
		BITMAP bm;        
		strcpy(s1,"CORLTH");
		ltoa(no,s2,10);
		strcat(s1,s2);  
		if (pBitmap->LoadBitmap(s1))
		{                 
			pBitmap->GetObject(sizeof(bm),&bm);
			size.cx = bm.bmWidth;
			size.cy = bm.bmHeight;
		}         
		delete pBitmap;
	}
	return size;
}

CSize GetCoreSize(int m_OG, int m_Lth)
{
	CSize size1,size2,size;
	size1 = GetCoreOGSize(m_OG);
	size2 = GetCoreLthSize(m_Lth);
	size.cy = size1.cy + size2.cy;
	if ((size1.cy > 0) && (size2.cy > 0)) size.cy -= 1;
	size.cx = (size1.cx > size2.cx) ? size1.cx:size2.cx;
	return size;
}


void DrawCoreOG(CDC* pDC,CPoint pt,int no)
{
	if (no<1 || (no>256)) return;
	char s1[20],s2[10];  
	int nSaveDC = pDC->SaveDC();
	pDC->LPtoDP(&pt);           
	pDC->SetViewportOrg(0, 0);
	pDC->SetWindowOrg(0,0);
	pDC->SetMapMode(MM_TEXT);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap;
	CBitmap* pBitmap = new CBitmap;
	BITMAP bm;
	pDC->SetTextColor(0x8000);

	CSize size;
	size.cx = (pDC->GetDeviceCaps(LOGPIXELSX)+50) / 100;
	size.cy = (pDC->GetDeviceCaps(LOGPIXELSY)+50) / 100;
	if (size.cx < 1) size.cx = 1;
	if (size.cy < 1) size.cy = 1;

	strcpy(s1,"COREOG");
	ltoa(no,s2,10);
	strcat(s1,s2);  
	if (pBitmap->LoadBitmap(s1))
	{                 
		if ((size.cx > 1) || (size.cy > 1)) pBitmap = ZoomBitmap(pBitmap,size.cx,size.cy);
		pBitmap->GetObject(sizeof(bm),&bm);
		pOldBitmap = dc.SelectObject(pBitmap);
		pDC->BitBlt(pt.x, pt.y, bm.bmWidth, bm.bmHeight, &dc, 0,0, SRCCOPY);
		dc.SelectObject(pOldBitmap);
		pBitmap->DeleteObject();
	}                         
	delete(pBitmap);
	pDC->RestoreDC(nSaveDC);
}

void DrawCoreLth(CDC* pDC,CPoint pt,int no)
{
	if (no<1 || (no>256)) return;
	char s1[20],s2[10];  
	int nSaveDC = pDC->SaveDC();
	pDC->LPtoDP(&pt);           
	pDC->SetViewportOrg(0, 0);
	pDC->SetWindowOrg(0,0);
	pDC->SetMapMode(MM_TEXT);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap;
	CBitmap* pBitmap = new CBitmap;
	BITMAP bm;
	pDC->SetTextColor(0x8000);

	CSize size;
	size.cx = (pDC->GetDeviceCaps(LOGPIXELSX)+50) / 100;
	size.cy = (pDC->GetDeviceCaps(LOGPIXELSY)+50) / 100;
	if (size.cx < 1) size.cx = 1;
	if (size.cy < 1) size.cy = 1;

	strcpy(s1,"CORLTH");
	ltoa(no,s2,10);
	strcat(s1,s2);  
	if (pBitmap->LoadBitmap(s1))
	{                 
		if ((size.cx > 1) || (size.cy > 1)) pBitmap = ZoomBitmap(pBitmap,size.cx,size.cy);
		pBitmap->GetObject(sizeof(bm),&bm);
		pOldBitmap = dc.SelectObject(pBitmap);        
		pDC->BitBlt(pt.x, pt.y, bm.bmWidth, bm.bmHeight, &dc, 0, 0, SRCCOPY);
		dc.SelectObject(pOldBitmap);
		pBitmap->DeleteObject();
	}                         
	delete(pBitmap);
	pDC->RestoreDC(nSaveDC);
}

void DrawCore(CDC* pDC1,CPoint pt,int m_OG,int m_Lth)
{
	char s1[20],s2[10];  
	CDC dc1;
	dc1.Attach(pDC1->m_hDC);
	CDC* pDC = &dc1;
	int nSaveDC = pDC->SaveDC();
	pDC->LPtoDP(&pt);           
	pDC->SetViewportOrg(0, 0);
	pDC->SetWindowOrg(0,0);
	pDC->SetMapMode(MM_TEXT);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap;
	CBitmap* pBitmap = new CBitmap;
	BITMAP bm;
	pDC->SetTextColor(0x8000);

	CSize size;
	size.cx = (::GetDeviceCaps(pDC->m_hDC,LOGPIXELSX)+50) / 100;
	size.cy = (::GetDeviceCaps(pDC->m_hDC,LOGPIXELSY)+50) / 100;
	if (size.cx < 1) size.cx = 1;
	if (size.cy < 1) size.cy = 1;

	strcpy(s1,"COREOG");
	ltoa(m_OG,s2,10);
	strcat(s1,s2);  
	if (pBitmap->LoadBitmap(s1))
	{                 
		if ((size.cx > 1) || (size.cy > 1)) pBitmap = ZoomBitmap(pBitmap,size.cx,size.cy);
		pBitmap->GetObject(sizeof(bm),&bm);
		pOldBitmap = dc.SelectObject(pBitmap);
		pDC->BitBlt(pt.x, pt.y-bm.bmHeight+1, bm.bmWidth, bm.bmHeight, &dc, 0,0, SRCCOPY);
		dc.SelectObject(pOldBitmap);
		pBitmap->DeleteObject();
	}                         
	strcpy(s1,"CORLTH");
	ltoa(m_Lth,s2,10);
	strcat(s1,s2);  
	if (pBitmap->LoadBitmap(s1))
	{                 
		if ((size.cx > 1) || (size.cy > 1)) pBitmap = ZoomBitmap(pBitmap,size.cx,size.cy);
		pBitmap->GetObject(sizeof(bm),&bm);
		pOldBitmap = dc.SelectObject(pBitmap);        
		pDC->BitBlt(pt.x, pt.y, bm.bmWidth, bm.bmHeight, &dc, 0, 0, SRCCOPY);
		dc.SelectObject(pOldBitmap);
		pBitmap->DeleteObject();
	}                         
	delete(pBitmap);
	pDC->RestoreDC(nSaveDC);
	pDC->Detach();
}


CBitmapPen::CBitmapPen()
{
	len = 0;
	org.x = 0; org.y = 0;
	color = 0x0;    
	width = 0;
	m_LineNo=1;
}

CBitmapPen::CBitmapPen(CBitmap* pBitmap,int nWidth,COLORREF nColor)
{
	CreatePen(pBitmap,nWidth,nColor);
}

void CBitmapPen::SetWidth(int nWidth)
{                                     
	width = nWidth;
}

int CBitmapPen::GetWidth()
{                                     
	return width;
}

void CBitmapPen::SetColor(COLORREF nColor)
{                                     
	color = nColor;
}

COLORREF CBitmapPen::GetColor()
{                                     
	return color;
}

void CBitmapPen::SetPenOrg(const CPoint& point)
{
	org = point;
}               

CPoint CBitmapPen::GetPenOrg()
{
	return org;
}

void CBitmapPen::CreatePen(CBitmap* pBitmap, int nWidth, COLORREF nColor)
{
	union _buffer
	{
		char ch[4];
		unsigned long ll;
	}buffer[32];
	BITMAP bm;      
	if (pBitmap == NULL) return;
	color = nColor;
	width = nWidth;
	if (width < 0) width = 0;
	pBitmap->GetObject(sizeof(bm),&bm);
	org.x = 0; org.y = 0;
	len = bm.bmWidth;
	if (len > 32) len = 32;
	pBitmap->GetBitmapBits(bm.bmWidth*bm.bmPlanes,buffer);
	char tmp = buffer[0].ch[3]; buffer[0].ch[3] = buffer[0].ch[0]; buffer[0].ch[0]= tmp;
	tmp = buffer[0].ch[2]; buffer[0].ch[2] = buffer[0].ch[1]; buffer[0].ch[1] = tmp;
	style = buffer[0].ll ^ 0xffffffff;
	pBitmap->DeleteObject();
}                         

CLinePen::CLinePen()
{
	m_LineNo = 0;
}

CLinePen::CLinePen(int nLineNo, int nWidth,COLORREF nColor)
{          
	CreateLinePen(nLineNo,nWidth,nColor);
}   

void CLinePen::CreateLinePen(int nLineNo,int nWidth,COLORREF nColor)
{          
	char s1[20],s2[10];           
	strcpy(s1,"LINE");
	CBitmap Bitmap ;
	ltoa(nLineNo,s2,10);
	strcat(s1,s2);  
	if (Bitmap.LoadBitmap(s1))
	{                 
		CreatePen(&Bitmap,nWidth,nColor);
		m_LineNo = nLineNo;
	}	                   
}   

void CLinePen::SetLineNo(int nLineNo)
{
	CreateLinePen(nLineNo,GetWidth(),GetColor());
}                    

CLinePen::GetLineNo()
{
	return m_LineNo;
}

#include <math.h>
#define DEFAULT_BOX_VALUE -3
#define CUSTOM_BOX_VALUE  -2
#define INVALID_Color    -1

#define MAX_ColorS			100
#define NUM_COLUMNS			10


ColorTableEntry CCJColorPopup::m_crColors[] = 
{
	// row 0
	{ RGB(128,   0,  64), _T("Light Maroon")}, 
	{ RGB(139,   0,   0), _T("Dark Red")},           
	{ RGB(255,   0,   0), _T("Red")},                
	{ RGB(255,   0, 128), _T("Deep Pink") },          
	{ RGB(255,   0, 255), _T("Magenta") },            
	{ RGB(255, 128, 192), _T("Pink") },               
	{ RGB(255, 128, 255), _T("Light Pink") },         
	{ RGB(255, 208, 232), _T("Very Light Pink") },    
	{ RGB(255, 140,   0), _T("Dark Orange") },        
	{ RGB(255, 165,   0), _T("Orange") }, 
	
	// row 1            
	{ RGB(255, 128, 128), _T("Light Salmon") }, 
	{ RGB(255, 224, 160), _T("Light Orange") }, 
	{ RGB(255, 215,   0), _T("Gold") },               
	{ RGB(255, 255,   0), _T("Yellow") },             
	{ RGB(255, 255, 128), _T("Desert Yellow") },      
	{ RGB(255, 255, 184), _T("Pale Yellow") },        
	{ RGB(  0,  64,   0), _T("Dark Green") },         
	{ RGB(  0,  64,  64), _T("Medium Green") }, 
	{ RGB(  0, 128,   0), _T("Green") },              
	{ RGB(  0, 128,  64), _T("Dark Teal") }, 
	
	// row 2         
	{ RGB(128, 128,   0), _T("Olive") },              
	{ RGB(  0, 128, 128), _T("Teal") },               
	{ RGB( 64, 128, 128), _T("Turquoise") },          
	{ RGB(  0, 255,  64), _T("Medium Spring Green") },	
	{ RGB(128, 255,   0), _T("Mint Green") },         
	{ RGB(  0, 255,   0), _T("Lime") },               
	{ RGB(  0, 255, 128), _T("Spring Green") }, 
	{ RGB(152, 251, 152), _T("Pale Green") },         
	{ RGB(127, 255, 212), _T("Aquamarine") },         
	{ RGB(192, 220, 192), _T("Very Pale Green") }, 
	
	// row 3   
	{ RGB( 64,   0,  64), _T("Deep Purple") },        
	{ RGB(128,   0, 128), _T("Purple") },             	
	{ RGB( 64,   0, 128), _T("Deep Violet") },        
	{ RGB(  0,   0,  64), _T("Midnight Blue") },      
	{ RGB(  0,   0, 160), _T("Dark Blue") },          
	{ RGB(  0,   0, 128), _T("Navy") },               
	{ RGB(128,   0, 255), _T("Violet") },             
	{ RGB(  0,   0, 255), _T("Blue") },               
	{ RGB(  0,  64, 128), _T("Medium Blue") },        
	{ RGB(  0, 128, 255), _T("Light Sky Blue") },     

	// row 4	
	{ RGB(100, 149, 237), _T("Cornflower Blue") },    
	{ RGB(128, 128, 192), _T("Sky Blue") },           
	{ RGB(  0, 128, 192), _T("Dark Cyan") },          
	{ RGB(128, 128, 255), _T("Medium Slate Blue") },  
	{ RGB(128, 255, 255), _T("Light Blue") },         
	{ RGB(  0, 255, 255), _T("Cyan") },               
	{ RGB(176, 224, 230), _T("Powder Blue") },        
	{ RGB(224, 255, 255), _T("Light Cyan") },         	
	{ RGB(  0,   0,   0), _T("Black") },              
	{ RGB(128, 128, 128), _T("Gray") }, 

	// row 5              
	{ RGB(152, 152, 152), _T("Gray 40%") },           
	{ RGB(192, 192, 192), _T("Silver") },             
	{ RGB(211, 211, 211), _T("Light Gray") },         
	{ RGB(245, 245, 220), _T("Beige") },              
	{ RGB(255, 251, 240), _T("Floral White") }, 
	{ RGB(255, 255, 255), _T("White") },              	
	{ RGB(  0,   0,   0), _T("Window") },             
	{ RGB(  0,   0,   0), _T("WindowText") },         
	{ RGB(  0,   0,   0), _T("InfoBk") },             
	{ RGB(  0,   0,   0), _T("InfoText") },

	// row 6           
	{ RGB(  0,   0,   0), _T("ActiveCaption") },      
	{ RGB(  0,   0,   0), _T("CaptionText") },        
	{ RGB(  0,   0,   0), _T("InactiveCaption") },    
	{ RGB(  0,   0,   0), _T("InactiveCaptionText") },
	{ RGB(  0,   0,   0), _T("BtnFace") },            
	{ RGB(  0,   0,   0), _T("BtnText") },            
	{ RGB(  0,   0,   0), _T("BtnShadow") },          
	{ RGB(  0,   0,   0), _T("BtnHighlight") },   
	{ RGB(  0,   0,   0), _T("Highlight") },     
	{ RGB(  0,   0,   0), _T("HighlightText") },

	// row 7 - window system colors 
	{ RGB(  0,   0,   0), _T("3DLight") },       
	{ RGB(  0,   0,   0), _T("Desktop") },	
	{ RGB(192, 192, 192), _T("1") },                   
	{ RGB(192, 192, 192), _T("2") },                   
	{ RGB(192, 192, 192), _T("3") },                   
	{ RGB(192, 192, 192), _T("4") },                   
	{ RGB(192, 192, 192), _T("5") },                   
	{ RGB(192, 192, 192), _T("6") },                   
	{ RGB(192, 192, 192), _T("7") },                   
	{ RGB(192, 192, 192), _T("8") }, 
	
	// row 8 - window system colors, row #2	
	{ RGB(192, 192, 192), _T("9") },                   
	{ RGB(192, 192, 192), _T("10") },                  
	{ RGB(192, 192, 192), _T("11") },                  
	{ RGB(192, 192, 192), _T("12") },                  
	{ RGB(192, 192, 192), _T("13") },                  
	{ RGB(192, 192, 192), _T("14") },                  
	{ RGB(192, 192, 192), _T("15") },                  
	{ RGB(192, 192, 192), _T("16") }, 
	{ RGB(192, 192, 192), _T("15") },                  
	{ RGB(192, 192, 192), _T("16") }, 
	// row 9 - divider line  
	{ RGB(192, 192, 192), _T("17") },                   
	{ RGB(192, 192, 192), _T("18") },                  
	{ RGB(192, 192, 192), _T("19") },                  
	{ RGB(192, 192, 192), _T("20") },                  
	{ RGB(192, 192, 192), _T("21") },                  
	{ RGB(192, 192, 192), _T("22") },                  
	{ RGB(192, 192, 192), _T("23") },                  
	{ RGB(192, 192, 192), _T("24") }, 
	{ RGB(192, 192, 192), _T("25") },                  
	{ RGB(192, 192, 192), _T("26") },               
};

/////////////////////////////////////////////////////////////////////////////
// CCJColorPopup

CCJColorPopup::CCJColorPopup()
{
    Initialise();
}

CCJColorPopup::CCJColorPopup(CPoint p, COLORREF crColor, CWnd* pParentWnd,
                           LPCTSTR szDefaultText /* = NULL */,
                           LPCTSTR szCustomText  /* = NULL */)
{
    Initialise();

    m_crColor       = m_crInitialColor = crColor;
    m_pParent        = pParentWnd;

////////////////////////////////////////
// 1999-06-11 begin mods Kirk Stowell
////////////////////////////////////////
	if( szDefaultText == NULL )
		m_strDefaultText = "自动";
	else
		m_strDefaultText = szDefaultText;

	if( szCustomText == NULL )
		m_strCustomText = "其它颜色...";
	else
		m_strCustomText = szCustomText;
////////////////////////////////////////
// 1999-06-11 end mods Kirk Stowell
////////////////////////////////////////

    CCJColorPopup::Create(p, crColor, pParentWnd, szDefaultText, szCustomText);
}

void CCJColorPopup::Initialise()
{
    m_nNumColors       = sizeof(m_crColors)/sizeof(ColorTableEntry);
    ASSERT(m_nNumColors <= MAX_ColorS);
    if (m_nNumColors > MAX_ColorS)
        m_nNumColors = MAX_ColorS;

    m_nNumColumns       = 0;
    m_nNumRows          = 0;
    m_nBoxSize          = 18;
    m_nMargin           = ::GetSystemMetrics(SM_CXEDGE);
    m_nCurrentSel       = INVALID_Color;
    m_nChosenColorSel  = INVALID_Color;
    m_pParent           = NULL;
    m_crColor          = m_crInitialColor = RGB(0,0,0);

    // Idiot check: Make sure the Color square is at least 5 x 5;
    if (m_nBoxSize - 2*m_nMargin - 2 < 5) m_nBoxSize = 5 + 2*m_nMargin + 2;

    // Create the font
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));
    m_Font.CreateFontIndirect(&(ncm.lfMessageFont));

    // Create the palette
    struct {
        LOGPALETTE    LogPalette;
        PALETTEENTRY  PalEntry[MAX_ColorS];
    } pal;

    LOGPALETTE* pLogPalette = (LOGPALETTE*) &pal;
    pLogPalette->palVersion    = 0x300;
    pLogPalette->palNumEntries = (WORD) m_nNumColors; 

    for (int i = 0; i < m_nNumColors; i++)
    {
        pLogPalette->palPalEntry[i].peRed   = GetRValue(m_crColors[i].crColor);
        pLogPalette->palPalEntry[i].peGreen = GetGValue(m_crColors[i].crColor);
        pLogPalette->palPalEntry[i].peBlue  = GetBValue(m_crColors[i].crColor);
        pLogPalette->palPalEntry[i].peFlags = 0;
    }

    m_Palette.CreatePalette(pLogPalette);
}

CCJColorPopup::~CCJColorPopup()
{
    m_Font.DeleteObject();
    m_Palette.DeleteObject();
}

BOOL CCJColorPopup::Create(CPoint p, COLORREF crColor, CWnd* pParentWnd,
                          LPCTSTR szDefaultText /* = NULL */,
                          LPCTSTR szCustomText  /* = NULL */)
{
    ASSERT(pParentWnd && ::IsWindow(pParentWnd->GetSafeHwnd()));
    ASSERT(pParentWnd->IsKindOf(RUNTIME_CLASS(CCJColorPicker)));

    m_pParent  = pParentWnd;
    m_crColor = m_crInitialColor = crColor;

    // Get the class name and create the window
    CString szClassName = AfxRegisterWndClass(CS_CLASSDC|CS_SAVEBITS|CS_HREDRAW|CS_VREDRAW,
                                              0,
                                              (HBRUSH) (COLOR_BTNFACE+1), 
                                              0);

    if (!CWnd::CreateEx(0, szClassName, _T(""), WS_VISIBLE|WS_POPUP, 
                        p.x, p.y, 100, 100, // size updated soon
                        pParentWnd->GetSafeHwnd(), 0, NULL))
        return FALSE;

    // Store the Custom text
    if (szCustomText != NULL) 
        m_strCustomText = szCustomText;

    // Store the Default Area text
    if (szDefaultText != NULL) 
        m_strDefaultText = szDefaultText;
        
    // Set the window size
    SetWindowSize();

    // Create the tooltips
    CreateToolTips();

    // Find which cell (if any) corresponds to the initial Color
    FindCellFromColor(crColor);

    // Capture all mouse events for the life of this window
    SetCapture();

    return TRUE;
}

BEGIN_MESSAGE_MAP(CCJColorPopup, CWnd)
    //{{AFX_MSG_MAP(CCJColorPopup)
    ON_WM_NCDESTROY()
    ON_WM_LBUTTONUP()
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()
    ON_WM_QUERYNEWPALETTE()
    ON_WM_PALETTECHANGED()
	ON_WM_KILLFOCUS()
	ON_WM_ACTIVATEAPP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJColorPopup message handlers

// For tooltips
BOOL CCJColorPopup::PreTranslateMessage(MSG* pMsg) 
{
    m_ToolTip.RelayEvent(pMsg);
    return CWnd::PreTranslateMessage(pMsg);
}

// If an arrow key is pressed, then move the selection
void CCJColorPopup::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    int row = GetRow(m_nCurrentSel),
        col = GetColumn(m_nCurrentSel);

    if (nChar == VK_DOWN) 
    {
        if (row == DEFAULT_BOX_VALUE) 
            row = col = 0; 
        else if (row == CUSTOM_BOX_VALUE)
        {
            if (m_strDefaultText.GetLength())
                row = col = DEFAULT_BOX_VALUE;
            else
                row = col = 0;
        }
        else
        {
            row++;
            if (GetIndex(row,col) < 0)
            {
                if (m_strCustomText.GetLength())
                    row = col = CUSTOM_BOX_VALUE;
                else if (m_strDefaultText.GetLength())
                    row = col = DEFAULT_BOX_VALUE;
                else
                    row = col = 0;
            }
        }
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_UP) 
    {
        if (row == DEFAULT_BOX_VALUE)
        {
            if (m_strCustomText.GetLength())
                row = col = CUSTOM_BOX_VALUE;
            else
           { 
                row = GetRow(m_nNumColors-1); 
                col = GetColumn(m_nNumColors-1); 
            }
        }
        else if (row == CUSTOM_BOX_VALUE)
        { 
            row = GetRow(m_nNumColors-1); 
            col = GetColumn(m_nNumColors-1); 
        }
        else if (row > 0) row--;
        else /* row == 0 */
        {
            if (m_strDefaultText.GetLength())
                row = col = DEFAULT_BOX_VALUE;
            else if (m_strCustomText.GetLength())
                row = col = CUSTOM_BOX_VALUE;
            else
            { 
                row = GetRow(m_nNumColors-1); 
                col = GetColumn(m_nNumColors-1); 
            }
        }
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_RIGHT) 
    {
        if (row == DEFAULT_BOX_VALUE) 
            row = col = 0; 
        else if (row == CUSTOM_BOX_VALUE)
        {
            if (m_strDefaultText.GetLength())
                row = col = DEFAULT_BOX_VALUE;
            else
                row = col = 0;
        }
        else if (col < m_nNumColumns-1) 
            col++;
        else 
        { 
            col = 0; row++;
        }

        if (GetIndex(row,col) == INVALID_Color)
        {
            if (m_strCustomText.GetLength())
                row = col = CUSTOM_BOX_VALUE;
            else if (m_strDefaultText.GetLength())
                row = col = DEFAULT_BOX_VALUE;
            else
                row = col = 0;
        }

        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_LEFT) 
    {
        if (row == DEFAULT_BOX_VALUE)
        {
            if (m_strCustomText.GetLength())
                row = col = CUSTOM_BOX_VALUE;
            else
           { 
                row = GetRow(m_nNumColors-1); 
                col = GetColumn(m_nNumColors-1); 
            }
        }
        else if (row == CUSTOM_BOX_VALUE)
        { 
            row = GetRow(m_nNumColors-1); 
            col = GetColumn(m_nNumColors-1); 
        }
        else if (col > 0) col--;
        else /* col == 0 */
        {
            if (row > 0) { row--; col = m_nNumColumns-1; }
            else 
            {
                if (m_strDefaultText.GetLength())
                    row = col = DEFAULT_BOX_VALUE;
                else if (m_strCustomText.GetLength())
                    row = col = CUSTOM_BOX_VALUE;
                else
                { 
                    row = GetRow(m_nNumColors-1); 
                    col = GetColumn(m_nNumColors-1); 
                }
            }
        }
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_ESCAPE) 
    {
        m_crColor = m_crInitialColor;
        EndSelection(CPN_SELENDCANCEL);
        return;
    }

    if (nChar == VK_RETURN || nChar == VK_SPACE)
    {
        EndSelection(CPN_SELENDOK);
        return;
    }

    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

// auto-deletion
void CCJColorPopup::OnNcDestroy() 
{
    CWnd::OnNcDestroy();
    delete this;
}

void CCJColorPopup::OnPaint() 
{
    CPaintDC dc(this); // device context for painting

    // Draw the Default Area text
    if (m_strDefaultText.GetLength())
        DrawCell(&dc, DEFAULT_BOX_VALUE);
 
    // Draw Color cells
    for (int i = 0; i < m_nNumColors; i++)
        DrawCell(&dc, i);
    
    // Draw custom text
    if (m_strCustomText.GetLength())
        DrawCell(&dc, CUSTOM_BOX_VALUE);

    // Draw raised window edge (ex-window style WS_EX_WINDOWEDGE is sposed to do this,
    // but for some reason isn't
    CRect rect;
    GetClientRect(rect);
    dc.DrawEdge(rect, EDGE_RAISED, BF_RECT);
}

void CCJColorPopup::OnMouseMove(UINT nFlags, CPoint point) 
{
    int nNewSelection = INVALID_Color;

    // Translate points to be relative raised window edge
    point.x -= m_nMargin;
    point.y -= m_nMargin;

    // First check we aren't in text box
    if (m_strCustomText.GetLength() && m_CustomTextRect.PtInRect(point))
        nNewSelection = CUSTOM_BOX_VALUE;
    else if (m_strDefaultText.GetLength() && m_DefaultTextRect.PtInRect(point))
        nNewSelection = DEFAULT_BOX_VALUE;
    else
    {
        // Take into account text box
        if (m_strDefaultText.GetLength()) 
            point.y -= m_DefaultTextRect.Height();  

        // Get the row and column
        nNewSelection = GetIndex(point.y / m_nBoxSize, point.x / m_nBoxSize);

        // In range? If not, default and exit
        if (nNewSelection < 0 || nNewSelection >= m_nNumColors)
        {
            CWnd::OnMouseMove(nFlags, point);
            return;
        }
    }

    // OK - we have the row and column of the current selection (may be CUSTOM_BOX_VALUE)
    // Has the row/col selection changed? If yes, then redraw old and new cells.
    if (nNewSelection != m_nCurrentSel)
        ChangeSelection(nNewSelection);

    CWnd::OnMouseMove(nFlags, point);
}

// End selection on LButtonUp
void CCJColorPopup::OnLButtonUp(UINT nFlags, CPoint point) 
{    
    CWnd::OnLButtonUp(nFlags, point);

    DWORD pos = GetMessagePos();
    point = CPoint(LOWORD(pos), HIWORD(pos));

    if (m_WindowRect.PtInRect(point))
        EndSelection(CPN_SELENDOK);
    else
        EndSelection(CPN_SELENDCANCEL);
}

/////////////////////////////////////////////////////////////////////////////
// CCJColorPopup implementation

int CCJColorPopup::GetIndex(int row, int col) const
{ 
    if ((row == CUSTOM_BOX_VALUE || col == CUSTOM_BOX_VALUE) && m_strCustomText.GetLength())
        return CUSTOM_BOX_VALUE;
    else if ((row == DEFAULT_BOX_VALUE || col == DEFAULT_BOX_VALUE) && m_strDefaultText.GetLength())
        return DEFAULT_BOX_VALUE;
    else if (row < 0 || col < 0 || row >= m_nNumRows || col >= m_nNumColumns)
        return INVALID_Color;
    else
    {
        if (row*m_nNumColumns + col >= m_nNumColors)
            return INVALID_Color;
        else
            return row*m_nNumColumns + col;
    }
}

int CCJColorPopup::GetRow(int nIndex) const               
{ 
    if (nIndex == CUSTOM_BOX_VALUE && m_strCustomText.GetLength())
        return CUSTOM_BOX_VALUE;
    else if (nIndex == DEFAULT_BOX_VALUE && m_strDefaultText.GetLength())
        return DEFAULT_BOX_VALUE;
    else if (nIndex < 0 || nIndex >= m_nNumColors)
        return INVALID_Color;
    else
        return nIndex / m_nNumColumns; 
}

int CCJColorPopup::GetColumn(int nIndex) const            
{ 
    if (nIndex == CUSTOM_BOX_VALUE && m_strCustomText.GetLength())
        return CUSTOM_BOX_VALUE;
    else if (nIndex == DEFAULT_BOX_VALUE && m_strDefaultText.GetLength())
        return DEFAULT_BOX_VALUE;
    else if (nIndex < 0 || nIndex >= m_nNumColors)
        return INVALID_Color;
    else
        return nIndex % m_nNumColumns; 
}

void CCJColorPopup::FindCellFromColor(COLORREF crColor)
{
    if (crColor == CLR_DEFAULT && m_strDefaultText.GetLength())
    {
        m_nChosenColorSel = DEFAULT_BOX_VALUE;
        return;
    }

    for (int i = 0; i < m_nNumColors; i++)
    {
        if (GetColor(i) == crColor)
        {
            m_nChosenColorSel = i;
            return;
        }
    }

    if (m_strCustomText.GetLength())
        m_nChosenColorSel = CUSTOM_BOX_VALUE;
    else
        m_nChosenColorSel = INVALID_Color;
}

// Gets the dimensions of the Color cell given by (row,col)
BOOL CCJColorPopup::GetCellRect(int nIndex, const LPRECT& rect)
{
    if (nIndex == CUSTOM_BOX_VALUE)
    {
        ::SetRect(rect, 
                  m_CustomTextRect.left,  m_CustomTextRect.top,
                  m_CustomTextRect.right, m_CustomTextRect.bottom);
        return TRUE;
    }
    else if (nIndex == DEFAULT_BOX_VALUE)
    {
        ::SetRect(rect, 
                  m_DefaultTextRect.left,  m_DefaultTextRect.top,
                  m_DefaultTextRect.right, m_DefaultTextRect.bottom);
        return TRUE;
    }

    if (nIndex < 0 || nIndex >= m_nNumColors)
        return FALSE;

    rect->left = GetColumn(nIndex) * m_nBoxSize + m_nMargin;
    rect->top  = GetRow(nIndex) * m_nBoxSize + m_nMargin;

    // Move everything down if we are displaying a default text area
    if (m_strDefaultText.GetLength()) 
        rect->top += (m_nMargin + m_DefaultTextRect.Height());

    rect->right = rect->left + m_nBoxSize;
    rect->bottom = rect->top + m_nBoxSize;

    return TRUE;
}

// Works out an appropriate size and position of this window
void CCJColorPopup::SetWindowSize()
{
    CSize TextSize;

    // If we are showing a custom or default text area, get the font and text size.
    if (m_strCustomText.GetLength() || m_strDefaultText.GetLength())
    {
        CClientDC dc(this);
        CFont* pOldFont = (CFont*) dc.SelectObject(&m_Font);

        // Get the size of the custom text (if there IS custom text)
        TextSize = CSize(0,0);
        if (m_strCustomText.GetLength())
            TextSize = dc.GetTextExtent(m_strCustomText);

        // Get the size of the default text (if there IS default text)
        if (m_strDefaultText.GetLength())
        {
            CSize DefaultSize = dc.GetTextExtent(m_strDefaultText);
            if (DefaultSize.cx > TextSize.cx) TextSize.cx = DefaultSize.cx;
            if (DefaultSize.cy > TextSize.cy) TextSize.cy = DefaultSize.cy;
        }

        dc.SelectObject(pOldFont);
        TextSize += CSize(2*m_nMargin,2*m_nMargin);

        // Add even more space to draw the horizontal line
        TextSize.cy += 2*m_nMargin + 2;
    }

    // Get the number of columns and rows
    //m_nNumColumns = (int) sqrt((double)m_nNumColors);    // for a square window (yuk)
    m_nNumColumns = NUM_COLUMNS;
    m_nNumRows = m_nNumColors / m_nNumColumns;
    if (m_nNumColors % m_nNumColumns) m_nNumRows++;

    // Get the current window position, and set the new size
    CRect rect;
    GetWindowRect(rect);

    m_WindowRect.SetRect(rect.left, rect.top, 
                         rect.left + m_nNumColumns*m_nBoxSize + 2*m_nMargin,
                         rect.top  + m_nNumRows*m_nBoxSize + 2*m_nMargin);

    // if custom text, then expand window if necessary, and set text width as
    // window width
    if (m_strDefaultText.GetLength()) 
    {
        if (TextSize.cx > m_WindowRect.Width())
            m_WindowRect.right = m_WindowRect.left + TextSize.cx;
        TextSize.cx = m_WindowRect.Width()-2*m_nMargin;

        // Work out the text area
        m_DefaultTextRect.SetRect(m_nMargin, m_nMargin, 
                                  m_nMargin+TextSize.cx, 2*m_nMargin+TextSize.cy);
        m_WindowRect.bottom += m_DefaultTextRect.Height() + 2*m_nMargin;
    }

    // if custom text, then expand window if necessary, and set text width as
    // window width
    if (m_strCustomText.GetLength()) 
    {
        if (TextSize.cx > m_WindowRect.Width())
            m_WindowRect.right = m_WindowRect.left + TextSize.cx;
        TextSize.cx = m_WindowRect.Width()-2*m_nMargin;

        // Work out the text area
        m_CustomTextRect.SetRect(m_nMargin, m_WindowRect.Height(), 
                                 m_nMargin+TextSize.cx, 
                                 m_WindowRect.Height()+m_nMargin+TextSize.cy);
        m_WindowRect.bottom += m_CustomTextRect.Height() + 2*m_nMargin;
   }

    // Need to check it'll fit on screen: Too far right?
    CSize ScreenSize(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
    if (m_WindowRect.right > ScreenSize.cx)
        m_WindowRect.OffsetRect(-(m_WindowRect.right - ScreenSize.cx), 0);

    // Too far left?
    if (m_WindowRect.left < 0)
        m_WindowRect.OffsetRect( -m_WindowRect.left, 0);

    // Bottom falling out of screen?
    if (m_WindowRect.bottom > ScreenSize.cy)
    {
        CRect ParentRect;
        m_pParent->GetWindowRect(ParentRect);
        m_WindowRect.OffsetRect(0, -(ParentRect.Height() + m_WindowRect.Height()));
    }

    // Set the window size and position
    MoveWindow(m_WindowRect, TRUE);
}

void CCJColorPopup::CreateToolTips()
{
    // Create the tool tip
    if (!m_ToolTip.Create(this)) return;

    // Add a tool for each cell
    for (int i = 0; i < m_nNumColors; i++)
    {
        CRect rect;
        if (!GetCellRect(i, rect)) continue;
            m_ToolTip.AddTool(this, GetColorName(i), rect, 1);
    }
}

void CCJColorPopup::ChangeSelection(int nIndex)
{
    CClientDC dc(this);        // device context for drawing

    if (nIndex > m_nNumColors)
        nIndex = CUSTOM_BOX_VALUE; 

    if ((m_nCurrentSel >= 0 && m_nCurrentSel < m_nNumColors) ||
        m_nCurrentSel == CUSTOM_BOX_VALUE || m_nCurrentSel == DEFAULT_BOX_VALUE)
    {
        // Set Current selection as invalid and redraw old selection (this way
        // the old selection will be drawn unselected)
        int OldSel = m_nCurrentSel;
        m_nCurrentSel = INVALID_Color;
        DrawCell(&dc, OldSel);
    }

    // Set the current selection as row/col and draw (it will be drawn selected)
    m_nCurrentSel = nIndex;
    DrawCell(&dc, m_nCurrentSel);

    // Store the current Color
    if (m_nCurrentSel == CUSTOM_BOX_VALUE)
        m_pParent->SendMessage(CPN_SELCHANGE, (WPARAM) m_crInitialColor, 0);
    else if (m_nCurrentSel == DEFAULT_BOX_VALUE)
    {
        m_crColor = CLR_DEFAULT;
        m_pParent->SendMessage(CPN_SELCHANGE, (WPARAM) CLR_DEFAULT, 0);
    }
    else
    {
        m_crColor = GetColor(m_nCurrentSel);
        m_pParent->SendMessage(CPN_SELCHANGE, (WPARAM) m_crColor, 0);
    }
}

void CCJColorPopup::EndSelection(int nMessage)
{
    ReleaseCapture();

    // If custom text selected, perform a custom Color selection
    if (nMessage != CPN_SELENDCANCEL && m_nCurrentSel == CUSTOM_BOX_VALUE)
    {
        CColorDialog dlg(m_crInitialColor, CC_FULLOPEN | CC_ANYCOLOR, this);

        if (dlg.DoModal() == IDOK)
            m_crColor = dlg.GetColor();
        else
            nMessage = CPN_SELENDCANCEL;
    } 

    if (nMessage == CPN_SELENDCANCEL)
        m_crColor = m_crInitialColor;

    m_pParent->SendMessage(nMessage, (WPARAM) m_crColor, 0);
    
    DestroyWindow();
}

void CCJColorPopup::DrawCell(CDC* pDC, int nIndex)
{
    // For the Custom Text area
    if (m_strCustomText.GetLength() && nIndex == CUSTOM_BOX_VALUE)
    {
        // The extent of the actual text button
        CRect TextButtonRect = m_CustomTextRect;
        TextButtonRect.top += 2*m_nMargin;

        // Fill background
        pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw horizontal line
        pDC->FillSolidRect(m_CustomTextRect.left+2*m_nMargin, m_CustomTextRect.top,
                           m_CustomTextRect.Width()-4*m_nMargin, 1, ::GetSysColor(COLOR_3DSHADOW));
        pDC->FillSolidRect(m_CustomTextRect.left+2*m_nMargin, m_CustomTextRect.top+1,
                           m_CustomTextRect.Width()-4*m_nMargin, 1, ::GetSysColor(COLOR_3DHILIGHT));

        TextButtonRect.DeflateRect(1,1);

        // fill background
        if (m_nChosenColorSel == nIndex && m_nCurrentSel != nIndex)
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DLIGHT));
        else
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw button
        if (m_nCurrentSel == nIndex) 
            pDC->DrawEdge(TextButtonRect, BDR_RAISEDINNER, BF_RECT);
        else if (m_nChosenColorSel == nIndex)
            pDC->DrawEdge(TextButtonRect, BDR_SUNKENOUTER, BF_RECT);

        // Draw custom text
        CFont *pOldFont = (CFont*) pDC->SelectObject(&m_Font);
        pDC->SetBkMode(TRANSPARENT);
        pDC->DrawText(m_strCustomText, TextButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        pDC->SelectObject(pOldFont);

        return;
    }        

    // For the Default Text area
    if (m_strDefaultText.GetLength() && nIndex == DEFAULT_BOX_VALUE)
    {
        // Fill background
        pDC->FillSolidRect(m_DefaultTextRect, ::GetSysColor(COLOR_3DFACE));

        // The extent of the actual text button
        CRect TextButtonRect = m_DefaultTextRect;
        TextButtonRect.DeflateRect(1,1);

        // fill background
        if (m_nChosenColorSel == nIndex && m_nCurrentSel != nIndex)
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DLIGHT));
        else
            pDC->FillSolidRect(TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw thin line around text
        CRect LineRect = TextButtonRect;
        LineRect.DeflateRect(2*m_nMargin,2*m_nMargin);
        CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
        CPen* pOldPen = pDC->SelectObject(&pen);
        pDC->SelectStockObject(NULL_BRUSH);
        pDC->Rectangle(LineRect);
        pDC->SelectObject(pOldPen);

        // Draw button
        if (m_nCurrentSel == nIndex) 
            pDC->DrawEdge(TextButtonRect, BDR_RAISEDINNER, BF_RECT);
        else if (m_nChosenColorSel == nIndex)
            pDC->DrawEdge(TextButtonRect, BDR_SUNKENOUTER, BF_RECT);

        // Draw custom text
        CFont *pOldFont = (CFont*) pDC->SelectObject(&m_Font);
        pDC->SetBkMode(TRANSPARENT);
        pDC->DrawText(m_strDefaultText, TextButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        pDC->SelectObject(pOldFont);

        return;
    }        

    CRect rect;
    if (!GetCellRect(nIndex, rect)) return;

    // Select and realize the palette
    CPalette* pOldPalette = NULL;
    if (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
    {
        pOldPalette = pDC->SelectPalette(&m_Palette, FALSE);
        pDC->RealizePalette();
    }

    // fill background
    if (m_nChosenColorSel == nIndex && m_nCurrentSel != nIndex)
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DHILIGHT));
    else
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DFACE));

    // Draw button
    if (m_nCurrentSel == nIndex) 
        pDC->DrawEdge(rect, BDR_RAISEDINNER, BF_RECT);
    else if (m_nChosenColorSel == nIndex)
        pDC->DrawEdge(rect, BDR_SUNKENOUTER, BF_RECT);

    CBrush brush(PALETTERGB(GetRValue(GetColor(nIndex)), 
                            GetGValue(GetColor(nIndex)), 
                            GetBValue(GetColor(nIndex)) ));
    CPen   pen;
    pen.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));

    CBrush* pOldBrush = (CBrush*) pDC->SelectObject(&brush);
    CPen*   pOldPen   = (CPen*)   pDC->SelectObject(&pen);

    // Draw the cell Color
    rect.DeflateRect(m_nMargin+1, m_nMargin+1);
    pDC->Rectangle(rect);

    // restore DC and cleanup
    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
    brush.DeleteObject();
    pen.DeleteObject();

    if (pOldPalette && pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
        pDC->SelectPalette(pOldPalette, FALSE);
}

BOOL CCJColorPopup::OnQueryNewPalette() 
{
    Invalidate();    
    return CWnd::OnQueryNewPalette();
}

void CCJColorPopup::OnPaletteChanged(CWnd* pFocusWnd) 
{
    CWnd::OnPaletteChanged(pFocusWnd);

    if (pFocusWnd->GetSafeHwnd() != GetSafeHwnd())
        Invalidate();
}

void CCJColorPopup::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

    ReleaseCapture();
    //DestroyWindow(); - causes crash when Custom Color dialog appears.
}

// KillFocus problem fix suggested by Paul Wilkerson.
void CCJColorPopup::OnActivateApp(BOOL bActive, DWORD hTask) 
{
	CWnd::OnActivateApp(bActive, hTask);

	// If Deactivating App, cancel this selection
	if (!bActive)
		 EndSelection(CPN_SELENDCANCEL);
}

void AFXAPI DDX_CJColorPicker(CDataExchange *pDX, int nIDC, COLORREF& crColor)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    ASSERT (hWndCtrl != NULL);                
    
    CCJColorPicker* pColorPicker = (CCJColorPicker*) CWnd::FromHandle(hWndCtrl);
    if (pDX->m_bSaveAndValidate)
    {
        crColor = pColorPicker->GetColor();
    }
    else // initializing
    {
        pColorPicker->SetColor(crColor);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker

CCJColorPicker::CCJColorPicker()
{
    SetBkColor(GetSysColor(COLOR_3DFACE));
    SetTextColor(GetSysColor(COLOR_BTNTEXT));

    m_bTrackSelection = FALSE;
    m_nSelectionMode = CP_MODE_BK;
    m_bActive = FALSE;

////////////////////////////////////////
// 1999-06-11 begin mods Kirk Stowell
////////////////////////////////////////
	m_strDefaultText = "自动";
	m_strCustomText  = "其它颜色...";
////////////////////////////////////////
// 1999-06-11 end mods Kirk Stowell
////////////////////////////////////////
}

CCJColorPicker::~CCJColorPicker()
{
}

IMPLEMENT_DYNCREATE(CCJColorPicker, CButton)

BEGIN_MESSAGE_MAP(CCJColorPicker, CButton)
    //{{AFX_MSG_MAP(CCJColorPicker)
    ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
    ON_WM_CREATE()
	//}}AFX_MSG_MAP
    ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
    ON_MESSAGE(CPN_SELENDCANCEL, OnSelEndCancel)
    ON_MESSAGE(CPN_SELCHANGE,    OnSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker message handlers

LONG CCJColorPicker::OnSelEndOK(UINT lParam, LONG /*wParam*/)
{
    COLORREF crNewColor = (COLORREF) lParam;
    m_bActive = FALSE;
    SetColor(crNewColor);

    CWnd *pParent = GetParent();
    if (pParent) {
        pParent->SendMessage(CPN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        pParent->SendMessage(CPN_SELENDOK, lParam, (WPARAM) GetDlgCtrlID());
    }

    if (crNewColor != GetColor())
        if (pParent) pParent->SendMessage(CPN_SELCHANGE, lParam, (WPARAM) GetDlgCtrlID());

    return TRUE;
}

LONG CCJColorPicker::OnSelEndCancel(UINT lParam, LONG /*wParam*/)
{
    m_bActive = FALSE;
    SetColor((COLORREF) lParam);

    CWnd *pParent = GetParent();
    if (pParent) {
        pParent->SendMessage(CPN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        pParent->SendMessage(CPN_SELENDCANCEL, lParam, (WPARAM) GetDlgCtrlID());
    }

    return TRUE;
}

LONG CCJColorPicker::OnSelChange(UINT lParam, LONG /*wParam*/)
{
    if (m_bTrackSelection) SetColor((COLORREF) lParam);

    CWnd *pParent = GetParent();
    if (pParent) pParent->SendMessage(CPN_SELCHANGE, lParam, (WPARAM) GetDlgCtrlID());

    return TRUE;
}

int CCJColorPicker::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CButton::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    SetWindowSize();    // resize appropriately
    return 0;
}

// On mouse click, create and show a CJColorPopup window for Color selection
BOOL CCJColorPicker::OnClicked()
{
    m_bActive = TRUE;

    CRect rect;
    GetWindowRect(rect);
    new CCJColorPopup(CPoint(rect.left, rect.bottom),    // Point to display popup
                     GetColor(),                       // Selected Color
                     this,                              // parent
                     m_strDefaultText,                  // "Default" text area
                     m_strCustomText);                  // Custom Text

    CWnd *pParent = GetParent();
    if (pParent)
        pParent->SendMessage(CPN_DROPDOWN, (LPARAM)GetColor(), (WPARAM) GetDlgCtrlID());

    // Docs say I should return FALSE to stop the parent also getting the message.
    // HA! What a joke.

    return TRUE;
}

////////////////////////////////////////
// 1999-06-11 begin mods Kirk Stowell
////////////////////////////////////////
void CCJColorPicker::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC*	pDC			= CDC::FromHandle( lpDrawItemStruct->hDC );
    UINT	itemState	= lpDrawItemStruct->itemState;
	CRect	rcItem		= lpDrawItemStruct->rcItem;
	DWORD	dwState		= EDGE_RAISED;
	DWORD	dwArrow		= DFCS_SCROLLDOWN;
	
	// erase everything....
	pDC->FillRect( rcItem, CBrush::FromHandle(::GetSysColorBrush( COLOR_3DFACE )));
	
	// set display flags based on state.
	if((itemState & ODS_SELECTED) || (m_bActive == TRUE)) {
		dwState = EDGE_SUNKEN;
		dwArrow = DFCS_SCROLLDOWN|DFCS_PUSHED;
	}
	if(itemState & ODS_DISABLED) {
		dwArrow = DFCS_SCROLLDOWN|DFCS_INACTIVE;
	}
	
	// Draw the drop arrow.
	CRect rcArrow( rcItem );
	rcArrow.left = rcArrow.Width()-::GetSystemMetrics( SM_CXHTHUMB );
    pDC->DrawFrameControl( &rcArrow, DFC_SCROLL, dwArrow );
	pDC->Draw3dRect( rcArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DFACE ));
	rcArrow.DeflateRect( 1,1 );
	pDC->Draw3dRect( rcArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DFACE ));
	
	if((itemState & ODS_SELECTED) || (m_bActive == TRUE))
		rcArrow.OffsetRect( 1,1 );
	
	// draw the seperator line.
	CPen penShadow( PS_SOLID, 1, ::GetSysColor( COLOR_3DSHADOW  ));
	pDC->SelectObject( &penShadow );
	pDC->MoveTo( rcArrow.left-1, rcArrow.top+2 );
	pDC->LineTo( rcArrow.left-1, rcArrow.bottom-2 );

	CPen penHilite( PS_SOLID, 1, ::GetSysColor( COLOR_3DHILIGHT ));
	pDC->SelectObject( &penHilite );
	pDC->MoveTo( rcArrow.left, rcArrow.top+2 );
	pDC->LineTo( rcArrow.left, rcArrow.bottom-2 );
	
	// draw the control border.
    pDC->DrawEdge( rcItem, dwState, BF_RECT );
	
	// draw the focus rect.
	rcItem.DeflateRect( 2, 2 );
	pDC->Draw3dRect( rcItem, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DFACE ) );
	if( itemState & ODS_FOCUS ) {
		pDC->DrawFocusRect( rcItem );
	}
	
	// draw the color box.
	if((itemState & ODS_SELECTED) || (m_bActive == TRUE))
		rcItem.OffsetRect( 1,1 );
	
	rcItem.DeflateRect( 2, 2 );
	rcItem.right = rcArrow.left-4;
	
    CBrush brush((( itemState & ODS_DISABLED ) || m_crColorBk == CLR_DEFAULT ) ? 
		::GetSysColor( COLOR_3DFACE ) : m_crColorBk );
    CBrush* pOldBrush = ( CBrush* )pDC->SelectObject( &brush );

	pDC->SelectStockObject(( itemState & ODS_DISABLED )?WHITE_PEN:BLACK_PEN );
	pDC->Rectangle( rcItem );
    pDC->SelectObject( pOldBrush );
	
    // Draw the window text (if any)
	CString strText;
    GetWindowText(strText);
    if(strText.GetLength())
    {
        pDC->SetBkMode(TRANSPARENT);
        if(itemState & ODS_DISABLED)
        {
            rcItem.OffsetRect(1,1);
            pDC->SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
            pDC->DrawText(strText, rcItem, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
            rcItem.OffsetRect(-1,-1);
            pDC->SetTextColor(::GetSysColor(COLOR_3DSHADOW));
            pDC->DrawText(strText, rcItem, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
        }
        else
        {
            pDC->SetTextColor((m_crColorText == CLR_DEFAULT)? 0 : m_crColorText);
            pDC->DrawText(strText, rcItem, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
        }
    }
}
////////////////////////////////////////
// 1999-06-11 end mods Kirk Stowell
////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker overrides

void CCJColorPicker::PreSubclassWindow() 
{
    ModifyStyle(0, BS_OWNERDRAW);        // Make it owner drawn
    CButton::PreSubclassWindow();
    SetWindowSize();                     // resize appropriately
}

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker attributes

COLORREF CCJColorPicker::GetColor()
{ 
    return (m_nSelectionMode == CP_MODE_TEXT)? 
        GetTextColor(): GetBkColor(); 
}

void CCJColorPicker::SetColor(COLORREF crColor)
{ 
    (m_nSelectionMode == CP_MODE_TEXT)? 
        SetTextColor(crColor): SetBkColor(crColor); 
}

void CCJColorPicker::SetBkColor(COLORREF crColorBk)
{
    m_crColorBk = crColorBk;
    if (IsWindow(m_hWnd)) RedrawWindow();
}

void CCJColorPicker::SetTextColor(COLORREF crColorText)
{
    m_crColorText = crColorText;
    if (IsWindow(m_hWnd)) RedrawWindow();
}

void CCJColorPicker::SetDefaultText(LPCTSTR szDefaultText)
{
    m_strDefaultText = (szDefaultText)? szDefaultText : _T("");
}

void CCJColorPicker::SetCustomText(LPCTSTR szCustomText)
{
    m_strCustomText = (szCustomText)? szCustomText : _T("");
}

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker implementation

void CCJColorPicker::SetWindowSize()
{
    // Get size dimensions of edges
    CSize MarginSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));

    // Get size of dropdown arrow
    int nArrowWidth = max(::GetSystemMetrics(SM_CXHTHUMB), 5*MarginSize.cx);
    int nArrowHeight = max(::GetSystemMetrics(SM_CYVTHUMB), 5*MarginSize.cy);
    CSize ArrowSize(max(nArrowWidth, nArrowHeight), max(nArrowWidth, nArrowHeight));

    // Get window size
    CRect rect;
    GetWindowRect(rect);

    CWnd* pParent = GetParent();
    if (pParent)
        pParent->ScreenToClient(rect);

    // Set window size at least as wide as 2 arrows, and as high as arrow + margins
    int nWidth = max(rect.Width(), 2*ArrowSize.cx + 2*MarginSize.cx);
    MoveWindow(rect.left, rect.top, nWidth, ArrowSize.cy+2*MarginSize.cy, TRUE);

    // Get the new coords of this window
    GetWindowRect(rect);
    ScreenToClient(rect);

    // Get the rect where the arrow goes, and convert to client coords.
    m_ArrowRect.SetRect(rect.right - ArrowSize.cx - MarginSize.cx, 
                        rect.top + MarginSize.cy, rect.right - MarginSize.cx,
                        rect.bottom - MarginSize.cy);
}

#define LINE_STYLE_ITEM_HEIGHT 20

CLineStyleBox::CLineStyleBox(BOOL bWindows)
{
	m_bWindows = bWindows;
}

void CLineStyleBox::SetStyleType(BOOL bWindows)
{
	m_bWindows = bWindows;
}

void CLineStyleBox::AddItem()
{                              
	char s1[20],s2[10];
	int i;    
	if(m_bWindows)
		for(i=1; i<14; i++)
			AddString((LPCSTR) i);
	else for (i=1;i<14; i++)
	{
		CBitmap bitmap;
		strcpy(s1,"LINE");
		ltoa(i,s2,10);
		strcat(s1,s2);  
		if(bitmap.LoadBitmap(s1))
			AddString((LPCSTR) i);
		else break;	
	}
}

void CLineStyleBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	lpMIS->itemHeight = LINE_STYLE_ITEM_HEIGHT; 
}

void CLineStyleBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);       
	if(lpDIS->itemAction & ODA_DRAWENTIRE)
	{
		CBrush br(0xffffff);
		pDC->FillRect(&lpDIS->rcItem, &br);
		int style = (int)lpDIS->itemData;
		if (style < 0) return;
    	RECT rect = lpDIS->rcItem;
		if(m_bWindows) 
		{
			CPen m_Pen;
			m_Pen.CreatePen(style-1,1,RGB(0,0,0));
			CPen* pOldPen = pDC->SelectObject(&m_Pen); 
			pDC->MoveTo(rect.left+3,(rect.top + rect.bottom) /2);
			pDC->LineTo(rect.right-3,(rect.top + rect.bottom) /2);
			pDC->SelectObject(pOldPen);
		}
		else
		{ 
			CLinePen m_LinePen(style,1,0);
			DrawHorLine(rect.left+3,rect.right-3,(rect.top + rect.bottom)/2,&m_LinePen,pDC);
		}
    }                   
    
	if((lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		CBrush br(0x08000);
		pDC->FrameRect(&lpDIS->rcItem, &br);
	}

	if (!(lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & ODA_SELECT))
	{
		CBrush br(0xffffff);
		pDC->FrameRect(&lpDIS->rcItem, &br);
	}
}

#define FILL_PATTERN_ITEM_HEIGHT 28

CFillPatternBox::CFillPatternBox(BOOL bWindows)
{
	m_bWindows = bWindows;
}

void CFillPatternBox::SetStyleType(BOOL bWindows)
{
	m_bWindows = bWindows;
}

void CFillPatternBox::AddItem()
{                              
	int i;	
	if(m_bWindows)	
	{
		for (i=1; i<9; i++)
			AddString((LPCSTR) i);
		return;
	}

	/*for (i=1; i<3 ; i++)
		AddString((LPCSTR) i);
	CString PathStr=GetWorkPath("LithLib");
	char s1[512],s2[10];
	for (i=3; ; i++)
	{
		strcpy(s1,PathStr);  
		strcat(s1,"Lith");        
		ltoa(i-2,s2,10);
		strcat(s1,s2); 
		strcat(s1,".dat");
		if(DetectFileExist(s1))
			AddString((LPCSTR) i);
		else break;
	}*/
	for (i=1; i<101; i++)
		AddString((LPCSTR) i);
}

void CFillPatternBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	lpMIS->itemHeight = FILL_PATTERN_ITEM_HEIGHT; 
}

void CFillPatternBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);       
	char s[10];
   	CRect rect = lpDIS->rcItem;
	int h = (rect.bottom - rect.top - 16) / 2;
	rect.top = rect.top + h;
	rect.bottom = rect.top + 16;
	if (lpDIS->itemAction & ODA_DRAWENTIRE)
	{
		CBrush br(0xffffff);
		pDC->FillRect(rect, &br);
		int style = (int)lpDIS->itemData;
		if (style < 0) return;
		ltoa(style,s,10);
		CSize size = pDC->GetTextExtent(s,(int)strlen(s));
		int k = (16 - size.cy)/2;
		CRect rect1 = rect;
		rect1.top += k; rect1.bottom -= k;
   		pDC->TextOut(rect1.left,rect1.top,s,(int)strlen(s));
		rect.left += size.cx+10;
		CString szHT;   	
       	if (m_bWindows)
    	{
			if (style == 1)
   				pDC->TextOut(rect.left,rect.top+2,"空");
			else
			{
    			CBrush br1;
    			br1.CreateHatchBrush(style-2,0x0);
    			pDC->FillRect(&rect,&br1);
			}
    	}    	
    	else
		{
			if (style == 1)
   				pDC->TextOut(rect.left,rect.top+2,"半透明");
			else
       			LithFillBox(rect,style-1,0x08000,rect.TopLeft (),pDC);
		}
    }
	if ((lpDIS->itemState & ODS_SELECTED) &&
			(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		CBrush br(0x08000);
		pDC->FrameRect(&lpDIS->rcItem, &br);
	}

	if (!(lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & ODA_SELECT))
	{
		CBrush br(0xffffff);
		pDC->FrameRect(&lpDIS->rcItem, &br);
	}
}

void SerializeLogFont(CArchive& ar,LOGFONT& lf)
{
	if (ar.IsStoring())
	{
		ar << lf.lfHeight;
		ar << lf.lfWidth;
		ar << lf.lfEscapement;
		ar << lf.lfOrientation;
		ar << lf.lfWeight;
		ar << lf.lfItalic;
		ar << lf.lfUnderline;
		ar << lf.lfStrikeOut;
		ar << lf.lfCharSet;
		ar << lf.lfOutPrecision;
		ar << lf.lfClipPrecision;
		ar << lf.lfQuality;
		ar << lf.lfPitchAndFamily;
		ar.Write(lf.lfFaceName,LF_FACESIZE);
	}
	else
	{
		ar >> lf.lfHeight;
		ar >> lf.lfWidth;
		ar >> lf.lfEscapement;
		ar >> lf.lfOrientation;
		ar >> lf.lfWeight;
		ar >> lf.lfItalic;
		ar >> lf.lfUnderline;
		ar >> lf.lfStrikeOut;
		ar >> lf.lfCharSet;
		ar >> lf.lfOutPrecision;
		ar >> lf.lfClipPrecision;
		ar >> lf.lfQuality;
		ar >> lf.lfPitchAndFamily;
		ar.Read(lf.lfFaceName,LF_FACESIZE);
	}
}

void SerializeLogPen(CArchive& ar,LOGPEN& lopn)
{
	if (ar.IsStoring())
	{
		ar << lopn.lopnStyle;
		ar << lopn.lopnWidth;
		ar << lopn.lopnColor;
	}
	else
	{
 		ar >> lopn.lopnStyle;
		ar >> lopn.lopnWidth;
		ar >> lopn.lopnColor;
	}
}

void SerializeLogBrush(CArchive& ar,LOGBRUSH& lb)
{
	if (ar.IsStoring())
	{
		ar << lb.lbStyle;
		ar << lb.lbColor;
		ar << lb.lbHatch;
	}
	else
	{
		ar >> lb.lbStyle;
		ar >> lb.lbColor;
		ar >> lb.lbHatch;
	}
}

void Convert(char* buf,int len,int bytes)
{
	if (bytes <= 1) return;
	char* tmp = new char[bytes];          
	long j=0;
	for (int i=0; i<len; i++)
	{
		memcpy(tmp,buf+j,bytes);
		for (int k=0; k<bytes; k++) buf[j+k] = tmp[bytes-k-1];
		j += bytes;
	}       
	delete tmp;
}

void SerializePoint(CArchive& ar,LPPOINT pt, int num)
{
	if (ar.IsStoring())
	{
#ifdef _UNIX_
		char* buf = new char[sizeof(POINT)*num];
		memcpy(buf,pt,sizeof(POINT)*num);
		Convert(buf,sizeof(POINT)*num,sizeof(POINT));
		ar.Write(buf,sizeof(POINT)*num);
		delete buf;
#else
		ar.Write(pt,sizeof(POINT)*num);
#endif
	}
	else
	{
		ar.Read(pt,sizeof(POINT)*num);
#ifdef _UNIX_
		Convert(pt,sizeof(POINT)*num,sizeof(POINT));
#endif
	}
}

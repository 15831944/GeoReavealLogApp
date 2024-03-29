// Surfer.cpp: implementation of the CSurfer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Surfer.h"
#include "Gradient.h"
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define		BOUNDARY_LIST	1
#define		AXIS_LIST		2
#define		DATA_LIST		3
#define		WALL_LIST		4
#define		IMAGE_LIST		5

GLuint		g_nImageID;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSurfer::CSurfer()
{
	m_crBoundary = RGB(255, 0, 255);
	m_bBoundary = true;
	m_bWall = true;
	m_pImage = NULL;
	m_bSupportImage = false;
	m_bShowImage = false;
	m_bShowLegend = false;

	m_ZMin=240;m_ZMax=480;
	m_SchemeFile="Wave-STC";
	m_nLegend=1;
	m_nNumOfLevels=24;
	Colors=NULL;
}

CSurfer::~CSurfer()
{
	if(Colors!=NULL)
		delete Colors;
}

void CSurfer::Init(HWND hWnd) throw()
{
	if(Colors!=NULL)
		delete Colors;
	Colors=new COLORREF[m_nNumOfLevels];
	CGradient m_Gradient;
	CString ColorFile=GetWorkPath("Color")+m_SchemeFile+".cip";//
	CFile file(ColorFile, CFile::modeRead | CFile::shareDenyWrite );
	CArchive ar(&file, CArchive::load);	
	m_Gradient.Serialize(ar);
	RGBTRIPLE *pal = new RGBTRIPLE[m_nNumOfLevels],*entry;
	m_Gradient.SetQuantization(m_nNumOfLevels);
	m_Gradient.MakeEntries(pal,m_nNumOfLevels);
	for(int i=0;i<m_nNumOfLevels; i++) 
	{
		entry = &pal[i];
		Colors[i]=RGB(entry->rgbtRed,entry->rgbtGreen, entry->rgbtBlue);
	}
	delete[] pal;

	C3DGrapher::Init(hWnd);

	BuildBoundaryList();
	BuildAxisList();
	BuildDataList();
	BuildWallList();

	SetBkGndColor(RGB(0, 0, 72));
}

void CSurfer::Destroy() throw()
{
	BEGIN_GL
	::glDeleteLists(BOUNDARY_LIST, WALL_LIST);
	if(m_bSupportImage)
		::glDeleteLists(IMAGE_LIST, 1);
	END_GL

	C3DGrapher::Destroy();
}

void CSurfer::Render() throw()
{
	BEGIN_GL
	// Clear First
	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start rendering
	::glPushMatrix();
	::glTranslated(0, 0, -m_dZrange*2);
	::glTranslated(0, 0, m_dZtran);
	::glRotatef(m_dXrot, 1.0, 0.0, 0.0);
	::glRotatef(m_dYrot, 0.0, 1.0, 0.0);
	::glRotatef(m_dZrot, 0.0, 0.0, 1.0);
	if(m_bBoundary)
		::glCallList(BOUNDARY_LIST);
	::glCallList(AXIS_LIST);
	::glCallList(DATA_LIST);
	if(m_bWall)
		::glCallList(WALL_LIST);
	if(m_bSupportImage && m_bShowImage)
		::glCallList(IMAGE_LIST);
	if(m_bShowLegend)
		ShowLegend();
 	::glPopMatrix();
	// Flush & Swap
	::glFlush();
	::SwapBuffers(m_hDC);
	END_GL
}

void CSurfer::BuildBoundaryList() const throw()
{
	BEGIN_GL
	double x = m_dXrange/2;
	double y = m_dYrange/2;
	double z = m_dZrange/2;

	::glNewList(BOUNDARY_LIST, GL_COMPILE);
	::glColor3ub(GetRValue(m_crBoundary), GetGValue(m_crBoundary), GetBValue(m_crBoundary));

	::glBegin(GL_LINE_LOOP);
	
	::glVertex3d(-x,-y,-z);
	::glVertex3d(x-1,-y,-z);
	::glVertex3d(x-1,y-1,-z);
	::glVertex3d(-x,y-1,-z);
	::glEnd();

	::glBegin(GL_LINE_LOOP);
	::glVertex3d(-x,-y,z-1);
	::glVertex3d(x-1,-y,z-1);
	::glVertex3d(x-1,y-1,z-1);
	::glVertex3d(-x,y-1,z-1);
	::glEnd();

	::glBegin(GL_LINES);
	::glVertex3d(-x,-y,-z);
	::glVertex3d(-x,-y,z-1);

	::glVertex3d(x-1,-y,-z);
	::glVertex3d(x-1,-y,z-1);

	::glVertex3d(x-1,y-1,-z);
	::glVertex3d(x-1,y-1,z-1);

	::glVertex3d(-x,y-1,-z);
	::glVertex3d(-x,y-1,z-1);

	::glEnd();
	::glEndList();
	END_GL
}

void CSurfer::BuildAxisList() const throw()
{
	double x = m_dXrange/2;
	double y = m_dYrange/2;
	double z = m_dZrange/2;

	BEGIN_GL

	::glNewList(AXIS_LIST, GL_COMPILE);
	// x axis(x-coord)
	::glBegin(GL_LINES);
	::glColor3ub(255, 255, 255);
	::glVertex3d(-x, -y, -z);
	::glVertex3d(x, -y, -z);
	::glEnd();
	PrintString(m_strXtitle, x+10, -y-10, -z);

	// y axis(thickness)
	::glBegin(GL_LINES);
	::glColor3ub(255, 255, 255);
	::glVertex3d(-x, -y, -z);
	::glVertex3d(-x, y, -z);
	::glEnd();
	PrintString(m_strYtitle, -x, y+10, -z);

	// z axis(y-coord)
	::glBegin(GL_LINES);
	::glColor3ub(255, 255, 255);
	::glVertex3d(-x, -y, -z);
	::glVertex3d(-x, -y, z);
	::glEnd();
	PrintString(m_strZtitle, -x-20, -y-10, z+10);

	::glEndList();
	END_GL
}

void CSurfer::BuildDataList() const throw()
{
	BEGIN_GL
	C3DData* pdata;
	double xPos, yPos, thickness;
	double x, y, z;
	
	// We'll have only one C3DData*
	pdata = m_listData.front();

	::glNewList(DATA_LIST, GL_COMPILE);
	double i;
	for(double j=0; j<m_dZrange-1; j+=1) {
		::glBegin(GL_TRIANGLE_STRIP);
		i = 0;
		xPos = i;
		yPos = j;
		thickness = pdata->GetZdata((int)(i + m_dXrange*j));
		x = -m_dXrange/2+xPos;
		y = -m_dYrange/2+thickness;
		z = -m_dZrange/2+yPos;
		SetDataColor(thickness);
		::glVertex3f(x, y, z);
		for(i=0; i<m_dXrange; i+=1) {
			xPos = i;
			yPos = j+1;
			thickness = pdata->GetZdata((int)(i + m_dXrange*(j+1)));
			x = -m_dXrange/2+xPos;
			y = -m_dYrange/2+thickness;
			z = -m_dZrange/2+yPos;
			SetDataColor(thickness);
			::glVertex3f(x, y, z);

			xPos = i+1;
			yPos = j;
			thickness = pdata->GetZdata((int)(i + 1 + m_dXrange*j));
			x = -m_dXrange/2+xPos;
			y = -m_dYrange/2+thickness;
			z = -m_dZrange/2+yPos;
			SetDataColor(thickness);
			::glVertex3f(x, y, z);
		}
		::glEnd();
	}
	::glEndList();
	END_GL
}

void CSurfer::BuildWallList() const throw()
{
	BEGIN_GL
	C3DData* pdata;
	double xPos, yPos, thickness;
	double x, y, z;
	pdata = m_listData.front();

	::glNewList(WALL_LIST, GL_COMPILE);
	// x-t plane where y=0
	::glFrontFace(GL_CCW);
	::glBegin(GL_QUAD_STRIP);
	xPos = 0;
	yPos = 0;
	thickness = 0;
	x = -m_dXrange/2+xPos;
	y = -m_dYrange/2+thickness;
	z = -m_dZrange/2+yPos;
	SetDataColor(thickness);
	::glVertex3f(x, y, z);
	SetDataColor(thickness);
	thickness = pdata->GetZdata(0);
	y = -m_dYrange/2+thickness;
	::glVertex3f(x, y, z);
	int i;
	for(i=1; i<m_dXrange; i++)
	{
		xPos = (double)i;
		thickness = 0;
		x = -m_dXrange/2+xPos;
		y = -m_dYrange/2+thickness;
		SetDataColor(thickness);
		::glVertex3f(x, y, z);
		thickness = pdata->GetZdata(i);
		y = -m_dYrange/2+thickness;
		SetDataColor(thickness);
		::glVertex3f(x, y, z);
	}
	::glEnd();
	// x-t plane where y=height
	::glFrontFace(GL_CW);
	::glBegin(GL_QUAD_STRIP);
	xPos = 0;
	yPos = m_dZrange -1;
	thickness = 0;
	x = -m_dXrange/2+xPos;
	y = -m_dYrange/2+thickness;
	z = -m_dZrange/2+yPos;
	SetDataColor(thickness);
	::glVertex3f(x, y, z);
	thickness = pdata->GetZdata((int)(m_dXrange*(m_dZrange-1)));
	y = -m_dYrange/2+thickness;
	SetDataColor(thickness);
	::glVertex3f(x, y, z);
	for(i=1; i<m_dXrange; i++) 
	{
		xPos = (double)i;
		thickness = 0;
		x = -m_dXrange/2+xPos;
		y = -m_dYrange/2+thickness;
		SetDataColor(thickness);
		::glVertex3f(x, y, z);
		thickness = pdata->GetZdata((int)(m_dXrange*(m_dZrange-1) + i));
		y = -m_dYrange/2+thickness;
		SetDataColor(thickness);
		::glVertex3f(x, y, z);
	}
	::glEnd();
	// y-t plane where x=0
	::glFrontFace(GL_CW);
	::glBegin(GL_QUAD_STRIP);
	xPos = 0;
	yPos = 0;
	thickness = 0;
	x = -m_dXrange/2+xPos;
	y = -m_dYrange/2+thickness;
	z = -m_dZrange/2+yPos;
	SetDataColor(thickness);
	::glVertex3f(x, y, z);
	thickness = pdata->GetZdata(0);
	y = -m_dYrange/2+thickness;
	SetDataColor(thickness);
	::glVertex3f(x, y, z);
	for(i=1; i<m_dZrange; i++)
	{
		yPos = (double)i;
		thickness = 0;
		y = -m_dYrange/2+thickness;
		z = -m_dZrange/2+yPos;
		SetDataColor(thickness);
		::glVertex3f(x, y, z);
		thickness = pdata->GetZdata((int)(i*m_dXrange));
		y = -m_dYrange/2+thickness;
		SetDataColor(thickness);
		::glVertex3f(x, y, z);
	}
	::glEnd();
	// y-t plane where x=width
	::glFrontFace(GL_CCW);
	::glBegin(GL_QUAD_STRIP);
	xPos = m_dXrange-1;
	yPos = 0;
	thickness = 0;
	x = -m_dXrange/2+xPos;
	y = -m_dYrange/2+thickness;
	z = -m_dZrange/2+yPos;
	SetDataColor(thickness);
	::glVertex3f(x, y, z);
	thickness = pdata->GetZdata(0);
	y = -m_dYrange/2+thickness;
	SetDataColor(thickness);
	::glVertex3f(x, y, z);
	for(i=1; i<m_dZrange; i++)
	{
		yPos = (double)i;
		thickness = 0;
		y = -m_dYrange/2+thickness;
		z = -m_dZrange/2+yPos;
		SetDataColor(thickness);
		::glVertex3f(x, y, z);
		thickness = pdata->GetZdata((int)(i*m_dXrange + m_dXrange - 1));
		y = -m_dYrange/2+thickness;
		SetDataColor(thickness);
		::glVertex3f(x, y, z);
	}
	::glEnd();
	::glEndList();
	END_GL
}

void CSurfer::ShowLegend() const throw() 
{
	double x = m_dXrange/2;
	double y = m_dYrange/2;
	double z = m_dZrange/2;

	CRect rt;
	::GetClientRect(m_hWnd, &rt);
	BEGIN_GL
	::glPopMatrix();
	::glPushMatrix();
	::glTranslated(0.0, 0.0, -4);

	::glColor3f(1.0, 1.0, 1.0);
	::gluOrtho2D(0.0f, rt.right, 0, rt.bottom);
	::glBegin(GL_LINE_LOOP);
	::glVertex2f(rt.right+rt.right/4.f, rt.top-30.f);
	::glVertex2f(rt.right+rt.right/4.f, rt.bottom);
	::glVertex2f(rt.right+rt.right/4.f+300.f, rt.bottom);
	::glVertex2f(rt.right+rt.right/4.f+300.f, rt.top-30.f);
	::glEnd();
	int size = m_nNumOfLevels;
	int yoffset =0;
	string str;char buf[128];
	double interval=(m_ZMax-m_ZMin)/m_nNumOfLevels;
	for(int i=0; i<size; i++) 
	{
		::glColor3ub(GetRValue(Colors[i]), GetGValue(Colors[i]), GetBValue(Colors[i]));
		::glBegin(GL_QUADS);
		yoffset = rt.Height()*i/m_nNumOfLevels;
		::glVertex2f(rt.right+rt.right/4.f+10.0f,rt.bottom-yoffset-10.f);
		::glVertex2f(rt.right+rt.right/4.f+60.f,rt.bottom-yoffset-10.f);
		yoffset = rt.Height()*(i+1)/m_nNumOfLevels;
		::glVertex2f(rt.right+rt.right/4.f+60.f,rt.bottom-yoffset-10.f);
		::glVertex2f(rt.right+rt.right/4.f+10.f,rt.bottom-yoffset-10.f);
		::glEnd();
		if(i%m_nLegend==0)
		{
			::sprintf(buf,_T("%.1f-%.1f"), m_ZMin+i*interval,m_ZMin+(i+1)*interval);
			str = buf;
			PrintString(str, rt.right+rt.right/4.+70., rt.bottom-yoffset, 0.);
		}
	}
	END_GL
}

void CSurfer::SetDataColor(double thickness) const throw()
{
	if(m_dYrange == m_dThicknessMin) {
		BEGIN_GL
		::glColor3ub(255, 0, 0);
		END_GL
	}
	double ratio = (thickness-m_dThicknessMin) / (m_dYrange-m_dThicknessMin);
	int size = m_nNumOfLevels;//sizeof(Colors)/sizeof(Colors[0]);
	int nRatio = ratio * (size - 1);
	COLORREF col = Colors[nRatio];
	BEGIN_GL
	::glColor3ub(GetRValue(col), GetGValue(col), GetBValue(col));
	END_GL
}

CString CSurfer::GetWorkPath(CString flag)
{
	CString WorkPath;
	if(!GetLogTopRegString("SOFTWARE\\四川石油管理局测井公司\\测井解释平台\\User","WorkDisk",WorkPath))
		WorkPath="";
	else
		WorkPath+="\\"+flag+"\\";
	return WorkPath;
}

BOOL CSurfer::GetLogTopRegString(CString szSubKey,LPCTSTR lpszEntry,CString& strValue)
{
	ASSERT(lpszEntry != NULL);
	HKEY hSecKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKey, 0, KEY_QUERY_VALUE,
		&hSecKey) == ERROR_SUCCESS)
	{
		DWORD dwType, dwCount;
		LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
			NULL, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_BINARY);
			LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
				(LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);
			strValue.ReleaseBuffer();
		}
		RegCloseKey(hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_BINARY);
			return TRUE;
		}
	}
	return FALSE;
}
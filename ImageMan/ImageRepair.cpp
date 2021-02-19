/*Author: ZhongMing Zhao */
/*E-mail: Zhao978_cj@cnpc.com.cn */
/*Nov.30, 2018 */


#include "StdAfx.h"
#include "ImageRepair.h"
#include <memory.h>
#include <math.h>
#define MAX(a, b)  (((a) > (b)) ? (a) : (b)) 
#define MIN(a, b)  (((a) < (b)) ? (a) : (b)) 

CImageRepair::CImageRepair(int width,int height)
{
	m_width = width;
	m_height = height;

	m_nMark = new int[m_width*m_height];
	m_pConfid = new float[m_width*m_height];
	m_pPriority = new float[m_width*m_height];

	m_bSource = new bool[m_width*m_height];
	m_pData = new float[m_width*m_height];

	// initialize the rectangle area
	m_top = m_height;  
	m_bottom = 0; 
	m_left = m_width;
	m_right = 0;
}

CImageRepair::~CImageRepair(void)
{
}

void CImageRepair::CleanBuf(void)
{
	if(m_nMark) delete m_nMark;
	if(m_pConfid) delete m_pConfid;
	if(m_pPriority) delete m_pPriority;
	
	if(m_bSource) delete m_bSource;
	if(m_pData) delete m_pData;
}

bool CImageRepair::Process(void)
{
	memset(m_pConfid, 0, m_width*m_height*sizeof(float));
	memset(m_pPriority, 0, m_width*m_height*sizeof(float));
	
	DrawBoundary();
	DrawSource();
	
	int i,j;
	for(j= m_top; j<=m_bottom; j++)
	    for(i = m_left; i<= m_right; i++)
			if(m_nMark[j*m_width+i] == BOUNDARY)
				m_pPriority[j*m_width+i] = ComputePriority(i,j);//if it is boundary, calculate the priority

	float max_pri;
	int pri_x,pri_y, patch_x, patch_y;

	int k,m,n;
	for(k=0;k<2;k++)
		for(n= m_top; n<=m_bottom; n++)
			for(m = m_left; m<= m_right; m++)
				if(m_nMark[n*m_width+m]<0)
	{
		max_pri = 0.f;
		for(j= m_top; j<=m_bottom; j++)
			for(i = m_left; i<= m_right; i++)
			{
				// find the boundary pixel with highest priority
				if(m_nMark[j*m_width+i] == BOUNDARY&&m_pPriority[j*m_width+i]>max_pri)
				{
					pri_x = i;
					pri_y = j;
					max_pri = m_pPriority[j*m_width+i];
				}
			}
		// find the most similar source patch
		PatchTexture(pri_x, pri_y, patch_x, patch_y);  
		// inpaint this area and update confidence
		Update(pri_x, pri_y, patch_x,patch_y, ComputeConfidence(pri_x,pri_y));
		// update boundary near the changed area
		UpdateBoundary(pri_x, pri_y); 
		// update priority near the changed area
		UpdatePriority(pri_x, pri_y);  
	}
	return true;
}
// first time draw boundary
void CImageRepair::DrawBoundary(void)
{
	int x,y;
	for(y = 0; y<m_height; y++)
		for(x = 0; x<m_width; x++)
		{  
			if(m_pData[y*m_width+x]==-9999.f)//PAINT_COLOR)// if the pixel is specified as boundary
			{
				m_nMark[y*m_width+x] = TARGET;
				m_pConfid[y*m_width+x] = 0.f;
			}
			else 
			{
				m_nMark[y*m_width+x] = SOURCE;
				m_pConfid[y*m_width+x] = 1.f;
			}
		}

	for(y= 0; y< m_height; y++)
	    for(x = 0; x< m_width; x++)
		{
			if(m_nMark[y*m_width+x]==TARGET)
			{
				// resize the rectangle to the range of target area
				if(x<m_left) m_left = x; 
				if(x>m_right) m_right = x;
				if(y>m_bottom) m_bottom = y;
				if(y<m_top) m_top = y;
				//if one of the four neighbours is source pixel, then this should be a boundary
				if(y==m_height-1
					||y==0
					||x==0
					||x==m_width-1
					||m_nMark[(y-1)*m_width+x]==SOURCE
					||m_nMark[y*m_width+x-1]==SOURCE
					||m_nMark[y*m_width+x+1]==SOURCE
					||m_nMark[(y+1)*m_width+x]==SOURCE)
					m_nMark[y*m_width+x] = BOUNDARY;
			}
		}
}

bool CImageRepair::DrawSource(void) 
{
	// draw a window around the pixel, if all of the points within the window are source pixels, then this patch can be used as a source patch
	bool flag;
	int i,j,x,y;
	for(j = 0; j<m_height; j++)
	{
		for(i = 0; i<m_width; i++)
		{
			flag=true;
			if(i<WINSIZE||j<WINSIZE||i>=m_width-WINSIZE||j>=m_height-WINSIZE)
				m_bSource[j*m_width+i]=false;//cannot form a complete window
			else
			{
				for(y = j-WINSIZE; y<=j+WINSIZE; y++)
				{
					for(x = i-WINSIZE; x<=i+WINSIZE; x++)
					{
						if(m_nMark[y*m_width+x]!=SOURCE)
						{
							m_bSource[j*m_width+i]=false;
							flag = false;
							break;			
						}		
					}
					if(!flag) break;
				}
				if(flag) m_bSource[j*m_width+i]=true;
			}
		}
	}
	return true;
}

float CImageRepair::ComputePriority(int i, int j)
{
	float confidence, data;
	confidence = ComputeConfidence(i,j); // confidence term
	data = ComputeData(i,j);// data term
	return confidence*data;
}

float CImageRepair::ComputeConfidence(int i, int j) 
{
	int x,y;
	float confidence=0.f;
	for(y = MAX(j -WINSIZE,0); y<= MIN(j+WINSIZE,m_height-1); y++)
		for(x = MAX(i-WINSIZE,0); x<=MIN(i+WINSIZE, m_width-1); x++)
			confidence+= m_pConfid[y*m_width+x];
	confidence/= (WINSIZE*2+1)*(WINSIZE*2+1);
	return confidence;
}

float CImageRepair::ComputeData(int i, int j)
{
	norm tmp,tmp1;
	tmp1.x=0.f;
	tmp1.y=0.f;
	float magnitude;
	float max=0.f;
	int x, y;
	for(y = MAX(j -WINSIZE,1); y<= MIN(j+WINSIZE,m_height-2); y++)
		for( x = MAX(i-WINSIZE,1); x<=MIN(i+WINSIZE, m_width-2); x++)
		{
			// find the greatest gradient in this patch, this will be the gradient of this pixel(according to "detail paper")
			if(m_nMark[y*m_width+x]==SOURCE) // source pixel
			{
				//since I use four neighbors to calculate the gradient, make sure this four neighbors do not touch target region(big jump in tmp2ient)
				if(m_nMark[y*m_width+x+1]<0
					||m_nMark[y*m_width+x-1]<0
					||m_nMark[(y+1)*m_width+x]<0
					||m_nMark[(y-1)*m_width+x]<0)
					continue;
 				tmp = GetGradient(x,y); 
				magnitude = tmp.x*tmp.x+tmp.y*tmp.y;
				if(magnitude>max)
				{
					tmp1.x = tmp.x;
					tmp1.y = tmp.y;
					max = magnitude;
				}
			}
		}
	tmp = GetNorm(i,j);
	// perpendicular to the gradient: (x,y)->(y, -x)
	return fabs(tmp.x*tmp1.y-tmp.y*tmp1.x);// dot product	
}

norm CImageRepair::GetGradient(int i, int j)
{
	norm result;

	if(i==0) 
		result.x = m_pData[j*m_width+i+1] - m_pData[j*m_width+i];
	else if(i==m_width-1) 
		result.x = m_pData[j*m_width+i] - m_pData[j*m_width+i-1];
	else
		result.x = (m_pData[j*m_width+i+1] - m_pData[j*m_width+i-1])/2.f;
	if(j==0) 
		result.y = m_pData[(j+1)*m_width +i] - m_pData[j*m_width+i];
	else if(j==m_height-1) 
		result.y = m_pData[j*m_width +i] - m_pData[(j-1)*m_width+i];
	else
		result.y = (m_pData[(j+1)*m_width +i] - m_pData[(j-1)*m_width+i])/2.f;

	return result;
}

norm CImageRepair::GetNorm(int i, int j)
{
	norm result;
	int num=0;
	int neighbor_x[9];
	int neighbor_y[9];
	int record[9];
	int count = 0;
	int x,y;
	for(y = MAX(j-1,0); y<=MIN(j+1,m_height-1); y++)
		for(x = MAX(i-1,0); x<=MIN(i+1,m_width-1); x++)
		{
			count++;
			if(x==i&&y==j) continue;
			if(m_nMark[y*m_width+x]==BOUNDARY)
			{
				num++;
				neighbor_x[num] = x;
				neighbor_y[num] = y;		
				record[num]=count;
			}
		}
	
	// draw a line between the two neighbors of the boundary pixel, then the norm is the perpendicular to the line
	y = neighbor_x[2]-neighbor_x[1];
	x = neighbor_y[2]-neighbor_y[1];
	float square = pow((x*x*1.f+y*y),0.5f);
		
	result.x =x/square;
	result.y =y/square;
	return result;
}

bool CImageRepair::PatchTexture(int x, int y, int &patch_x, int &patch_y)
{
	float tmp;
	// find the most similar patch, according to SSD
	float min=99999999.f;
	float sum;
	int source_x, source_y;
	int target_x, target_y;
	int i,j,iter_x,iter_y;
	for(j = 0; j<m_height; j++)
		for(i = 0; i<m_width; i++)
		{		
			if(m_bSource[j*m_width+i]==false)
				continue;
			sum=0.f;
			for(iter_y=(-1)*WINSIZE; iter_y<=WINSIZE; iter_y++)
				for(iter_x=(-1)*WINSIZE; iter_x<=WINSIZE; iter_x++)
				{
					source_x = i+iter_x;
					source_y = j+iter_y;

					target_x = x+iter_x;
					target_y = y+iter_y;
					
					if(target_x<0||target_x>=m_width||target_y<0||target_y>=m_height)
						continue;

					if(m_nMark[target_y*m_width+target_x]==SOURCE)
					{
						tmp= m_pData[target_y*m_width+target_x]-m_pData[source_y*m_width+source_x];
						sum+= tmp*tmp;
					}
				}
			if(sum<min)
			{
				min=sum;
				patch_x = i;
				patch_y = j;
			}
		}
	return true;
}

bool CImageRepair::Update(int target_x, int target_y, int source_x, int source_y, float confid)
{
	int x0,y0,x1,y1,iter_x,iter_y;
	for(iter_y=(-1)*WINSIZE; iter_y<=WINSIZE; iter_y++)
	{
		for(iter_x=(-1)*WINSIZE; iter_x<=WINSIZE; iter_x++)
		{
			x0 = source_x+iter_x;
			y0 = source_y+iter_y;

			x1 = target_x + iter_x;
			y1 = target_y + iter_y;
			if(x1>m_width-1||y1>m_height-1||x0>m_width-1||y0>m_height-1) 
				continue;
			if(x1<0||y1<0||x0<0||y0<0) 
				continue;
			if(m_nMark[y1*m_width+x1]<0)
			{
				m_pData[y1*m_width+x1] = m_pData[y0*m_width+x0];
				m_pConfid[y1*m_width+x1] = confid; // update the confidence
			}
		}
	}
	return true;
}
// just update the area near the changed patch. (+-2 pixels)
void CImageRepair::UpdateBoundary(int i, int j)
{
	int x, y;
	for(y = MAX(j -WINSIZE-2,0); y<= MIN(j+WINSIZE+2,m_height-1); y++)
		for( x = MAX(i-WINSIZE-2,0); x<=MIN(i+WINSIZE+2, m_width-1); x++)
		{
			if(m_pData[y*m_width+x]==-9999.f)//PAINT_COLOR)
				m_nMark[y*m_width+x] = TARGET;
			else m_nMark[y*m_width+x] = SOURCE;
		}

	for(y = MAX(j -WINSIZE-2,0); y<= MIN(j+WINSIZE+2,m_height-1); y++)
		for( x = MAX(i-WINSIZE-2,0); x<=MIN(i+WINSIZE+2, m_width-1); x++)
		{
			if(m_nMark[y*m_width+x]==TARGET)
			{
				if(y==m_height-1||y==0||x==0||x==m_width-1
					||m_nMark[(y-1)*m_width+x]==SOURCE
					||m_nMark[y*m_width+x-1]==SOURCE
					||m_nMark[y*m_width+x+1]==SOURCE
					||m_nMark[(y+1)*m_width+x]==SOURCE)
					m_nMark[y*m_width+x] = BOUNDARY;
			}
		}		
}
// just update the area near the changed patch. (+-3 pixels)
void CImageRepair::UpdatePriority(int i, int j) 
{
	int x,y;
	for(y = MAX(j -WINSIZE-3,0); y<= MIN(j+WINSIZE+3,m_height-1); y++)
		for( x = MAX(i-WINSIZE-3,0); x<=MIN(i+WINSIZE+3, m_width-1); x++)
			if(m_nMark[y*m_width+x] == BOUNDARY)
				m_pPriority[y*m_width+x] = ComputePriority(x,y);

}

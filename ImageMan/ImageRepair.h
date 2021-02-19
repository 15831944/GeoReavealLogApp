#pragma once

/*Author: ZhongMing Zhao */
/*E-mail: Zhao978_cj@cnpc.com.cn */
/*Nov.30, 2018 */

#ifndef INPAINTING_H
#define INPAINTING_H

//#define PAINT_COLOR  0xffffff	//°×É« RGB(255,255,255)  // the color used to specify the target area
#define SOURCE 0
#define TARGET -1
#define BOUNDARY -2
#define WINSIZE 4  // the window size

typedef struct
{
	float x;
	float y;
}norm;  // the structure that record the norm

class CImageRepair
{
public:
	int m_width;  // image width
	int m_height; // image height

	// the rectangle of inpaint area
	int m_top, m_bottom, m_left, m_right; 

	float  *m_pData;
	int    *m_nMark;		// mark it as source(0) or to-be-inpainted area(-1) or bondary(-2).
	float  *m_pConfid;		// record the confidence for every pixel
	float  *m_pPriority;	// record the priority for pixels. only boudary pixels will be used
	bool   *m_bSource;		// whether this pixel can be used as an example texture center

	CImageRepair(int width,int height);
	~CImageRepair(void);
	void CleanBuf(void);

	bool Process(void);			// the main function to process the whole image
	void DrawBoundary(void);	// the first time to draw boundary on the image.

	float ComputeConfidence(int i, int j);	// the function to compute confidence
	float ComputePriority(int x, int y);	// the function to compute priority
	float ComputeData(int i, int j);		//the function to compute data item

	norm GetGradient(int i, int j);			// calculate the gradient at one pixel
	norm GetNorm(int i, int j);				// calculate the norm at one pixel
	bool DrawSource(void);					// find out all the pixels that can be used as an example texture center
	bool PatchTexture(int x, int y,int &patch_x,int &patch_y);// find the most similar patch from sources.
	bool Update(int target_x, int target_y, int source_x, int source_y, float confid);// inpaint this patch and update pixels' confidence within this area

	void UpdateBoundary(int i, int j);		// update boundary
	void UpdatePriority(int i, int j);		//update priority for boundary pixels.
};
#endif

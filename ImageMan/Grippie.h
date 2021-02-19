#pragma once

#define GRIPPIE_SQUARE_SIZE 11

class CGrippie : public CScrollBar
{
	DECLARE_DYNAMIC(CGrippie)
	CGrippie(const CGrippie& other);
	void operator=(const CGrippie& other);
public:
	CGrippie();
	virtual ~CGrippie();
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
public:
	BOOL Create(CWnd* parent);
	void OnParentSize(void);
private:
	CWnd* m_parent;
};


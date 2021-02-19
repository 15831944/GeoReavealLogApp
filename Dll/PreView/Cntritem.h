/******************************************************************
*   Cntritem.h - declarations of the cntritem class               *           
*******************************************************************/


#ifndef __CNTRITEM_H__
#define __CNTRITEM_H__

class LogPlotDoc;
class COleObj;

class CDrawItem : public COleClientItem
{
	DECLARE_SERIAL(CDrawItem)

// Constructors
public:
	CDrawItem(CLogPlotDoc* pContainer = NULL, COleObj* pDrawObj = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CLogPlotDoc* GetDocument()
		{ return (CLogPlotDoc*)COleClientItem::GetDocument(); }
	CView* GetActiveView()
		{ return (CView*)COleClientItem::GetActiveView(); }

	COleObj* m_pDrawObj;    // back pointer to OLE draw object

// Operations
	BOOL UpdateExtent();

// Implementation
public:
	~CDrawItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
	virtual void OnGetItemPosition(CRect& rPosition);

protected:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual void OnActivate();
};                    

#endif //__ENTRITEM_H__

/////////////////////////////////////////////////////////////////////////////

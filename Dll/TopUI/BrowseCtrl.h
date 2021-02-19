///////////////////////////////////////////////////////////////////////////
//				BrowseCtrl.h
//
// CBrowseCtrl is a CButton derived control used to let the user browse for
// files or folders on their systems.
//
// CBrowseCtrl integrates an edit-box and a browse-button together so the
// develops only need to maintain one control in their projects instead of two.

// Moreover, CBrowseCtrl draws its own "build-in" images on the browse-button
// without requiring the developers to include any extra icon, bitmap or other
// image resources.
//
// Written by Zhaozhongming (zzmslc@yahoo.com)
//
// History:
// 
// Jan 08, 2004 - Initial public release.
// Jan 09, 2004 - 1, Improved drawing functions to properly handle the case in
//                   which the control client area is too small to be drawn.
//                   In debug mode it will provide a message box to inform the
//                   developer about that issue.
//                2, Improved the edit box so it acts better upon receiving and
//                   losing the input focus. 
// Jan 10, 2004 - 1, Fixed a problem where the edit bix is not properly redrawn
//                   in some mouse events. Thanks to JOHN11.
//                2, Added method "GetSelectedCount" which returns the number of
//                   items the user has selected in the most recent file/folder
//                   dialog that was terminated by IDOK.
//                3, Improved the mouse/focus events monitoring functions
//                4, Fixed a drawing glitch when the user clicks on the edges of
//                   the edit box.
//                5, Changed the drawing rects calculating methods for performance
//                   improvement.
// Jan 14, 2004 - 1, Updated "SetPathName" and "GetPathName" member functions.
//                2, Altered the message sending method so the lparam is now a
//                   pointer to this CBrowseCtrl object.
// Jan 22, 2004 - 1, Added methods to monitor whether the user has manually changed
//                   the contents of the edit box when BC_CTL_ALLOWEDIT is set. The
//                   return value of "GetPathName" will also be properly affected.
//                2, The window titles of file/folder dialogs can now be accessed
//                   by calling "SetDialogTitle" and "GetDialogTitle".
//                3, The banner text of folder dialogs can now be accessed by calling
//                   "SetDialogBanner" and "GetDialogBanner".
//                4, Added method "ModifyButtonStyle" to allow convenient style changing.
// Feb 07, 2004 - 1, Improved drawing functions so that images/text are partially drawn
//                   if there is not enough space.
//
// May 22, 2004 - 1, Updated drawing functions.
//                2, Added functions: GetDriveLetter, GetDirectory, GetFileName
//                   GetFileTitle, GetFileExt.
//
/////////////////////////////////////////////////////////////////////////// 
/*
How to Use
You need to add source files BrowseCtrl.h and BrowseCtrl.cpp to your workspace first, 
and include BrowseCtrl.h wherever needed. To create the control, 
you can either use CBrowseCtrl::Create to create one at runtime, 
or draw a CButton on the dialog template and bind it with a CBrowseCtrl variable.

Code Samples
Specifying Control Styles
You may access the control styles anytime to change its appearance and behavior. 
Multiple style flags may be combined using the "|" operator. 
The valid style flags are:

Flag                             Effects
 
BC_CTL_ALLOWEDIT           Allows user to type in the editbox 
BC_CTL_FOLDERSONLY         Browse for folders instead of files 
BC_BTN_LEFT                Display the browse button on the left-side of the editbox 
BC_BTN_FLAT                Use flat button style 
BC_BTN_ICON                Use icon, no text will be displayed 
BC_ICO_ARROWFOLDER         The arrow-folder icon, must be combined with BC_BTN_ICON 
BC_ICO_FOLDER              The folder icon, must be combined with BC_BTN_ICON 
BC_ICO_EXPLORER            The Windows Explorer icon, must be combined with BC_BTN_ICON 

To set the "Windows Explorer" icon on the browse button, make the button flat, and finally,
 make the control "Browse for Folders" only:

DWORD dwStyle = m_wndBrowseCtrl.GetButtonStyle();
dwStyle |= BC_BTN_ICON;                    Make sure to display icon instead of text
dwStyle |= BC_BTN_FLAT;                    Flat button
dwStyle &= ~BC_ICO_ARROWFOLDER;            Remove other icons
dwStyle &= ~BC_ICO_FOLDER;
dwStyle |= BC_ICO_EXPLORER;                Set the explorer icon
dwStyle |= BC_CTL_FOLDERSONLY;             Browse for folders only
m_wndBrowseCtrl.SetButtonStyle(dwStyle);   Apply
I've got tired of images and want the browse button to display text, say, "Click Me!", 
also make it display tooltip "Click to browse your files.":

m_wndBrowseCtrl.SetButtonStyle(m_wndBrowseCtrl.GetButtonStyle() & ~BC_BTN_ICON);
m_wndBrowseCtrl.SetButtonText(_T("Click Me!"));
m_wndBrowseCtrl.SetTooltipText(_T("Click to browse your files."));
Browsing for Files and Folders
CBrowseCtrl handles the button events by itself so whenever the user clicks on the browse button, 
a file or folder dialog is popped up automatically. You can, of course, 
programmatically handle it in your code also, 
usually you may want to initialize the file dialog using 
whatever you would pass into the CFileDialog constructor. 
CBrowseCtrl provides a set of functions to allow doing the same initialization, 
you basically can just treat the control as a dialog:

m_wndBrowseCtrl.SetOpenSave(TRUE);                           TRUE-"open", FALSE-"save as"
m_wndBrowseCtrl.SetDefExt(NULL);                             Do not use any default extension
m_wndBrowseCtrl.SetFilter(_T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"));
m_wndBrowseCtrl.SetFileFlags(OFN_FILEMUSTEXIST | OFN_HIDEREADONLY);
m_wndBrowseCtrl.SetPathName(_T("c:\\MyApp\\readme.txt"));    default path

if (m_wndBrowseCtrl.DoModal() == IDOK)
    MessageBox(m_wndBrowseCtrl.GetPathName());
To browse for folders:

m_wndBrowseCtrl.SetButtonStyle(m_wndBrowseCtrl.GetButtonStyle() 
                                            | BC_CTL_FOLDERSONLY);
// Display an editbox on the dialog
m_wndBrowseCtrl.SetFolderFlags(BIF_EDITBOX); 
m_wndBrowseCtrl.SetFolderDialogTitle(_T("Please select a folder"));

if (m_wndBrowseCtrl.DoModal() == IDOK)
    MessageBox(m_wndBrowseCtrl.GetPathName());
And if the user has selected multiple files from the file dialog,
 you can iterate through all path names by the following code:

if (m_wndBrowseCtrl.GetSelectedCount() > 1)// Multiple files selected
{
    POSITION pos = m_wndBrowseCtrl.GetStartPosition();
    while (pos != NULL)
    {
        CString sPathName = m_wndBrowseCtrl.GetNextPathName(pos);
        // Process the path name...
    }
}
Notifying the Parent Window
After the file/folder dialog is closed by the user, 
a notifying message is sent to the parent window. 
The wParam is either IDOK or IDCANCEL, the lParam is a pointer to this CBrowseCtrl object.

To make the control notify the parent window, 
you need to provide CBrowseCtrl a custom window message which is greater than 0. 
If the message you provide is 0, the message will not be sent. 
If you do not provide any message, no message will be sent either.

#define WM_BROWSE_NOTIFY WM_APP + 100
m_wndBrowseCtrl.SetNotifyMessageID(WM_BROWSE_NOTIFY);
// Now ready to receive the notification!
*********************************************************************************************/
#ifndef __BROWSECTRL_H__
#define __BROWSECTRL_H__

/////////////////////////////////////////////////////////////////////
//		Browse Button Style Flags
/////////////////////////////////////////////////////////////////////
#define BC_BTN_LEFT			0x0001 // Display the browse button on the left-side of the editbox
#define BC_BTN_ICON			0x0002 // Use icon, no text will be displayed
#define BC_BTN_FLAT			0x0004 // Use flat button style
#define BC_ICO_ARROWFOLDER	0x0008 // The arrow-folder icon, must be combined with BC_BTN_ICON
#define BC_ICO_FOLDER		0x0010 // The folder icon, must be combined with BC_BTN_ICON
#define BC_ICO_EXPLORER		0x0020 // The Windows Explorer icon, must be combined with BC_BTN_ICON
#define BC_CTL_ALLOWEDIT	0x0040 // Allow user to type in the editbox
#define BC_CTL_FOLDERSONLY	0x0080 // Browse for folder

class AFX_EXT_CLASS CBrowseCtrl : public CButton
{
public:	

	/////////////////////////////////////////////////////////////////////
	//		Constructor & Destructor
	/////////////////////////////////////////////////////////////////////

	CBrowseCtrl();
	virtual ~CBrowseCtrl();

	/////////////////////////////////////////////////////////////////////
	//		Runtime Creation
	/////////////////////////////////////////////////////////////////////

	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwBtnStyles = BC_BTN_ICON | BC_ICO_ARROWFOLDER, UINT nNotifyMessageID = 0);

	/////////////////////////////////////////////////////////////////////
	//		Operations
	/////////////////////////////////////////////////////////////////////

	int DoModal(); // Popup the file/folder browse dialog

	/////////////////////////////////////////////////////////////////////
	//		General Control Settings
	/////////////////////////////////////////////////////////////////////
	
	void SetButtonStyle(DWORD dwStyles); // The browse-button styles.
	DWORD GetButtonStyle() const;
	void ModifyButtonStyle(DWORD dwRemove, DWORD dwAdd);
	void SetButtonText(LPCTSTR lpszText); // The button text
	CString GetButtonText() const;	
	void SetTooltipText(LPCTSTR lpszText); // Button tooltip text
	CString GetTooltipText() const;		
	void SetNotifyMessageID(UINT nNotifyMsgID); // Sent to the parent window when dialog closes
	UINT GetNotifyMessageID() const;	
	BOOL IsEditManuallyChanged() const; // Whether the user has manually changed the contents of the edit box

	/////////////////////////////////////////////////////////////////////
	//		Browse Dialog Settings
	/////////////////////////////////////////////////////////////////////

	// The string displays on the edit control, also will be the default selected
	// file/folder when the dialog pops up
	void SetPathName(LPCTSTR lpszPathName);
	
	void SetOpenSave(BOOL bOpen); // "Open" or "Save As", CFileDialog only
	BOOL GetOpenSave() const;
	void SetDefExt(LPCTSTR lpszDefExt); // Default file extension, CFileDialog only
	CString GetDefExt() const;
	void SetFilter(LPCTSTR lpszFilter); // File extension filters, CFileDialog only
	CString GetFilter() const;
	void SetFileFlags(DWORD dwFlags); // File dialog flags, CFileDialog only
	DWORD GetFileFlags() const;
	void SetFolderFlags(UINT nFlags); // Folder dialog flags, folder dialog only
	UINT GetFolderFlags() const;
	
	// The text appear on the file/folder dialog title bar, if lpTitle is NULL or an
	// empty string, the default title is used.
	void SetDialogTitle(LPCTSTR lpszTitle);
	CString GetDialogTitle() const;

	// The banner text on the folder dialog.
	void SetDialogBanner(LPCTSTR lpszBanner);
	CString GetDialogBanner() const;
	
	/////////////////////////////////////////////////////////////////////
	//		Browse Result Data Access
	/////////////////////////////////////////////////////////////////////
	int GetSelectedCount() const; // How many files did the user select?
	CString GetPathName() const; // The full path name	
	CString GetDriveLetter() const; // Drive letter, e.g. "C"
	CString GetDirectory() const; // Folder path, e.g. "C:\\My Document"
	CString GetFileName() const; // File name with extension, files only
	CString GetFileTitle() const; // File name without extension, files only
	CString GetFileExt() const; // File extension, files only
	POSITION GetStartPosition() const;	// The path-name start position, for multiple selection 
	CString GetNextPathName(POSITION& pos) const;	// Next path name
	BOOL GetReadOnlyPref() const; // The "Read-Only" checkbox on dialog, CFileDialog only

	/////////////////////////////////////////////////////////////////////
	//		Overloads
	/////////////////////////////////////////////////////////////////////
	void SetWindowText(LPCTSTR lpszString);
	void GetWindowText(CString& rString) const;
	int GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const;	

protected:

	/////////////////////////////////////////////////////////////////////
	//		Protected Methods
	/////////////////////////////////////////////////////////////////////
	BOOL CreateCtrl(); // Initialize the control
	
	// Button drawing
	void PrepareMemDC(CDC* pMemDC, CBitmap* pBmp, CDC* pSrcDC, const CRect& srcRect, COLORREF bkColor) const;
	void DrawButtonExplorer(CDC* pDC, COLORREF bkColor, const CRect& rect) const; // Draw the explorer icon on the button
	void DrawButtonFolder(CDC* pDC, COLORREF bkColor, const CRect& rect) const; // Draw the folder icon on the button
	void DrawButtonArrowFolder(CDC* pDC, COLORREF bkColor, const CRect& rect) const; // Draw the arrow-folder icon on the button
	void DrawButtonText(CDC* pDC, COLORREF bkColor, const CRect& rect) const; // Draw text on the button
	void DrawButtonFrame(CDC* pDC, COLORREF bkColor) const; // Draw the button frame and background

	// File/Folder dialog popup
	int FileDoModal();
	int FolderDoModal();

	// Folder dialog call back function
	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT nMsg, LPARAM lParam, LPARAM lpData);
	
	// Edit control
	BOOL CreateEdit();
	
	// Mouse/input events
	void OnMouseEnter(); // The mouse cursor has entered the browse button area
	void OnMouseLeave(); // The mouse cursor has left the browse button area
	void OnSetEditFocus(); // The input focus has entered the edit control
	void OnKillEditFocus(); // The input focus has left the edit control

	void RecalculateRects(BOOL bMoveEdit = FALSE); // Recalculate drawing rects
	void OnButtonClicked();
	static BOOL SplitPath(LPCTSTR lpszPathName, CString* pStrDrvLetter, CString* pStrFolder, CString* pStrFileName, CString* pStrFileTitle, CString* pStrFileExt);

	/////////////////////////////////////////////////////////////////////
	//		Member Data
	/////////////////////////////////////////////////////////////////////

	// If greater than 0, this message is sent to the parent window when the
	// browse dialog is closed, wParam is either IDOK or IDCANCEL, lParam 
	// is a pointer to this CBrowseCtrl object.
	UINT m_nNotifyMsg; 

	CRect m_rcButton; // Rect of the browse button
	CRect m_rcContent; // Rect of the actual drawing area
	CRect m_rcEdit; // Rect of the edit box
	CEdit m_wndEdit; // The edit control which displays browse results
	BOOL m_bEditFocused; // The edit control is currently focused
	BOOL m_bOwnCreate; // Set to TRUE when the "Create" method has been called
	DWORD m_dwStyle; // The control styles
	BOOL m_bBtnPushed; // Set to TRUE if the button has been pused down, FALSE when released.
	BOOL m_bMouseIn; // Set to TRUE if the mouse cursor has entered the button area
	CToolTipCtrl m_wndTooltip; // The tooltip control for the button
	CString m_sTootipText; // Tooltip text, can be changed at fly
	CString m_sButtonText; // The button text, only displayed BC_BTN_ICON flag is not set

	// stuff for the file/folder dialog
	BOOL m_bDlgPopped; // The file/folder dialog has been popped up
	CString m_sDlgTitle; // To display on the file/folder dialog title bar
	CString m_sDlgBanner; //  To display a banner text on the folder dialog
	int m_nPathNamesLen; // Used when multiple files are selected, CFileDialog only
	BOOL m_bOpenFileDialog; // "Open" or "Save as", CFileDialog only
	DWORD m_dwFileFlags; // Dialog flags, CFileDialog only
	UINT m_nFolderFlags; // Dialog flags, folder dialog only
	BOOL m_bReadOnlyRef; // The "Open as Read Only" mark, CFileDialog only
	CString m_sDefExt; // Default file extension, CFileDialog only
	CString m_sFilter; // Default filters, CFileDialog only
	LPTSTR m_lpszPathNames; // Default path name, CFileDialog only
	int m_nSelCount;	// How many files did the user select?
	BOOL m_bEditChanged; // To monitor the changes made to the edit box by direct user input
	CString m_sEditText; // To record text changes in the edit box, needed if the control is destroyed before calling GetPathName

	/////////////////////////////////////////////////////////////////////
	//		Wizard Generated Stuff
	/////////////////////////////////////////////////////////////////////

	//{{AFX_VIRTUAL(CBrowseCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	//{{AFX_MSG(CBrowseCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg void OnChangeEdit();
	DECLARE_MESSAGE_MAP()
};

#endif // #ifndef __BROWSECTRL_H__

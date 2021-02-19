#if !defined(AFX_DBASEFILE_H__6B932114_A89A_11D2_8565_444553540000__INCLUDED_)
#define AFX_DBASEFILE_H__6B932114_A89A_11D2_8565_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define		FIELD_REC_LENGTH	32			/*	length of field description record	*/
#define		HEADER_LENGTH		32			/*	length of header without field desc	*/
#define		MEMO_BLOCK_SIZE		512			/*  memo block size (dBase III) */

/* error codes */
#define		DBASE_OUT_OF_MEM		100		/*	not enough memory error message */
#define		DBASE_NO_FILE			101		/*	file not found error message */
#define		DBASE_BAD_FORMAT		102		/*	file is not a dBASE III file */
#define		DBASE_INVALID_RECORD	103		/*	requested record does not exist */
#define		DBASE_EOF				104		/*  the end of the file */
#define		DBASE_BOF				105		/*  the begin of the file */
#define		DBASE_NOT_OPEN			106		/*  file is not open */ 
#define		DBASE_INVALID_FIELDNO	107		/*  invalid field number */
#define		DBASE_INVALID_DATA		108		/*	invalid data error message */
#define		DBASE_SUCCESS			109		/*	operation succesfull */
#define		DBASE_NOT_FOUND			110		/*	record not found */
#define		DBASE_WRITE_ERROR		111		/*  write error */
#define		DBASE_READ_ERROR		112		/*  read error */
#define		DBASE_INVALID_BLOCK_NO	113		/*  invalid memo block number */
#define		DBASE_NO_MEMO_DATA		114		/*  no memo data available */

class CField : public CObject
{
	DECLARE_DYNCREATE(CField)
//private :
public :
	char	 		m_Name[11];		// field name in ASCII zero-filled 
	char	 		m_Type;			// field type in ASCII 
	unsigned char	m_Length;		// field length in binary 
	unsigned char 	m_DecCount;		// field decimal count in binary 
	unsigned char	m_FieldNumber;	// field number within the record 
	char*			m_DataAddress;	// field data address

public :
	CField();
	CField(LPCTSTR lpszName, char Type, unsigned char Length,unsigned char DecCount, unsigned char FieldNum);
	~CField();
};


class AFX_EXT_CLASS CDbaseFile : public CObject
{
	DECLARE_DYNCREATE(CDbaseFile)

	CDbaseFile();           
	~CDbaseFile();
// Attributes
public:	
	DWORD m_nRecordCount;			// number of records in datafile
	DWORD m_nCurrentRecord;			// current record in memory 
	UCHAR	m_nFieldCount;			// number of fields 
	CString	m_strFileName;			// Name of data file 
private:	
	CFile	m_DatabaseFile;
	UCHAR	m_Version;				// dbf version number
	UCHAR	m_LastUpdateYY;			// date of last update - year
	UCHAR	m_LastUpdateMM;			// date of last update - month
	UCHAR	m_LastUpdateDD;			// date of last update - day
	
	short	m_HeaderLength;			// length of header structure
	short	m_RecordLength;			// length of a record

	BOOL m_bModified;				// has database contents changed?
	CObList	m_FieldList;			// linked list with field structure
	char *m_RecordDataPtr;			// pointer to current record struct

// Operations
public:
	int Open(LPCTSTR lpszFileName);
	int Close();
	int Create(LPCTSTR lpszFileName, CStringArray &strFieldArray);
	BOOL CloneDatabase(LPCTSTR lpszCloneName, BOOL bCopyRecords = FALSE, BOOL bSkipDeleted = FALSE);

	CString GetLastUpdate();
	void GetLastError(LPTSTR lpszErrorMessage);

	int GetRecord(DWORD nRecordIndex);
	int PutRecord(DWORD nRecordIndex);

	int DeleteRecord(BOOL bDelete = TRUE);
	int AddRecord();
	int InsertRecord(DWORD nRecordIndex);

	int GetFirstRecord();
	int GetLastRecord();
	int GetNextRecord();
	int GetPrevRecord();
	BOOL IsRecordDeleted();

	// field operations
	CField *GetFieldPtr(int nFieldIndex);
	CField *GetFieldPtr(LPCTSTR lpszFieldName);
	int GetField(int nFieldIndex, char *buff, char type = '?');
	int GetField(LPCTSTR lpszFieldName, char *buff, char type = '?');
	int GetDateField(int nFieldIndex, char *buff);
	int GetDateField(LPCTSTR lpszFieldName, char *buff);
	CString GetCharField(int nFieldIndex);
	CString GetCharField(LPCTSTR lpszFieldName);

	COleDateTime GetDateField(int nFieldIndex);
	COleDateTime GetDateField(LPCTSTR lpszFieldName);
	long GetNumericField(int nFieldIndex);
	long GetNumericField(LPCTSTR lpszFieldName);
	double GetFloatField(int nFieldIndex);
	double GetFloatField(LPCTSTR lpszFieldName);
	BOOL GetLogicalField(int nFieldIndex);
	BOOL GetLogicalField(LPCTSTR lpszFieldName);

	int PutField(int nFieldIndex, char *buff);
	int PutField(LPCTSTR lpszFieldName, char *buff);
	int PutDateField(int nFieldIndex, char *buff, int baseyear = 1900);
	int PutDateField(LPCTSTR lpszFieldName, char *buff, int baseyear = 1900);
	int PutNumericField(int nFieldIndex, long value);
	int PutNumericField(LPCTSTR lpszFieldName, long value);
	int PutFloatField(int nFieldIndex, double value);
	int PutFloatField(LPCTSTR lpszFieldName, double value);
	int PutCharField(int nFieldIndex, CString str);
	int PutCharField(LPCTSTR lpszFieldName, CString str);
	int PutLogicalField(int nFieldIndex, BOOL bValue);
	int PutLogicalField(LPCTSTR lpszFieldName, BOOL bValue);

	DWORD SearchRecord(LPCTSTR lpszFieldName, LPCTSTR lpszCriteria);
	DWORD SearchRecord(int nFieldIndex, LPCTSTR lpszCriteria);
	DWORD SearchFromRecord(LPCTSTR lpszFieldName, LPCTSTR lpszCriteria, DWORD nStartRec);
	DWORD SearchFromRecord(int nFieldIndex, LPCTSTR lpszCriteria, DWORD nStartRec);

	int Pack();

	DWORD GetCurRecNo() { return m_nCurrentRecord; }
	DWORD GetRecordCount() { return m_nRecordCount; }
	long GetFieldCount() { return m_nFieldCount; }

	BOOL IsValidDate(char *pData);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDbaseFile)
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_strErrorMessage;

	// MEMO field related variables
	CFile m_MemoFile;
	DWORD m_dwMemoBlockSize;
	DWORD m_dwMemoNextFreeBlock;
	char *m_MemoDataPtr;

public:
	CString GetMemoFileName(LPCTSTR lpszFileName = NULL);
	int OpenMemoFile(LPCTSTR lpszFileName = NULL);
	int CreateMemoFile();
	void CloseMemoFile();
	DWORD GetMemoFieldLength(int nFieldIndex);
	DWORD GetMemoFieldLength(LPCTSTR lpszFieldName);
	int GetMemoField(int nFieldIndex, char *buff, DWORD dwLength);
	int GetMemoField(LPCTSTR lpszFieldName, char *buff, DWORD dwLength);
	int PutMemoField(int nFieldIndex, char* buff, DWORD dwLength);
	int PutMemoField(LPCTSTR lpszFieldName, char* buff, DWORD dwLength);
	int ClearMemoField(int nFieldIndex);
	int ClearMemoField(LPCTSTR lpszFieldName);
};

class AFX_EXT_CLASS CDBase  
{
public:
	CDBase();
	CDBase(LPCTSTR lpszPathName);
	virtual ~CDBase();
// Attributes
public:
	CDbaseFile *m_dBaseFile;
	CString m_FileName;
// Operations
public:
	BOOL NewDBase();
	BOOL OpenDBase(LPCTSTR lpszPathName);
	void CloseDBase();
	BOOL SaveDBase(LPCTSTR lpszPathName);

// Implementation
public:
	BOOL SetFilter(LPCTSTR lpszFieldName, LPCTSTR lpszCriteria);
public:
	void FileStructure();
	void FilePackdatabase();
	void FileExport();
protected:

protected:
	void ExportToHTML(LPCTSTR lpszFileName);
	void ExportToText(LPCTSTR lpszFileName);
	BOOL CopyBackupData(LPCTSTR lpszBackupFile);
	char m_szTempFileName[MAX_PATH];
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBASEFILE_H__6B932114_A89A_11D2_8565_444553540000__INCLUDED_)

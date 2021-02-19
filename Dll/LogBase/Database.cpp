/****************************************************************/
/*																*/
/*  DataBase.cpp												*/
/*																*/
/*  Implementation of the CMainFrame class.						*/
/*	This file is part of the DBF Explorer application.			*/
/*																*/
/*  Last updated: 25 Feb. 2004  								*/
/*																*/
/****************************************************************/


#include "stdafx.h"
#include "DataBase.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/****************************************************************/
/*																*/
/* Remove all trailing and leading spaces.						*/
/*																*/
/****************************************************************/
char *TrimSpace(char *str)
{
   char  *pEnd;

   while (*str == ' ')
   {
      lstrcpy(str, str + 1);
   }
   pEnd = str + lstrlen(str) - 1;
   while (*pEnd == ' ')
   {
      *pEnd-- = '\0';
   }
   return (str);
}


IMPLEMENT_DYNCREATE(CField, CObject)


// Default constructor
CField::CField()
{	
	ZeroMemory(m_Name, 11);
	m_Name[0] = '\0';
	m_Type = 'C';
	m_Length = 0;
	m_DecCount = 0;
	m_FieldNumber = 0;
	m_DataAddress = NULL;
}


// Constructor with an initialization of most data members
CField::CField(LPCTSTR lpszName, char Type, unsigned char Length, unsigned char DecCount, unsigned char FieldNum)
{	
	ZeroMemory(m_Name, 11);
	lstrcpy(m_Name, lpszName);
	m_Type = Type;
	m_Length = Length;
	m_DecCount = DecCount;
	m_FieldNumber = FieldNum;
}


// Destructor
CField::~CField()
{
}



IMPLEMENT_DYNCREATE(CDbaseFile, CObject)


CDbaseFile::CDbaseFile()
{
	m_strErrorMessage = "";
	m_DatabaseFile.m_hFile = NULL;
	m_nFieldCount = 0;

	// MEMO field stuff
	m_dwMemoNextFreeBlock = 1;
	m_MemoFile.m_hFile = NULL;
	m_MemoDataPtr = NULL;
	m_dwMemoBlockSize = MEMO_BLOCK_SIZE;
}

CDbaseFile::~CDbaseFile()
{
	if (m_DatabaseFile.m_hFile != NULL) 
	{
		Close();
	}
}


/****************************************************************/
/*																*/
/* Open dBase file, read dbf header and fill field list.		*/
/*																*/
/****************************************************************/
int CDbaseFile::Open(LPCTSTR lpszFileName)
{
	m_nFieldCount = 0;

	m_strFileName.Format("%s", lpszFileName);

	try
	{
		// open database file
		if (!m_DatabaseFile.Open(lpszFileName, CFile::modeReadWrite | CFile::shareDenyNone))
		{
 			m_strErrorMessage = "Unable to open file";
			m_DatabaseFile.m_hFile = NULL;
			return DBASE_NO_FILE;
		}
		// set file pointer to begin of file
		m_DatabaseFile.SeekToBegin();
	
		// dbf version
		m_DatabaseFile.Read(&m_Version, 1);
		// update year
		m_DatabaseFile.Read(&m_LastUpdateYY, 1);
		// update month
		m_DatabaseFile.Read(&m_LastUpdateMM, 1);
		// update day
		m_DatabaseFile.Read(&m_LastUpdateDD, 1);
		// number of records in datafile
		m_DatabaseFile.Read(&m_nRecordCount, 4);
		// length of header structure 
		m_DatabaseFile.Read(&m_HeaderLength, 2);
		// length of the record
		m_DatabaseFile.Read(&m_RecordLength, 2);
		
		// check if date is correct
		if (m_LastUpdateMM == 0)
		{
  			m_strErrorMessage = "Bad format";
			// file is bad format
			m_DatabaseFile.Close();
			m_DatabaseFile.m_hFile = NULL;
			return DBASE_BAD_FORMAT;
		}

		m_nCurrentRecord = 0;
		
		// calculate number of fields
		m_nFieldCount = ((m_HeaderLength - (FIELD_REC_LENGTH+1)) / HEADER_LENGTH);

		// allocate memory for record buff
		if((m_RecordDataPtr = (char *)malloc(m_RecordLength)) == NULL)
		{
	  		m_strErrorMessage = "Out of memory";
			m_DatabaseFile.Close();
			m_DatabaseFile.m_hFile = NULL;
			return DBASE_OUT_OF_MEM;
		}
		// set pointer to field description
		m_DatabaseFile.Seek(HEADER_LENGTH, CFile::begin);
		
		int iAddressOffset = 1;

		// read field structure into memory
		for(int i=0; i< m_nFieldCount; i++)
		{
			// create new field
			CField *pField = new CField;
			
			// add field to linked list
			m_FieldList.AddTail(pField);
			
			// get field name
			m_DatabaseFile.Read(&pField->m_Name, 11);
			// get field type
			m_DatabaseFile.Read(&pField->m_Type, 1);
			// get field data address
			pField->m_DataAddress = m_RecordDataPtr + iAddressOffset;
			// skip field address offset
			m_DatabaseFile.Seek(4, CFile::current);
			// get field length
			m_DatabaseFile.Read(&pField->m_Length, 1);
			// get field decimal count
			m_DatabaseFile.Read(&pField->m_DecCount, 1);
			// set field number
			pField->m_FieldNumber = i+1;
			// skip reserved bytes
			m_DatabaseFile.Seek(14, CFile::current);
			iAddressOffset += pField->m_Length;
		}
		
		// not modified yet
		m_bModified = FALSE;

		// memo file ?
		if (m_Version == 0x83)
		{
			OpenMemoFile();
		}
		// open is succesfull 
		return DBASE_SUCCESS;
	}
	catch(CFileException *e)
	{
		e->Delete();
		if (m_DatabaseFile.m_hFile != NULL)
			m_DatabaseFile.Close();
		m_DatabaseFile.m_hFile = NULL;
		return DBASE_NO_FILE;
	}
}


/****************************************************************/
/*																*/
/* Close and Save dBase file.									*/
/*																*/
/****************************************************************/
int CDbaseFile::Close()
{
	if (m_DatabaseFile.m_hFile == NULL)
		return DBASE_SUCCESS;

	CloseMemoFile();

	if(m_bModified)
	{
		SYSTEMTIME sysTime;

		GetSystemTime(&sysTime);
		m_LastUpdateDD = (UCHAR)sysTime.wDay;
		m_LastUpdateMM = (UCHAR)sysTime.wMonth;
		// 1900 is base year -> 1997 = 61h, 2001 -> 65h, so a valid range is 1900 - 2155 
		m_LastUpdateYY = (UCHAR)sysTime.wYear-1900;	

		// set file pointer to begin of file 
		m_DatabaseFile.SeekToBegin();
		
		// dbf version
		m_DatabaseFile.Write(&m_Version, 1);
		// write last updated date
		m_DatabaseFile.Write(&m_LastUpdateYY, 1);
		m_DatabaseFile.Write(&m_LastUpdateMM, 1);
		m_DatabaseFile.Write(&m_LastUpdateDD, 1);
		// number of records in datafile
		m_DatabaseFile.Write(&m_nRecordCount, 4);
		// length of header structure 
		m_DatabaseFile.Write(&m_HeaderLength, 2);
		// length of the record
		m_DatabaseFile.Write(&m_RecordLength, 2);

		// set pointer to end of file
		m_DatabaseFile.SeekToEnd();
		// write eof character
//		m_DatabaseFile.Write("\0x1a", 1);
	}

	// Iterate over the list, delete each field
	while (!m_FieldList.IsEmpty())
	{
		CField* pField = (CField *)m_FieldList.RemoveHead();
		delete pField;
	}
	m_nFieldCount = 0;
	
	// free record memory
	free(m_RecordDataPtr);
	// close file
	m_DatabaseFile.Close();
	m_DatabaseFile.m_hFile = NULL;
	return DBASE_SUCCESS;
}


/****************************************************************/
/*																*/
/* This function checks if a date is valid.						*/
/*																*/
/****************************************************************/
BOOL CDbaseFile::IsValidDate(char *buff)
{
	if (lstrlen(buff) != 8)
		return FALSE;

	COleDateTime Datum;
	int year, month, day;

	sscanf(buff,"%04d%02d%02d", &year, &month, &day);
	Datum.SetDate(year, month, day);
	if(Datum.GetStatus() == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/****************************************************************/
/*																*/
/* Get last update date.										*/
/*																*/
/****************************************************************/
CString CDbaseFile::GetLastUpdate()
{
	CString strResult;
	strResult.Format("%02d/%02d/%04d", m_LastUpdateMM, m_LastUpdateDD, m_LastUpdateYY+1900);
	return strResult; 
}


/****************************************************************/
/*																*/
/* Load record from dBase file into memory.						*/
/*																*/
/****************************************************************/
int CDbaseFile::GetRecord(DWORD nRecordIndex)
{
	if (m_DatabaseFile.m_hFile == NULL)
		return DBASE_NOT_OPEN;

	// valid record ?
	if(nRecordIndex > m_nRecordCount || nRecordIndex < 1)
	{
		m_strErrorMessage = "Invalid record";
		return DBASE_INVALID_RECORD;
	}
	try
	{
		// set file pointer to requested record
		m_DatabaseFile.Seek(((DWORD)m_HeaderLength + ((nRecordIndex - 1) * m_RecordLength)), CFile::begin);
		// read record into memory
		m_DatabaseFile.Read(m_RecordDataPtr, m_RecordLength);
		// make 'nRecordIndex' current record
		m_nCurrentRecord = nRecordIndex;
		return DBASE_SUCCESS;
	}
	catch(CFileException *e)
	{
		e->Delete();
		m_strErrorMessage = "Invalid record";
		return DBASE_INVALID_RECORD;
	}
}


/****************************************************************/
/*																*/
/* Write record to file (update record).						*/
/*																*/
/****************************************************************/
int CDbaseFile::PutRecord(DWORD nRecordIndex)
{
	if (m_DatabaseFile.m_hFile == NULL)
		return DBASE_NOT_OPEN;

	// valid record ?
	if(nRecordIndex > m_nRecordCount || nRecordIndex < 1)
	{
		m_strErrorMessage = "Invalid record";
		return DBASE_INVALID_RECORD;
	}
	try
	{
		// set file pointer to requested record
		m_DatabaseFile.Seek(((DWORD)m_HeaderLength + ((nRecordIndex - 1) * m_RecordLength)), CFile::begin);
		// write memory contents to file
		m_DatabaseFile.Write(m_RecordDataPtr, m_RecordLength);
		// database file is modified
		m_bModified = TRUE;
		m_nCurrentRecord = nRecordIndex;
		return DBASE_SUCCESS;
	}
	catch(CFileException *e)
	{
		e->Delete();
		m_strErrorMessage = "Invalid record";
		return DBASE_INVALID_RECORD;
	}
}


/****************************************************************/
/*																*/
/* Mark record as "DELETED" or "UNDELETED".						*/
/*																*/
/****************************************************************/
int CDbaseFile::DeleteRecord(BOOL bDelete)
{
	if(m_RecordDataPtr) 
	{
		if (bDelete)
			m_RecordDataPtr[0] = '*';
		else
			m_RecordDataPtr[0] = ' ';

		m_bModified = TRUE;
		PutRecord(GetCurRecNo());
		return DBASE_SUCCESS;
	}
	else
	{
		return DBASE_INVALID_RECORD;
	}
}


/****************************************************************/
/*																*/
/* Check if record is marker 'DELETED'.							*/
/*																*/
/****************************************************************/
BOOL CDbaseFile::IsRecordDeleted()
{
	if(m_RecordDataPtr[0] == '*')
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} 


/****************************************************************/
/*																*/
/* Add new record to database file.								*/
/*																*/
/****************************************************************/
int CDbaseFile::AddRecord()
{
	if (m_DatabaseFile.m_hFile == NULL)
		return DBASE_NOT_OPEN;

	// empty record buffer
	memset(m_RecordDataPtr, ' ', m_RecordLength);
	
	try
	{
		// goto last record
		m_DatabaseFile.Seek(((DWORD)m_HeaderLength + (m_nRecordCount * m_RecordLength)), CFile::begin);
		m_DatabaseFile.Write(m_RecordDataPtr, m_RecordLength);
		m_nCurrentRecord = ++m_nRecordCount;
		m_bModified = TRUE;
		return DBASE_SUCCESS;
	}
	catch(CFileException *e)
	{
		e->Delete();
		m_strErrorMessage = "Error while adding new record.";
		return DBASE_INVALID_RECORD;
	}
}


/****************************************************************/
/*																*/
/* Insert new record, moving all following records one place.   */
/*																*/
/****************************************************************/
int CDbaseFile::InsertRecord(DWORD nRecordIndex)
{
	char buff[255];

	if(nRecordIndex > m_nRecordCount)
		return DBASE_INVALID_RECORD;

	DWORD nTotal = m_nRecordCount+1;

	// add new record at the end of file
	AddRecord();

	// move all records one place
	for (DWORD i=nTotal; i > nRecordIndex; i--)
	{
		 for (int j=1; j<=m_nFieldCount; j++)
		 {
			GetRecord(i-1);
			GetField(j, buff);
			GetRecord(i);
			PutField(j, buff);
			PutRecord(i);
		 }
	}
	m_bModified = TRUE;
	return DBASE_SUCCESS;
}


/****************************************************************/
/*																*/
/* Load the FIRST record of datafile in memory.					*/
/*																*/
/****************************************************************/
int CDbaseFile::GetFirstRecord()
{
	if(m_bModified == TRUE)
	{
		// update current record
		int nResult = PutRecord(m_nCurrentRecord);
		if (nResult != DBASE_SUCCESS)
			return nResult;
	}
	return GetRecord(1);
}


/****************************************************************/
/*																*/
/* Load the LAST record of datafile in memory.					*/
/*																*/
/****************************************************************/
int CDbaseFile::GetLastRecord()
{
	if(m_bModified == TRUE)
	{
		// update current record
		int nResult = PutRecord(m_nCurrentRecord);
		if (nResult != DBASE_SUCCESS)
			return nResult;
	}
	return GetRecord(m_nRecordCount);
}


/****************************************************************/
/*																*/
/* Load the NEXT record of datafile in memory.					*/
/*																*/
/****************************************************************/
int CDbaseFile::GetNextRecord()
{
	if(m_bModified == TRUE)
	{
		// update current record
		int nResult = PutRecord(m_nCurrentRecord);
		if (nResult != DBASE_SUCCESS)
		{
			return nResult;
		}
	}

	if(m_nCurrentRecord >= m_nRecordCount)
		return DBASE_EOF; 

	m_nCurrentRecord++;
	return GetRecord(m_nCurrentRecord);
}


/****************************************************************/
/*																*/
/* Load the PREVIOUS record of datafile in memory.				*/
/*																*/
/****************************************************************/
int CDbaseFile::GetPrevRecord()
{
	if(m_bModified == TRUE)
	{
		// update current record
		int nResult = PutRecord(m_nCurrentRecord);
		if (nResult != DBASE_SUCCESS)
		{
			return nResult;
		}
	}

	if(m_nCurrentRecord <= 1)
		return DBASE_BOF; 

	m_nCurrentRecord--;
	return GetRecord(m_nCurrentRecord);
}


/****************************************************************/
/*																*/
/* Return pointer to field data.								*/
/*																*/
/****************************************************************/
CField* CDbaseFile::GetFieldPtr(int nFieldIndex)
{
	POSITION pos;

	// check for valid fieldno
	if(nFieldIndex > 0 && nFieldIndex <= m_nFieldCount)
	{
		// find field in linked list
		if((pos = m_FieldList.FindIndex(nFieldIndex-1)) != NULL)
		{
			// get pointer to requested field
			CField *fp = (CField *)m_FieldList.GetAt(pos);
			return (fp);
		}
	}
	m_strErrorMessage = "Invalid field pointer";
	return NULL;
}


/****************************************************************/
/*																*/
/* Return pointer to field data.								*/
/*																*/
/****************************************************************/
CField* CDbaseFile::GetFieldPtr(LPCTSTR lpszFieldName)
{
	POSITION pos = m_FieldList.GetHeadPosition();

	// Iterate over the list, to find field by name
	while (pos != NULL)
	{
		// get next field pointer
		CField* fp = (CField *)m_FieldList.GetNext(pos);
		
		// check for requested field name
		if (lstrcmpi(fp->m_Name, lpszFieldName) == 0)
		{
			// return field pointer
			return (fp);
		}
	}	
	m_strErrorMessage = "Invalid field pointer";
	return NULL;
}


/****************************************************************/
/*																*/
/* Get field data by field index and put it in buffer.			*/
/*																*/
/****************************************************************/
int CDbaseFile::GetField(int nFieldIndex, char *buff, char type)
{
	CField *fp = GetFieldPtr(nFieldIndex);

	if(fp == NULL)
	{
		// not a valid field index -> return empty buffer
		buff[0] = '\0';
		m_strErrorMessage = "Invalid field index";
		return 0;
	}

	if ((fp->m_Type != type) && (type != '?'))
	{
		// unexpected type
		buff[0] = '\0';
		m_strErrorMessage = "Unexpected field type";
		return 0;
	}

	// copy field data in buffer
	memcpy(buff, fp->m_DataAddress , fp->m_Length);
	// make buffer null terminated
	buff[fp->m_Length] = '\0';

	// return field type
	return (fp->m_Length);
}


/****************************************************************/
/*																*/
/* Get field data by field name and put it in buffer.			*/
/*																*/
/****************************************************************/
int CDbaseFile::GetField(LPCTSTR lpszFieldName, char *buff, char type)
{
	CField *fp = GetFieldPtr(lpszFieldName);

	if(fp == NULL)
	{
		// not a valid field index -> return empty buffer
		buff[0] = '\0';
		m_strErrorMessage = "Invalid field index";
		return 0;
	}

	if ((fp->m_Type != type) && (type != '?'))
	{
		// unexpected type
		buff[0] = '\0';
		m_strErrorMessage = "Unexpected field type";
		return 0;
	}

	// copy field data in buffer
	memcpy(buff, fp->m_DataAddress , fp->m_Length);
	// make buffer null terminated
	buff[fp->m_Length] = '\0';

	// return field pointer
	return (fp->m_Length);
}


/****************************************************************/
/*																*/
/* Return date value in MM/DD/YY format by field index .		*/
/*																*/
/****************************************************************/
int CDbaseFile::GetDateField(int nFieldIndex, char *buff)
{
	int day;
	int mon;
	int yr;

	// get field data, check for DATE type
	if (!GetField(nFieldIndex, buff, 'D'))
	{
		lstrcpy(buff,"  /  /  ");
		m_strErrorMessage = "Invalid field type (not a DATE)";
		return(1);
	}

	// check for empty date field
	if (buff[0]==' ')
	{
		lstrcpy(buff,"  /  /  ");
		m_strErrorMessage = "Empty date";
		return(1);
	}
	else
	{
		// convert to MM/DD/YY format
		sscanf(buff, "%4d%02d%02d", &yr, &mon, &day);
		sprintf(buff, "%02d/%02d/%02d", mon, day, (yr % 100));
	}
	return(0);
}


/****************************************************************/
/*																*/
/* Return date value in MM/DD/YY format by field name.			*/
/*																*/
/****************************************************************/
int CDbaseFile::GetDateField(LPCTSTR lpszFieldName, char *buff)
{
	int day;
	int mon;
	int yr;
	
	// get field data, check for DATE type
	if (!GetField(lpszFieldName, buff, 'D'))
	{
		lstrcpy(buff,"  /  /  ");
		m_strErrorMessage = "Invalid field type (not a DATE)";
		return(1);
	}

	// check for empty date field
	if (buff[0]==' ')
	{
		lstrcpy(buff,"  /  /  ");
		m_strErrorMessage = "Empty date";
		return(1);
	}
	else
	{
		// convert to MM/DD/YY format
		sscanf(buff, "%4d%02d%02d", &yr, &mon, &day);
		sprintf(buff,"%02d/%02d/%02d", mon, day, (yr % 100));
	}
	return(0);
}


/****************************************************************/
/*																*/
/* Return date value in COleDateTime format by field name.		*/
/*																*/
/****************************************************************/
COleDateTime CDbaseFile::GetDateField(LPCTSTR lpszFieldName)
{
	COleDateTime datetime = COleDateTime::GetCurrentTime();
	int nDay;
	int nMonth;
	int nYear;
	char buff[12];
	
	// get field data, check for DATE type
	if (!GetField(lpszFieldName, buff, 'D'))
	{
		datetime.SetDateTime(9999, 12, 12, 0, 0, 0);
		m_strErrorMessage = "Invalid field type (not a DATE)";
		return datetime;
	}

	// check for empty date field
	if ((buff[0]==' ') || (lstrlen(buff) == 0))
	{
		datetime.SetDateTime(9999, 12, 12, 0, 0, 0);
		m_strErrorMessage = "Empty date";
		return datetime;
	}
	else
	{
		// convert to MM/DD/YY format
		sscanf(buff, "%4d%02d%02d", &nYear, &nMonth, &nDay);
		if ((nDay >= 1 && nDay <= 31) && (nMonth >= 1 && nMonth <= 12) && (nYear > 100))
		{
			datetime.SetDateTime(nYear, nMonth, nDay, 0, 0, 0);
		}
		else
			datetime.SetDateTime(9999, 12, 12, 0, 0, 0);
	}
	return datetime;
}



/****************************************************************/
/*																*/
/* Return date value in COleDateTime format by field index.		*/
/*																*/
/****************************************************************/
COleDateTime CDbaseFile::GetDateField(int nFieldIndex)
{
	COleDateTime datetime = COleDateTime::GetCurrentTime();
	int nDay;
	int nMonth;
	int nYear;
	char buff[12];
	
	// get field data, check for DATE type
	if (!GetField(nFieldIndex, buff, 'D'))
	{
		datetime.SetDateTime(9999, 12, 12, 0, 0, 0);
		m_strErrorMessage = "Invalid field type (not a DATE)";
		return datetime;
	}

	// check for empty date field
	if ((buff[0]==' ') || (lstrlen(buff) == 0))
	{
		datetime.SetDateTime(9999, 12, 12, 0, 0, 0);
		m_strErrorMessage = "Empty date";
		return datetime;
	}
	else
	{
		// convert to MM/DD/YY format
		sscanf(buff, "%4d%02d%02d", &nYear, &nMonth, &nDay);
		if ((nDay >= 1 && nDay <= 31) && (nMonth >= 1 && nMonth <= 12) && (nYear > 100))
		{
			datetime.SetDateTime(nYear, nMonth, nDay, 0, 0, 0);
		}
		else
			datetime.SetDateTime(9999, 12, 12, 0, 0, 0);
	}
	return datetime;
}


/****************************************************************/
/*																*/
/* Get numeric value by field index.							*/
/*																*/
/****************************************************************/
long CDbaseFile::GetNumericField(int nFieldIndex)
{
	char buff[19];
	// get field data, check for NUMERIC type
	if (!GetField(nFieldIndex, buff, 'N'))
	{
		// get field data, check for MEMO type
		if (!GetField(nFieldIndex, buff, 'M'))
		{
			m_strErrorMessage = "Invalid field type (not a NUMERIC)";
			return 0;
		}
		else
			return atol(buff);
	}
	else
		return atol(buff);
}


/****************************************************************/
/*																*/
/* Get numeric value by field name.								*/
/*																*/
/****************************************************************/
long CDbaseFile::GetNumericField(LPCTSTR lpszFieldName)
{
	char buff[19];
	// get field data, check for NUMERIC type
	if (!GetField(lpszFieldName, buff, 'N'))
	{
		// get field data, check for MEMO type
		if (!GetField(lpszFieldName, buff, 'M'))
		{
			m_strErrorMessage = "Invalid field type (not a NUMERIC)";	
			return 0;
		}
		else
			return atol(buff);
	}
	else
		return atol(buff);
}


/****************************************************************/
/*																*/
/* Get float value by field index.								*/
/*																*/
/****************************************************************/
double CDbaseFile::GetFloatField(int nFieldIndex)
{
	char buff[21];
	// get field data, check for NUMERIC type
	if(!GetField(nFieldIndex, buff, 'N'))
	{
		m_strErrorMessage = "Invalid type (not a FLOAT)";
		return 0;
	}
	else
		return atof(buff);
}


/****************************************************************/
/*																*/
/* Get float value by field name.								*/
/*																*/
/****************************************************************/
double CDbaseFile::GetFloatField(LPCTSTR lpszFieldName)
{
	char buff[21];
	// get field data, check for NUMERIC type
	if(!GetField(lpszFieldName, buff, 'N'))
	{
		m_strErrorMessage = "Invalid type (not a FLOAT)";
		return 0;
	}
	else
		return atof(buff);
}


/****************************************************************/
/*																*/
/* Get logical value by field index.							*/
/*																*/
/****************************************************************/
BOOL CDbaseFile::GetLogicalField(int nFieldIndex)
{
	char buff[2];
	// get field data, check for LOGICAL type
	if (!GetField(nFieldIndex, buff, 'L'))
	{
		m_strErrorMessage = "Invalid type (not a LOGICAL)";
		return FALSE;
	}
	return ((buff[0]=='Y')||(buff[0]=='y')||(buff[0]=='T')||(buff[0]=='t'));
}


/****************************************************************/
/*																*/
/* Get logical value by field name.								*/
/*																*/
/****************************************************************/
BOOL CDbaseFile::GetLogicalField(LPCTSTR lpszFieldName)
{
	char buff[2];
	// get field data, check for LOGICAL type
	if (!GetField(lpszFieldName, buff , 'L'))
	{
		m_strErrorMessage = "Invalid type (not a LOGICAL)";
		return FALSE;
	}
	return ((buff[0]=='Y')||(buff[0]=='y')||(buff[0]=='T')||(buff[0]=='t'));
}


/****************************************************************/
/*																*/
/* Get CHAR value by field name.								*/
/*																*/
/****************************************************************/
CString CDbaseFile::GetCharField(LPCTSTR lpszFieldName)
{
	CString strResult;
	CField *fp = GetFieldPtr(lpszFieldName);

	if(fp == NULL)
	{
		// not a valid field index -> return empty buffer
		m_strErrorMessage = "Invalid field index";
		return strResult;
	}
	if (fp->m_Type == 'C')
	{
		char buff[255];
		if (GetField(lpszFieldName, buff))
		{
			strResult.Format("%s", buff);
			strResult.TrimLeft();
			strResult.TrimRight();
		}
	}
	return strResult;
}


/****************************************************************/
/*																*/
/* Get CHAR value by field index.								*/
/*																*/
/****************************************************************/
CString CDbaseFile::GetCharField(int nFieldIndex)
{
	CString strResult;
	CField *fp = GetFieldPtr(nFieldIndex);

	if(fp == NULL)
	{
		// not a valid field index -> return empty buffer
		m_strErrorMessage = "Invalid field index";
		return strResult;
	}
	if (fp->m_Type == 'C')
	{
		char buff[255];
		if (GetField(nFieldIndex, buff))
		{
			strResult.Format("%s", buff);
			strResult.TrimLeft();
			strResult.TrimRight();
		}
	}
	return strResult;
}


/****************************************************************/
/*																*/
/* Update CHARACTER field data by field name.					*/
/*																*/
/****************************************************************/
int CDbaseFile::PutField(LPCTSTR lpszFieldName, char *buff)
{
	POSITION pos = m_FieldList.GetHeadPosition();

	int nFieldIndex = 1;

	// Iterate over the list, to find field by name
	while (pos != NULL)
	{
		// get next field pointer
		CField* fp = (CField *)m_FieldList.GetNext(pos);
		
		// check for requested field name
		if (lstrcmpi(fp->m_Name, lpszFieldName) == 0)
		{
			return PutField(nFieldIndex, buff);
		}
		nFieldIndex++;
	}	
	m_strErrorMessage = "Not a valid field index";
	// not a valid field index -> return 0
 	return DBASE_INVALID_FIELDNO;
}


/****************************************************************/
/*																*/
/* Update CHARACTER field data by field index.					*/
/*																*/
/****************************************************************/
int CDbaseFile::PutField(int nFieldIndex, char *buff)
{
	// get pointer to this field
	CField *fp = GetFieldPtr(nFieldIndex);

	if (fp == NULL)
	{
		m_strErrorMessage = "Not a valid field index";
		return DBASE_INVALID_FIELDNO;
	}

	// check for correct type
	switch(fp->m_Type)
	{
		case 'F':
		case 'N':
		case 'M':
		{
			// remove spaces
			TrimSpace(buff);
			// validate data
			char *ptr = buff;
			while(*ptr)
			{
				switch(*ptr)
				{
					case '+':
					case '-':
					case '.':
					case ',':
					case '0': 
					case '1': 
					case '2': 
					case '3': 
					case '4': 
					case '5': 
					case '6': 
					case '7': 
					case '8': 
					case '9':
						// next character 
						ptr++;
						break;
					default:
						// character not valid
						m_strErrorMessage = "Invalid type (not a FLOAT/NUMERIC)";
						return DBASE_INVALID_DATA;
				}
			}
			// everything seems ok
			break;
		}
		case 'D':
			if(!IsValidDate(buff))
			{
				m_strErrorMessage = "Invalid type (not a DATE)";
				return DBASE_INVALID_DATA;
			}
			break;
		case 'L':
			// validate data
			switch(buff[0])
			{
				case 'Y':
				case 'y':
				case 'N':
				case 'n':
				case 'F':
				case 'f':
				case 'T':
				case 't':
				case '?':
					break;
				default:
					m_strErrorMessage = "Invalid type (not a LOGICAL)";
					return DBASE_INVALID_DATA;
			}
			break;
	}

	// empty field space
	memset(fp->m_DataAddress, ' ', fp->m_Length);

	if (fp->m_Type == 'F' || fp->m_Type == 'N' || fp->m_Type == 'M')
	{
		char form[25];
		// allocate memory for fieldlength + '/0'
		char *lpszNumeric = new char[fp->m_Length+1];
		// clear memory
		memset(lpszNumeric, 0x00, fp->m_Length+1);
		// create a printf format
		sprintf(form, "%%.%dlf", fp->m_DecCount);
		// convert double to string
		sprintf(lpszNumeric, form, atof(buff));

		int nNumLength = lstrlen(lpszNumeric);
		// alignment for numbers is right
		memcpy(fp->m_DataAddress + fp->m_Length - nNumLength, lpszNumeric, nNumLength);
		delete[] lpszNumeric;
		return DBASE_SUCCESS;
	}

	// strip off 'extra' bytes
	int nLength = lstrlen(buff);
	if(nLength > fp->m_Length)
	{
		nLength = fp->m_Length;
	}

	memcpy(fp->m_DataAddress, buff, nLength);
	return DBASE_SUCCESS;
}


/****************************************************************/
/*																*/
/* Update DATE field data by field index.						*/
/*																*/
/****************************************************************/
int CDbaseFile::PutDateField(int nFieldIndex, char *buff, int baseyear)
{
	char szDate[10];
	int mon, day, yr;

	// convert from MM/DD/YY to YYYYMMDD
	sscanf(buff, "%2d/%02d/%02d", &mon, &day, &yr);
	sprintf(szDate, "%04d%02d%02d", yr+baseyear, mon, day);
	return PutField(nFieldIndex, szDate);
}


/****************************************************************/
/*																*/
/* Update DATE field data by field name.						*/
/*																*/
/****************************************************************/
int CDbaseFile::PutDateField(LPCTSTR lpszFieldName, char *buff, int baseyear)
{
	char szDate[10];
	int mon, day, yr;

	// convert from MM/DD/YY to YYYYMMDD
	sscanf(buff,"%2d/%02d/%02d", &mon, &day, &yr);
	sprintf(szDate,"%04d%02d%02d", yr+baseyear, mon, day);
	return PutField(lpszFieldName, szDate);
}


/****************************************************************/
/*																*/
/* Update NUMERIC field data by field index.					*/
/*																*/
/****************************************************************/
int CDbaseFile::PutNumericField(int nFieldIndex, long lValue)
{
	char buff[19];
	ltoa(lValue, buff, 10);
	return PutField(nFieldIndex, buff);
}


/****************************************************************/
/*																*/
/* Update NUMERIC field data by field name.						*/
/*																*/
/****************************************************************/
int CDbaseFile::PutNumericField(LPCTSTR lpszFieldName, long lValue)
{
	char buff[19];
	ltoa(lValue, buff, 10);
	return PutField(lpszFieldName, buff);
}


/****************************************************************/
/*																*/
/* Update FLOAT field data by field index.						*/
/*																*/
/****************************************************************/
int CDbaseFile::PutFloatField(int nFieldIndex, double value)
{
	char form[25];
	char *buff;
	int result;

	CField *fld = GetFieldPtr(nFieldIndex);	

	// check type
	if (!fld || fld->m_Type != 'N')
	{
		m_strErrorMessage = "Invalid type (not a FLOAT)";
		return DBASE_INVALID_DATA;
	}
	// check for maximum length
	sprintf(form,"%d",(int)value);
	if (lstrlen(form) > (fld->m_Length - fld->m_DecCount - 2))
	{
		m_strErrorMessage = "Invalid length (FLOAT): " + CString(form);
		return DBASE_INVALID_DATA;
	}

	// allocate memory for fieldlength + '/0'
	buff = new char[fld->m_Length+1];
	// clear memory
	memset(buff, 0x00, fld->m_Length+1);
	
	// create a printf format
	sprintf(form, "%%.%dlf", fld->m_DecCount);
	// convert double to string
	sprintf(buff, form, value);

	// set the value
	result = PutField(nFieldIndex, buff);
	delete[] buff;
	return result;
}


/****************************************************************/
/*																*/
/* Update FLOAT field data by field name.						*/
/*																*/
/****************************************************************/
int CDbaseFile::PutFloatField(LPCTSTR lpszFieldName, double value)
{
	POSITION pos = m_FieldList.GetHeadPosition();

	int nFieldIndex = 1;

	// Iterate over the list, to find field by name
	while (pos != NULL)
	{
		// get next field pointer
		CField* fp = (CField *)m_FieldList.GetNext(pos);
		
		// check for requested field name
		if (lstrcmpi(fp->m_Name, lpszFieldName) == 0)
		{
			return PutFloatField(nFieldIndex, value);
		}
		nFieldIndex++;
	}	
	// not a valid field index -> return 0
	return 0;
}


/****************************************************************/
/*																*/
/* Update CHAR value by field name.								*/
/*																*/
/****************************************************************/
int CDbaseFile::PutCharField(LPCTSTR lpszFieldName, CString str)
{
	int nResult = PutField(lpszFieldName, str.GetBuffer(str.GetLength()));
	str.ReleaseBuffer();
	return nResult;
}


/****************************************************************/
/*																*/
/* Update CHAR value by field index.							*/
/*																*/
/****************************************************************/
int CDbaseFile::PutCharField(int nFieldIndex, CString str)
{
	int nResult = PutField(nFieldIndex, str.GetBuffer(str.GetLength()));
	str.ReleaseBuffer();
	return nResult;
}


/****************************************************************/
/*																*/
/* Update LOGICAL field data by field index.					*/
/*																*/
/****************************************************************/
int CDbaseFile::PutLogicalField(int nFieldIndex, BOOL bValue)
{
	char buff[2];
	sprintf(buff, "%c", bValue ? 'Y' : 'N');
	return PutField(nFieldIndex, buff);
}


/****************************************************************/
/*																*/
/* Update LOGICAL field data by field name.						*/
/*																*/
/****************************************************************/
int CDbaseFile::PutLogicalField(LPCTSTR lpszFieldName, BOOL bValue)
{
	char buff[2];
	sprintf(buff, "%c", bValue ? 'Y' : 'N');
	return PutField(lpszFieldName, buff);
}


/****************************************************************/
/*																*/
/* This function clones the current database.					*/
/*																*/
/****************************************************************/
BOOL CDbaseFile::CloneDatabase(LPCTSTR lpszCloneName, BOOL bCopyRecords, BOOL bSkipDeleted)
{
	CStringArray fieldsArray;

	CString strField;
	// insert fieldnames
	for (int i=1; i<=GetFieldCount(); i++)
	{
		CField *pField = GetFieldPtr(i);
		if (pField)
		{
			strField.Format("%s,%c,%d,%d", pField->m_Name, pField->m_Type, pField->m_Length, pField->m_DecCount);
			fieldsArray.Add(strField);
		}
	}

	CDbaseFile dBaseFile;
	// create empty database.dbf
	if (dBaseFile.Create(lpszCloneName, fieldsArray) != DBASE_SUCCESS)
	{
		return FALSE;
	}			

	if (bCopyRecords)
	{
		DWORD dwCounter = 0;
		char szBuff[255];

		// open new file
		if (dBaseFile.Open(lpszCloneName) == DBASE_SUCCESS)
		{
			// copy all records
			for(int rc=GetFirstRecord(); rc==DBASE_SUCCESS; rc=GetNextRecord())
  			{
				if(bSkipDeleted && IsRecordDeleted())
					continue;

				dwCounter++;
				dBaseFile.AddRecord();
				dBaseFile.GetRecord(dwCounter);

				// copy all fields
				for (int i=1; i<=GetFieldCount(); i++)
				{
					CField *pField = GetFieldPtr(i);
					if (pField)
					{
						if (GetField(i, szBuff))
						{
							dBaseFile.PutField(i, szBuff);
						}
						
						// copy memo data
						if (pField->m_Type == 'M')
						{
							char *buff;
							CString strMemo;

							DWORD dwLength = GetMemoFieldLength(i);
							if (dwLength)
							{
								try
								{
									buff = new char[dwLength+1];
									GetMemoField(i, buff, dwLength);
									strMemo.Format("%s", buff);
									delete buff;

									// set value
									dBaseFile.PutMemoField(i, (char *)(LPCTSTR)strMemo, strMemo.GetLength());
								}
								catch(...)
								{
								}
							}	
							else
							{
								// initialize to zero
								dBaseFile.ClearMemoField(i);
							}
						}
					}
					else
					{
						// field does not exists-> do not copy data
					}
				}
				dBaseFile.PutRecord(dwCounter);
			}
			dBaseFile.Close();
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}


/****************************************************************/
/*																*/
/* Remove all deleted records from datafile.					*/
/*																*/
/****************************************************************/
int CDbaseFile::Pack()
{
	char szTempFileName[MAX_PATH];
	BOOL bMemoFileExists = FALSE;

	// create temporary file
	GetTempPath(sizeof(szTempFileName), szTempFileName);
	lstrcat(szTempFileName, "~DBFPACK.DBF");

	// open memo file .DBT
	if (m_MemoFile.m_hFile == NULL)
	{
		if (OpenMemoFile() == DBASE_SUCCESS)
			bMemoFileExists = TRUE;
	}
	else
	{
		bMemoFileExists = TRUE;
	}

	// clone database, copy all records except those marked 'deleted'
	if (!CloneDatabase(szTempFileName, TRUE, TRUE))
	{
		return DBASE_NO_FILE;
	}
	// close original data file (and DBT file)
	Close();
   
	// delete original data file
	if (DeleteFile(m_strFileName) == 0)
	{
		m_strErrorMessage.Format("Failed to delete file '%s'.", m_strFileName);
		return DBASE_NO_FILE;
	}

	// rename temporary file to original name
	if (MoveFile(szTempFileName, m_strFileName) == 0) 
	{
		m_strErrorMessage.Format("Failed to rename file '%s'.", szTempFileName);
		return DBASE_NO_FILE;
	}
	
	if (bMemoFileExists)
	{
		// delete original data file
		DeleteFile(GetMemoFileName());

		// rename temporary memo file to original name
		if (MoveFile(GetMemoFileName(szTempFileName), GetMemoFileName()) == 0) 
		{
			return DBASE_NO_FILE;
		}
	}
	// reopen file
	return Open((LPCTSTR)m_strFileName);
}


/****************************************************************/
/*																*/
/* Create new database file, based on structure in parameters.	*/
/*																*/
/****************************************************************/
int CDbaseFile::Create(LPCTSTR lpszFileName, CStringArray &strFieldArray)
{
	m_strFileName.Format("%s", lpszFileName);

	BOOL bCreateMemoFile = FALSE;

	// iterate through all arguments and create a list of all the fields
	for (int x=0; x < strFieldArray.GetSize(); x++)
	{
		CString strName, strBuff;
		char nType; 
		unsigned char nLength; 
		unsigned char nDecCount; 

		// get name
		AfxExtractSubString(strName, strFieldArray[x], 0, ',');
		strName.TrimLeft();
		strName.TrimRight();
		// get type
		AfxExtractSubString(strBuff, strFieldArray[x], 1, ',');
		strBuff.TrimLeft();
		strBuff.TrimRight();
		nType = strBuff.GetAt(0);
		// get length
		AfxExtractSubString(strBuff, strFieldArray[x], 2, ',');
		strBuff.TrimLeft();
		strBuff.TrimRight();
		nLength = atoi(strBuff);
		// get field dec count
		AfxExtractSubString(strBuff, strFieldArray[x], 3, ',');
		strBuff.TrimLeft();
		strBuff.TrimRight();
		nDecCount = atoi(strBuff);		

		if (strName.IsEmpty() || 
			(nType != 'C' && nType != 'D' && nType != 'N' && nType != 'L' && nType != 'F' && nType != 'M') ||
			nLength < 1 || 
			nDecCount < 0)
			break;

		// create new field
		CField *pField = new CField(strName, nType, nLength, nDecCount, x+1);
	
		// add field to linked list
		m_FieldList.AddTail(pField);

		if (nType == 'M')
			bCreateMemoFile = TRUE;
	}

	try
	{
		// now create the new file
		if (!m_DatabaseFile.Open(lpszFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
		{
  			return DBASE_NO_FILE;
		}
				
		// set some header info
		SYSTEMTIME sysTime;

		if (bCreateMemoFile)
			m_Version = 0x83;
		else
			m_Version = 0x03;

		m_nRecordCount = 0;
		m_HeaderLength = 0;
		m_RecordLength = 1;

		GetSystemTime(&sysTime);
		m_LastUpdateDD = (UCHAR)sysTime.wDay;
		m_LastUpdateMM = (UCHAR)sysTime.wMonth;
		// 1900 is base year -> 1997 = 61h, 2001 -> 65h, so a valid range is 1900 - 2155 
		m_LastUpdateYY = (UCHAR)sysTime.wYear-1900;	
					
		// set file pointer to begin of file
		m_DatabaseFile.SeekToBegin();

		// write last updated date
		m_DatabaseFile.Write(&m_Version, 1);
		m_DatabaseFile.Write(&m_LastUpdateYY, 1);
		m_DatabaseFile.Write(&m_LastUpdateMM, 1);
		m_DatabaseFile.Write(&m_LastUpdateDD, 1);

		// number of records in datafile
		m_DatabaseFile.Write(&m_nRecordCount, 4);
		// length of header structure 
		m_DatabaseFile.Write(&m_HeaderLength, 2);
		// length of the record
		m_DatabaseFile.Write(&m_RecordLength, 2);
					
		for(int i=1;i<=20;i++) 
			m_DatabaseFile.Write("\0x00", 1);

		m_HeaderLength = HEADER_LENGTH + (m_FieldList.GetCount() * FIELD_REC_LENGTH) + 1;

		// now add all the field to the file
		while (!m_FieldList.IsEmpty())
		{
			// get next filed pointer
			CField* pField = (CField *)m_FieldList.RemoveHead();
			// write field name		
			m_DatabaseFile.Write(&pField->m_Name, 11);
			// write field type
			m_DatabaseFile.Write(&pField->m_Type, 1);
			// write field data address
			for(i=1;i<=4;i++) 
				m_DatabaseFile.Write("\0x00", 1);
			// write field length
			m_DatabaseFile.Write(&pField->m_Length, 1);
			// write field decimal count
			m_DatabaseFile.Write(&pField->m_DecCount, 1);
			// write reserved bytes
			for(i=1;i<=14;i++) 
				m_DatabaseFile.Write("\0x00", 1);
			m_RecordLength = m_RecordLength + pField->m_Length;
			delete pField;
		}

		/*** Overwrite the header with the new structure info ***/

		// set file pointer to begin of file
		m_DatabaseFile.SeekToBegin();

		// write last updated date
		m_DatabaseFile.Write(&m_Version, 1);
		m_DatabaseFile.Write(&m_LastUpdateYY, 1);
		m_DatabaseFile.Write(&m_LastUpdateMM, 1);
		m_DatabaseFile.Write(&m_LastUpdateDD, 1);

		// number of records in datafile
		m_DatabaseFile.Write(&m_nRecordCount, 4);
		// length of header structure 
		m_DatabaseFile.Write(&m_HeaderLength, 2);
		// length of the record
		m_DatabaseFile.Write(&m_RecordLength, 2);
					
		for(i=1;i<=20;i++) 
			m_DatabaseFile.Write("\0x00", 1);

		// set pointer to end of file
		m_DatabaseFile.SeekToEnd();

		//  end of field record 
		m_DatabaseFile.Write("\0x0d", 1);
					
		// write eof character
		m_DatabaseFile.Write("\0x1a", 1);

		// close the file
		m_DatabaseFile.Close();
		m_DatabaseFile.m_hFile = NULL;
	}
	catch(CFileException *e)
	{
		e->Delete();
		if (m_DatabaseFile.m_hFile != NULL)
			m_DatabaseFile.Close();
		m_DatabaseFile.m_hFile = NULL;
		
		m_strErrorMessage = "An exception occured while creating database.";
		return DBASE_NO_FILE;
	}

	if (bCreateMemoFile)
	{
		int nResult = CreateMemoFile();
		CloseMemoFile();
		return nResult;
	}

	return DBASE_SUCCESS;
}


/****************************************************************/
/*																*/
/* Search for a specific record where criteria = field value	*/
/* Start searching from record '0'								*/
/*																*/
/****************************************************************/
DWORD CDbaseFile::SearchRecord(int nFieldIndex, LPCTSTR lpszCriteria)
{
	char buff[255];

	// walk through all records
    for(int nResult=GetFirstRecord(); nResult==DBASE_SUCCESS; nResult=GetNextRecord())
  	{
		if (!IsRecordDeleted())
		{
			GetField(nFieldIndex, buff);
			if (!lstrcmp(buff, lpszCriteria))
			{
				return GetCurRecNo();              			
			}
		}
	}
	return 0;
}


/****************************************************************/
/*																*/
/* Search for a specific record where criteria = field value	*/
/* Start searching from record '0'								*/
/*																*/
/****************************************************************/
DWORD CDbaseFile::SearchRecord(LPCTSTR lpszFieldName, LPCTSTR lpszCriteria)
{
	char buff[255];

	// walk through all records
    for(int nResult=GetFirstRecord(); nResult==DBASE_SUCCESS; nResult=GetNextRecord())
  	{
		if (!IsRecordDeleted())
		{
			GetField(lpszFieldName, buff);
			if (!lstrcmp(buff, lpszCriteria))
			{
				return GetCurRecNo();              			
			}
		}
	}
	return 0;
}


/****************************************************************/
/*																*/
/* Search for a specific record where criteria = field value	*/
/* Start searching from record 'nStartRec'						*/
/*																*/
/****************************************************************/
DWORD CDbaseFile::SearchFromRecord(int nFieldIndex, LPCTSTR lpszCriteria, DWORD nStartRec)
{
	char buff[255];

	// walk through all records
    for(int nResult=GetRecord(nStartRec); nResult==DBASE_SUCCESS; nResult=GetNextRecord())
  	{
		if (!IsRecordDeleted())
		{
			GetField(nFieldIndex, buff);
			if (!lstrcmp(buff, lpszCriteria))
			{
				return GetCurRecNo();              			
			}
		}
	}
	return 0;
}


/****************************************************************/
/*																*/
/* Search for a specific record where criteria = field value	*/
/* Start searching from record 'nStartRec'						*/
/*																*/
/****************************************************************/
DWORD CDbaseFile::SearchFromRecord(LPCTSTR lpszFieldName, LPCTSTR lpszCriteria, DWORD nStartRec)
{
	char buff[255];

	// walk through all records
    for(int nResult=GetRecord(nStartRec); nResult==DBASE_SUCCESS; nResult=GetNextRecord())
  	{
		if (!IsRecordDeleted())
		{
			GetField(lpszFieldName, buff);
			if (!lstrcmp(buff, lpszCriteria))
			{
				return GetCurRecNo();              			
			}
		}
	}
	return 0;
}


/****************************************************************/
/*																*/
/* Get description of last error.								*/
/*																*/
/****************************************************************/
void CDbaseFile::GetLastError(LPTSTR lpszErrorMessage)
{
	lstrcpy(lpszErrorMessage, (LPCTSTR)m_strErrorMessage);
}


//*** MEMO related routines ***//

/****************************************************************/
/*																*/
/* Memo field function: Open DBT file.							*/
/*																*/
/****************************************************************/
int CDbaseFile::OpenMemoFile(LPCTSTR lpszFileName)
{
	CString szMemo=m_strFileName;
	szMemo=szMemo.Left(szMemo.ReverseFind('.'))+".FPT";
	lpszFileName=szMemo;
	try
	{
		// open memo file
		if (!m_MemoFile.Open((lpszFileName != NULL) ? lpszFileName : GetMemoFileName(), CFile::modeReadWrite | CFile::shareDenyNone))
		{
			m_MemoFile.m_hFile = NULL;
 			m_strErrorMessage = "Unable to open memo file";
			return DBASE_NO_FILE;
		}

		// goto beginning of the file
		m_MemoFile.SeekToBegin();

		// first 4 bytes is next free block
		m_MemoFile.Read(&m_dwMemoNextFreeBlock, 4);
	}
	catch(CFileException *e)
	{
		e->Delete();
		if (m_MemoFile.m_hFile != NULL)
			m_MemoFile.Close();
		m_MemoFile.m_hFile = NULL;
 		m_strErrorMessage = "An error occured while reading from memo file";
		return DBASE_READ_ERROR;
	}

	// allocate memory for memoblock
	if((m_MemoDataPtr = (char *)malloc(m_dwMemoBlockSize)) == NULL)
	{
		m_MemoFile.Close();
		m_MemoFile.m_hFile = NULL;
	  	m_strErrorMessage = "Out of memory";
		return DBASE_OUT_OF_MEM;
	}
	return DBASE_SUCCESS;
}


/****************************************************************/
/*																*/
/* Memo field function: Create new DBT file.					*/
/*																*/
/****************************************************************/
int CDbaseFile::CreateMemoFile()
{
	try
	{
		// now create the new file
		if (!m_MemoFile.Open(GetMemoFileName(), CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
		{
 			m_strErrorMessage = "Unable to create memo file";
  			return DBASE_NO_FILE;
		}
		
		// goto beginning of the file
		m_MemoFile.SeekToBegin();

		m_dwMemoNextFreeBlock = 1;

		// first 4 bytes is next block
		m_MemoFile.Write(&m_dwMemoNextFreeBlock, 4);

		// reserved space
		for(int i = 4; i < 512; i++)
		{
			m_MemoFile.Write("\0x00", 1);
		}
		// write memo version
		m_MemoFile.Seek(16, CFile::begin);
		m_MemoFile.Write("\0x03", 1);
	}
	catch(CFileException *e)
	{
		e->Delete();
		if (m_MemoFile.m_hFile != NULL)
			m_MemoFile.Close();
		m_MemoFile.m_hFile = NULL;
		m_strErrorMessage = "An error occured while writing to memo file";
		return DBASE_WRITE_ERROR;
	}

	// allocate memory for memoblock
	if((m_MemoDataPtr = (char *)malloc(m_dwMemoBlockSize)) == NULL)
	{
		m_MemoFile.Close();
		m_MemoFile.m_hFile = NULL;
		m_strErrorMessage = "Out of memory";
		return DBASE_OUT_OF_MEM;
	}
	return DBASE_SUCCESS;
}


/****************************************************************/
/*																*/
/* Memo field function: Close DBT file.							*/
/*																*/
/****************************************************************/
void CDbaseFile::CloseMemoFile()
{
	// close file
	if (m_MemoFile.m_hFile != NULL)
		m_MemoFile.Close();
		
	m_MemoFile.m_hFile = NULL;

	// clean up memo data block
	if (m_MemoDataPtr != NULL)
		free(m_MemoDataPtr);

	m_MemoDataPtr = NULL;
}


/****************************************************************/
/*																*/
/* Memo field function: Get memo filename.						*/
/*																*/
/****************************************************************/
CString CDbaseFile::GetMemoFileName(LPCTSTR lpszFileName)
{
	CString strMemoFileName = (lpszFileName != NULL) ? lpszFileName : m_strFileName;

	int nLength = strMemoFileName.GetLength()-1;
	if (nLength < 0)
		return strMemoFileName;
	
	switch(strMemoFileName.GetAt(nLength))
	{
		case 'F':
			strMemoFileName.SetAt(nLength, 'T');
			break;
		default:
		case 'f':
			strMemoFileName.SetAt(nLength, 't');
			break;
	}
	return strMemoFileName;
}


/****************************************************************/
/*																*/
/* Memo field function: Get value of memo field by field name.	*/
/*																*/
/****************************************************************/
int CDbaseFile::GetMemoField(LPCTSTR lpszFieldName, char *buff, DWORD dwLength)
{
	CField *fp = GetFieldPtr(lpszFieldName);

	if(fp == NULL)
	{
		// not a valid field index -> return empty buffer
		buff[0] = '\0';
		m_strErrorMessage = "Invalid field index";
		return 0;
	}

	if (fp->m_Type != 'M')
	{
		// unexpected type
		buff[0] = '\0';
		m_strErrorMessage = "Unexpected field type";
		return 0;
	}

	return GetMemoField(fp->m_FieldNumber, buff, dwLength);
}


/****************************************************************/
/*																*/
/* Memo field function: Get value of memo field by field index.	*/
/*																*/
/****************************************************************/
int CDbaseFile::GetMemoField(int nFieldIndex, char *buff, DWORD dwLength)
{
	DWORD dwBlockNo;

	// get first block number
	if((dwBlockNo = GetNumericField(nFieldIndex)) == 0)
	{
		m_strErrorMessage = "No memo data available";
		return DBASE_NO_MEMO_DATA;
	}

	DWORD dwBlockCounter = 0;
	DWORD dwTotalCopied = 0;

	BOOL bContinue = TRUE;
	while(bContinue)
	{
		try
		{
			// goto specified memo block
			m_MemoFile.Seek(dwBlockNo * m_dwMemoBlockSize, CFile::begin); 
			// read data
			m_MemoFile.Read(m_MemoDataPtr, m_dwMemoBlockSize);
		}
		catch(CFileException *e)
		{
			e->Delete();
 			m_strErrorMessage = "An error occured while reading from memo file";
			return DBASE_READ_ERROR;
		}

		dwBlockNo++;
		dwBlockCounter = 0;

		// copy memo block
		while(dwBlockCounter < m_dwMemoBlockSize)
		{
			if(dwTotalCopied < dwLength)
			{
				buff[dwTotalCopied++] = m_MemoDataPtr[dwBlockCounter++];
			}
			else
			{
				bContinue = FALSE;
				break;
			}
		}
	}
	// terminate buffer
	buff[dwLength] = '\0';
	return DBASE_SUCCESS;
}


/****************************************************************/
/*																*/
/* Memo field function: Get length of field data by field name.	*/
/*																*/
/****************************************************************/
DWORD CDbaseFile::GetMemoFieldLength(LPCTSTR lpszFieldName)
{
	CField *fp = GetFieldPtr(lpszFieldName);

	if(fp == NULL)
	{
		// not a valid field index -> return empty buffer
		m_strErrorMessage = "Invalid field index";
		return 0;
	}
	return GetMemoFieldLength(fp->m_FieldNumber);
}


/****************************************************************/
/*																*/
/* Memo field function: Get length of field data by field index.*/
/*																*/
/****************************************************************/
DWORD CDbaseFile::GetMemoFieldLength(int nFieldIndex)
{
	DWORD dwBlockNo;
	// get first block number
	if((dwBlockNo = GetNumericField(nFieldIndex)) == 0)
		return 0;

	
	DWORD dwBlockCounter = 0;
	DWORD dwByteCount = 0;

	BOOL bContinue = TRUE;
	while(bContinue)
	{
		// read one block at a time
		try
		{
			// goto specified memo block
			m_MemoFile.Seek(dwBlockNo * m_dwMemoBlockSize, CFile::begin); 
			// read data
			m_MemoFile.Read(m_MemoDataPtr, m_dwMemoBlockSize);
		}
		catch(CFileException *e)
		{
			e->Delete();
 			m_strErrorMessage = "An error occured while reading from memo file";
			return dwByteCount;
		}

		dwBlockNo++;
		dwBlockCounter = 0;

		char chPrevious = 0;

		// search for field terminator
		while(dwBlockCounter < m_dwMemoBlockSize)
		{
			// field terminator ?
			if((m_MemoDataPtr[dwBlockCounter] == 0x1a) && (chPrevious == 0x1a))
			{
				bContinue = FALSE;
				break;
			}
			else
			{
				chPrevious = m_MemoDataPtr[dwBlockCounter];
				dwByteCount++; 
				dwBlockCounter++; 
			}
		}
	}
	return dwByteCount-1;
}


/****************************************************************/
/*																*/
/* Memo field function: Save data to memo field by Field name.	*/
/*																*/
/****************************************************************/
int CDbaseFile::PutMemoField(LPCTSTR lpszFieldName, char* buff, DWORD dwLength)
{
	CField *fp = GetFieldPtr(lpszFieldName);

	if(fp == NULL)
	{
		// not a valid field index -> return empty buffer
		m_strErrorMessage = "Invalid field index";
		return 0;
	}
	return PutMemoField(fp->m_FieldNumber, buff, dwLength);
}


/****************************************************************/
/*																*/
/* Memo field function: Reset memo field index.					*/
/*																*/
/****************************************************************/
int CDbaseFile::ClearMemoField(int nFieldIndex)
{
	if (m_MemoFile.m_hFile == NULL)
		return DBASE_NOT_OPEN;

	// reset memo field index
	return PutNumericField(nFieldIndex, 0);
}


/****************************************************************/
/*																*/
/* Memo field function: Reset memo field index.					*/
/*																*/
/****************************************************************/
int CDbaseFile::ClearMemoField(LPCTSTR lpszFieldName)
{
	CField *fp = GetFieldPtr(lpszFieldName);

	if(fp == NULL)
	{
		// not a valid field index
		m_strErrorMessage = "Invalid field index";
		return DBASE_INVALID_FIELDNO;
	}
	return ClearMemoField(fp->m_FieldNumber);
}


/****************************************************************/
/*																*/
/* Memo field function: Save data to memo field by field index.	*/
/*																*/
/****************************************************************/
int CDbaseFile::PutMemoField(int nFieldIndex, char* buff, DWORD dwLength)
{
	if (m_MemoFile.m_hFile == NULL)
		return DBASE_NOT_OPEN;

	DWORD dwTotalBlocks = ((dwLength+2) / m_dwMemoBlockSize) + 1;

	try
	{
		// always append new data at the end of file 
		m_dwMemoNextFreeBlock = m_MemoFile.SeekToEnd()/m_dwMemoBlockSize;

		DWORD dwTotalCopied = 0;   
		DWORD dwBlockCounter = 0;
		DWORD dwBlockNo = m_dwMemoNextFreeBlock;

		for(DWORD i = 0; i < dwTotalBlocks; i++)
		{
			// empty block
			memset(m_MemoDataPtr, 0x00, m_dwMemoBlockSize);

			// copy buffer data to memo block
			while(dwBlockCounter < m_dwMemoBlockSize)
			{
				if(dwTotalCopied >= dwLength)
				{
					// end of field marker 
					m_MemoDataPtr[dwBlockCounter++] = 0x1a; 
					m_MemoDataPtr[dwBlockCounter++] = 0x1a; 
					break;
				}
				else
				{
					m_MemoDataPtr[dwBlockCounter++] = buff[dwTotalCopied];  
				}
				dwTotalCopied++;
			}

			// goto specified memo block
			m_MemoFile.Seek(dwBlockNo * m_dwMemoBlockSize, CFile::begin); 
			// write data
			m_MemoFile.Write(m_MemoDataPtr, m_dwMemoBlockSize);

			dwBlockNo++;
			dwBlockCounter = 0;
		}

		// update memo start block
		PutNumericField(nFieldIndex, m_dwMemoNextFreeBlock);
		
		// update memo header
		m_dwMemoNextFreeBlock += dwTotalBlocks;
		m_MemoFile.SeekToBegin();
		m_MemoFile.Write(&m_dwMemoNextFreeBlock, 4);
		return DBASE_SUCCESS;
	}
	catch(CFileException *e)
	{
		e->Delete();
		m_strErrorMessage = "An error occured while writing to memo file";
		return DBASE_WRITE_ERROR;
	}	
	return DBASE_SUCCESS;
}

#include "ProgDlg.h"
#include "StructureDlg.h"
#include "ExportFileDialog.h"
#include "SetFilterDlg.h"

CDBase::CDBase()
{
	m_FileName=_T("");
	m_dBaseFile=NULL;
}

CDBase::CDBase(LPCTSTR lpszPathName)
{
	if (m_dBaseFile->Open(lpszPathName) != DBASE_SUCCESS)
	{
		m_FileName=_T("");
		m_dBaseFile=NULL;
	}
	else
		m_FileName=lpszPathName;
}

CDBase::~CDBase()
{
	if(m_dBaseFile!=NULL)
		m_dBaseFile->Close();
}

/********************************************************************/
/*																	*/
/* Function name : OnNewDBase									*/
/* Description   :													*/
/*																	*/
/********************************************************************/
BOOL CDBase::NewDBase()
{
	CStructureDlg dlg;
	dlg.m_strTitle = "新建DBF文件";
	if (dlg.DoModal() == IDOK)
	{
		char szPath[MAX_PATH];
		GetTempPath(sizeof(szPath), szPath);
		if (szPath[lstrlen(szPath)-1] != '\\')
			lstrcat(szPath, "\\");
	
		CTime date = CTime::GetCurrentTime();
		CString DateStr;
		DateStr.Format ("%d%d%d",date.GetYear (),date.GetMonth() ,date.GetDay ());
		CTime time = CTime::GetCurrentTime();
		CString strTime = DateStr+time.Format(_T("%I%M"));
		wsprintf(m_szTempFileName, "%s%s.dbf", szPath,strTime);
		if (m_dBaseFile->Create(m_szTempFileName, dlg.m_strFieldArray) == DBASE_SUCCESS)
		{
			if (OpenDBase(m_szTempFileName))
				return TRUE;
			return FALSE;
		}
	}
	return FALSE;
}


/********************************************************************/
/*																	*/
/* Function name : OnOpenDBase									*/
/* Description   :													*/
/*																	*/
/********************************************************************/
BOOL CDBase::OpenDBase(LPCTSTR lpszPathName) 
{
	// open database
	if(m_dBaseFile!=NULL)
		m_dBaseFile->Close();
	if (m_dBaseFile->Open(lpszPathName) != DBASE_SUCCESS)
	{
		return FALSE;
	}
	m_FileName=lpszPathName;
	return TRUE;
}


/********************************************************************/
/*																	*/
/* Function name : OnCloseDBase									*/
/* Description   :													*/
/*																	*/
/********************************************************************/
void CDBase::CloseDBase() 
{
	// close database
	m_dBaseFile->Close();

	// delete temperary files
	DeleteFile(m_szTempFileName);
	DeleteFile(m_dBaseFile->GetMemoFileName(m_szTempFileName));
}


/********************************************************************/
/*																	*/
/* Function name : OnSaveDBase									*/
/* Description   :													*/
/*																	*/
/********************************************************************/
BOOL CDBase::SaveDBase(LPCTSTR lpszPathName) 
{
	// close database
	m_dBaseFile->Close();

	if (lstrcmp(m_szTempFileName, "") != 0)
	{
		// copy memo file
		CString strMemoFile1 = m_dBaseFile->GetMemoFileName(m_szTempFileName);
		CString strMemoFile2 = m_dBaseFile->GetMemoFileName(lpszPathName);

		CopyFile(strMemoFile1, strMemoFile2, FALSE);

		if (CopyFile(m_szTempFileName, lpszPathName, FALSE))
		{
			// empty name
			lstrcpy(m_szTempFileName, "");
			return OpenDBase(lpszPathName);
		}
		else
			return FALSE;
	}

	// copy memo file
	CString strMemoFile1 = m_dBaseFile->GetMemoFileName(m_FileName);
	CString strMemoFile2 = m_dBaseFile->GetMemoFileName(lpszPathName);

	CopyFile(strMemoFile1, strMemoFile2, FALSE);

	// make copy and open new file
	if (CopyFile(m_FileName, lpszPathName, FALSE))
	{
		return OpenDBase(lpszPathName);
	}

	return OpenDBase(lpszPathName);
}


/********************************************************************/
/*																	*/
/* Function name : OnFileStructure									*/
/* Description   : Change file structure							*/
/*																	*/
/********************************************************************/
void CDBase::FileStructure() 
{
	CStructureDlg dlg;

	// copy fieldnames
	for (int i=1; i<=m_dBaseFile->GetFieldCount(); i++)
	{
		CField *pField = m_dBaseFile->GetFieldPtr(i);
		if (pField)
		{
			CString strFieldData;
			strFieldData.Format("%s,%c,%d,%d", pField->m_Name, pField->m_Type, pField->m_Length, pField->m_DecCount);
			dlg.m_strFieldArray.Add(strFieldData);
		}
	}

	if (dlg.DoModal() == IDOK)
	{
		if(MessageBox(GetFocus(), "将更新当前数据库结构.\n在更数据库结构时可能有些数据会丢失.\n是否确认更新当前数据库结构?", "测井数据库管理", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			// close database
			m_dBaseFile->Close();
	
			CString strFileName, strFileBackup, strMemoBackup;

			// temp file ?
			if (lstrcmp(m_szTempFileName, "") != 0)
			{
				strFileName = m_szTempFileName;
			}
			else
				strFileName = m_FileName;

			strFileBackup = strFileName;
			strMemoBackup = m_dBaseFile->GetMemoFileName(strFileName);

			// make backup names
			int nPos = strFileBackup.ReverseFind('\\');
			if (nPos != -1)
			{
				strFileBackup.SetAt(nPos+1, '_');
				strMemoBackup.SetAt(nPos+1, '_');
			}
			// delete existing backups
			DeleteFile(strFileBackup);
			DeleteFile(strMemoBackup);

			// make backup copy of existing file
			if (!MoveFile(strFileName, strFileBackup))
			{
				MessageBox(GetFocus(), "Error while making backup", "Updating structure", MB_ICONSTOP);
				return;
			}
			
			// make backup copy of memo file
			MoveFile(m_dBaseFile->GetMemoFileName(strFileName), strMemoBackup);

			// create file with new structure
			if (m_dBaseFile->Create(strFileName, dlg.m_strFieldArray) == DBASE_SUCCESS)
			{
				if (OpenDBase(strFileName))
				{
					CopyBackupData(strFileBackup);

					// reload records
//					POSITION pos = GetFirstViewPosition();
//					while (pos != NULL)
//						((CDBFExplorerView *)GetNextView(pos))->ShowRecords(GetActiveFrame()->m_bShowDeletedRecords);
				}
			}
		}
	}	
}


/********************************************************************/
/*																	*/
/* Function name : OnFilePackdatabase								*/
/* Description   : Remove deleted records							*/
/*																	*/
/********************************************************************/
void CDBase::FilePackdatabase() 
{
	if (MessageBox(GetFocus(), "是否确认彻底删除当前数据库中的标注记录?", "数据库整理", MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;

	if (m_dBaseFile->Pack() == DBASE_SUCCESS)
	{
		MessageBox(GetFocus(), "数据库整理成功.", "数据库整理", MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(GetFocus(), "数据库整理失败!", "数据库整理", MB_OK | MB_ICONSTOP);
	}

	// reload records
//	POSITION pos = GetFirstViewPosition();
//	while (pos != NULL)
//		((CDBFExplorerView *)GetNextView(pos))->ShowRecords(FALSE);
}


/********************************************************************/
/*																	*/
/* Function name : CopyBackupData									*/
/* Description   : Try to copy data from backup file to the new file*/
/*																	*/
/********************************************************************/
BOOL CDBase::CopyBackupData(LPCTSTR lpszBackupFile)
{
	// copy all data
	CDbaseFile backupDBF;

	DWORD dwCounter = 0;
	char szBuff[255];

	// open backup file
	if (backupDBF.Open(lpszBackupFile) == DBASE_SUCCESS)
	{
		// show all records
		for(int rc=backupDBF.GetFirstRecord(); rc==DBASE_SUCCESS; rc=backupDBF.GetNextRecord())
  		{
			dwCounter++;
			m_dBaseFile->AddRecord();
			m_dBaseFile->GetRecord(dwCounter);

			// copy all fields
			for (int i=1; i<=backupDBF.GetFieldCount(); i++)
			{
				CField *pBackupField = backupDBF.GetFieldPtr(i);
				CField *pField = m_dBaseFile->GetFieldPtr(pBackupField->m_Name);
				// check if field exists in new database
				if (pField)
				{
					if (backupDBF.GetField(i, szBuff))
					{
						m_dBaseFile->PutField(pBackupField->m_Name, szBuff);
					}
					
					// copy memo data
					if (pField->m_Type == 'M' && pBackupField->m_Type == 'M')
					{
						char *buff;
						CString strMemo;

						int nLength = backupDBF.GetMemoFieldLength(i);
						if (nLength)
						{
							try
							{
								buff = new char[nLength+1];
								backupDBF.GetMemoField(i, buff, nLength);
								strMemo.Format("%s", buff);
								delete buff;
							}
							catch(...)
							{
							}
						}			
						// set value
						m_dBaseFile->PutMemoField(pBackupField->m_Name, (char *)(LPCTSTR)strMemo, strMemo.GetLength());
					}
					else
					if (pField->m_Type == 'M')
					{
						// initialize data
						m_dBaseFile->ClearMemoField(pBackupField->m_Name);
					}
				}
				else
				{
					// field does not exists-> do not copy data
				}
			}
			m_dBaseFile->PutRecord(dwCounter);
		}
		backupDBF.Close();
		return TRUE;
	}
	return FALSE;
}


/********************************************************************/
/*																	*/
/* Function name : OnFileExport										*/
/* Description   : Export data to other format						*/
/*																	*/
/********************************************************************/
void CDBase::FileExport() 
{
 	static char szFilters[] = "Text Files (*.List;*.csv)|*.List;*.csv|HTML Files (*.html;*.htm)|*.html;*.htm||";
	
	CString strFileName = m_FileName;
	int nPos = strFileName.ReverseFind('.');
	if (nPos != -1)
		strFileName = strFileName.Left(nPos);
	
	CExportFileDialog dlg(FALSE, NULL, strFileName, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters);

	dlg.m_ofn.lpstrTitle = "保存文件";

	if (dlg.DoModal() == IDOK)
	{
		strFileName = dlg.GetPathName();
		CString strExtension = dlg.GetFileExt();
		if (dlg.m_strType == "Text Files (*.List;*.csv)")
		{
			if (strExtension.IsEmpty())
				strFileName += ".List";

			ExportToText(strFileName);
		}
		else
		if (dlg.m_strType == "HTML Files (*.html;*.htm)")
		{
			if (strExtension.IsEmpty())
				strFileName += ".html";

			ExportToHTML(strFileName);
		}
	}	
}


/********************************************************************/
/*																	*/
/* Function name : ExportToText										*/
/* Description   : Export data to text format						*/
/*																	*/
/********************************************************************/
void CDBase::ExportToText(LPCTSTR lpszFileName)
{
	try
	{
		CFile file;
		CString strHTML;

		// Create "progress bar"
		CProgressDlg wndProgress;
		wndProgress.Create(CWnd::GetActiveWindow());
		wndProgress.SetWindowText("Text 输出");
    
		// Initialize progress control range and step size
		wndProgress.SetRange(0, 100);

		int nCount = m_dBaseFile->GetRecordCount();

		if (file.Open(lpszFileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, NULL))
		{
			// show fieldnames
			for (int i=1; i<=m_dBaseFile->GetFieldCount(); i++)
			{
				CField *pField = m_dBaseFile->GetFieldPtr(i);
				if (pField)
				{
					strHTML.Format("%s", pField->m_Name);				
					if (i != 1)
						file.Write(",", 1);
					file.Write(strHTML, strHTML.GetLength());
				}
			}
			file.Write("\r\n", 2);
	    	// show all records
			for(int rc=m_dBaseFile->GetFirstRecord(); rc==DBASE_SUCCESS; rc=m_dBaseFile->GetNextRecord())
  			{
				// Test for "Esc" key
				if( ::GetAsyncKeyState(VK_ESCAPE) < 0)
				{
					break;
				}

				int nItem = m_dBaseFile->GetCurRecNo() - 1;
				
				// do not show deleted records
				if (m_dBaseFile->IsRecordDeleted())
					continue;

				for (int i=1; i<=m_dBaseFile->GetFieldCount(); i++)
				{
					char szBuff[255];

					CField *pField = m_dBaseFile->GetFieldPtr(i);
					if (pField->m_Type == 'M')
					{
						lstrcpy(szBuff, "MEMO");
					}
					else
					{
						m_dBaseFile->GetField(i, szBuff);
					}
					if (i != 1)
						file.Write(",", 1);
					file.Write(szBuff, lstrlen(szBuff));
				}
				file.Write("\r\n", 2);
				// Update progress control
				int nPos = (nItem*100)/nCount;
				wndProgress.SetPos(nPos);
			}	
		}
	}
	catch(CFileException *e)
	{
		e->Delete();
		MessageBox(GetFocus(), "Text 输出失败!", "Text 输出", MB_OK | MB_ICONSTOP);
	}
}	


/********************************************************************/
/*																	*/
/* Function name : ExportToHTML										*/
/* Description   : Export data to HTML format						*/
/*																	*/
/********************************************************************/
void CDBase::ExportToHTML(LPCTSTR lpszFileName)
{
	try
	{
		CFile file;
		CString strHTML;

		// Create "progress bar"
		CProgressDlg wndProgress;
		wndProgress.Create(CWnd::GetActiveWindow());
		wndProgress.SetWindowText("HTML 输出");
		// Initialize progress control range and step size
		wndProgress.SetRange(0, 100);

		int nCount = m_dBaseFile->GetRecordCount();

		if (file.Open(lpszFileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, NULL))
		{
			strHTML = "<HTML>\r\n";
			file.Write(strHTML, strHTML.GetLength());
			strHTML = "<HEAD>\r\n";
			file.Write(strHTML, strHTML.GetLength());
			strHTML = "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;charset=宋体\">\r\n";
			file.Write(strHTML, strHTML.GetLength());
			CString szTitle=m_FileName;
			int xxx=szTitle.ReverseFind ('.');
			if(xxx>0)
				szTitle=szTitle.Left (xxx);
			szTitle=szTitle.Mid (szTitle.ReverseFind ('\\')+1);
			strHTML.Format("<TITLE>%s</TITLE>\r\n", szTitle);
			file.Write(strHTML, strHTML.GetLength());
			strHTML = "</HEAD>\r\n";
			file.Write(strHTML, strHTML.GetLength());
			strHTML = "<BODY>\r\n";
			file.Write(strHTML, strHTML.GetLength());
			strHTML.Format("<TABLE BORDER=1 BGCOLOR=#c0c0c0 CELLSPACING=0><FONT FACE=\"宋体\" COLOR=#080000><CAPTION><B>%s</B></CAPTION></FONT>\r\n", szTitle);
			file.Write(strHTML, strHTML.GetLength());

			strHTML = "<THEAD>\r\n<TR>\r\n";
			file.Write(strHTML, strHTML.GetLength());

			// show fieldnames
			for (int i=1; i<=m_dBaseFile->GetFieldCount(); i++)
			{
				CField *pField = m_dBaseFile->GetFieldPtr(i);
				if (pField)
				{//c0c0c0
					strHTML.Format("<TD BGCOLOR=#0FF000 BORDERCOLOR=#c0c0c0 ><FONT SIZE=2 FACE=\"方正姚体\" COLOR=#000000>%s</FONT></TH>\r\n", pField->m_Name);
					file.Write(strHTML, strHTML.GetLength());
				}
			}
			strHTML = "</TR>\r\n</THEAD>\r\n\r\n<TBODY>\r\n";
			file.Write(strHTML, strHTML.GetLength());

			// show all records
			for(int rc=m_dBaseFile->GetFirstRecord(); rc==DBASE_SUCCESS; rc=m_dBaseFile->GetNextRecord())
  			{
				// Test for "Esc" key
				if( ::GetAsyncKeyState(VK_ESCAPE) < 0)
				{
					break;
				}

				int nItem = m_dBaseFile->GetCurRecNo() - 1;
				
				// do not show deleted records
				if (m_dBaseFile->IsRecordDeleted())
					continue;

				strHTML = "<TR VALIGN=TOP>\r\n";
				file.Write(strHTML, strHTML.GetLength());
				
				for (int i=1; i<=m_dBaseFile->GetFieldCount(); i++)
				{
					char szBuff[255];

					CField *pField = m_dBaseFile->GetFieldPtr(i);
					if (pField->m_Type == 'M')
					{
						lstrcpy(szBuff, "MEMO");
					}
					else
					{
						m_dBaseFile->GetField(i, szBuff);
					}
					strHTML.Format("<TD BGCOLOR=#FFFFFF BORDERCOLOR=#c0c0c0 ><FONT SIZE=2 FACE=\"宋体\" COLOR=#800000>%s</FONT></TD>\r\n", szBuff);
					file.Write(strHTML, strHTML.GetLength());
				}

				strHTML = "</TR>\r\n";
				file.Write(strHTML, strHTML.GetLength());

				// Update progress control
				int nPos = (nItem*100)/nCount;
				wndProgress.SetPos(nPos);
			}	
			strHTML = "</TBODY>\r\n<TFOOT></TFOOT>\r\n</TABLE>\r\n";
			file.Write(strHTML, strHTML.GetLength());
			strHTML = "</BODY>\r\n";
			file.Write(strHTML, strHTML.GetLength());
			strHTML = "</HTML>\r\n";
			file.Write(strHTML, strHTML.GetLength());
		}
	}
	catch(CFileException *e)
	{
		e->Delete();
		MessageBox(GetFocus(), "HTML 输出失败!", "HTML 输出", MB_OK | MB_ICONSTOP);
	}
}

BOOL CDBase::SetFilter(LPCTSTR lpszFieldName, LPCTSTR lpszCriteria)
{
	CSetFilterDlg dlg;
	dlg.m_FilterCriteria =lpszCriteria;
	dlg.m_FilterFieldName =lpszFieldName;
	
	for (int i=1; i<=m_dBaseFile->GetFieldCount(); i++)
	{
		CField *pField = m_dBaseFile->GetFieldPtr(i);
		if (pField)
		{
			CString strFieldData;
			strFieldData.Format("%s,%c,%d,%d", pField->m_Name, pField->m_Type, pField->m_Length, pField->m_DecCount);
			dlg.m_strFieldArray.Add(strFieldData);
		}
	}
	if (dlg.DoModal() != IDOK)
		return FALSE;
	lpszCriteria=dlg.m_FilterCriteria ;
	lpszFieldName=dlg.m_FilterFieldName ;

	CString TempFileName=lpszCriteria;
	TempFileName.TrimRight ();
		
	char szPath[MAX_PATH];
	GetTempPath(sizeof(szPath), szPath);
	if (szPath[lstrlen(szPath)-1] != '\\')
		lstrcat(szPath, "\\");

	TempFileName=szPath+TempFileName+".DO";
	if(!m_dBaseFile->CloneDatabase (TempFileName,TRUE,TRUE))
	{
		AfxMessageBox("创建筛选库文件失败!");
		return FALSE;
	}
	// open database
	CDbaseFile TempBaseFile;
	if (TempBaseFile.Open(TempFileName) != DBASE_SUCCESS)
	{
		AfxMessageBox("打开筛选库文件失败!");
		return FALSE;
	}
	
	// Create "progress bar"
	CProgressDlg wndProgress;
	wndProgress.Create(CWnd::GetActiveWindow());
	wndProgress.SetWindowText("筛选记录");
	// Initialize progress control range and step size
	wndProgress.SetRange(0, 100);

	int nCount = TempBaseFile.GetRecordCount();
	int nPos;
	char buff[255];
	// walk through all records  
	for(int nResult=TempBaseFile.GetRecord(1); nResult==DBASE_SUCCESS; nResult=TempBaseFile.GetNextRecord())
  	{
		// Test for "Esc" key
		if( ::GetAsyncKeyState(VK_ESCAPE) < 0)
		{
			break;
		}
		nPos = (nResult*100)/nCount;
		wndProgress.SetPos(nPos);

		TempBaseFile.GetField(lpszFieldName, buff);
		if (lstrcmp(buff, lpszCriteria))
			TempBaseFile.DeleteRecord ();              			
	}
	TempBaseFile.Pack ();
	TempBaseFile.Close ();
	OpenDBase(TempFileName);
	return TRUE;
}

// DecodeYst.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

//************************************
// Method:    GetFileFullName ��ȡ�ļ�������
// FullName:  GetFileFullName
// Access:    public 
// Returns:   CString ���أ�softreqhhh202102010201.docx
// Qualifier:
// Parameter: CString csFilePath  ��Σ��ļ�·���� D:\\download\\hr\\softreqhhh202102010201.docx
//************************************
CString GetFileFullName(CString csFilePath)
{
	int nPos = csFilePath.ReverseFind('\\'); // �ļ�·������'\'б�ָܷ���·��  
	CString csFileFullName;
	// ��ȡ�ļ�ȫ���������ļ�������չ�� 
	csFileFullName = csFilePath.Right(csFilePath.GetLength() - nPos - 1);  
	return csFileFullName;
}

//************************************
// Method:    GetFileName ��ȡ�ļ�����չ��ǰ�沿�֣�
// FullName:  GetFileName
// Access:    public 
// Returns:   CString 
// Qualifier:
// Parameter: CString csFileFullName
//************************************
CString GetFileName(CString csFileFullName)
{
	int nPos = csFileFullName.ReverseFind('.');
	// ��ȡ�ļ��� 
	//�� softreqhhh202102010201��
	// �� "D:\\download\\hr\\softreqhhh202102010201"
	CString  csFileName = csFileFullName.Left(nPos);  
	return csFileName;
}

//************************************
// Method:    GetFileExtName ��ȡ�ļ���չ��
// FullName:  GetFileExtName
// Access:    public 
// Returns:   CString ���� "docx"
// Qualifier:
// Parameter: CString csFileFullName
//************************************
CString  GetFileExtName(CString csFileFullName)
{
	int nPos = csFileFullName.ReverseFind('.');
	// ��ȡ��չ�� 
	CString  csFileExtName = csFileFullName.Right(csFileFullName.GetLength() - nPos - 1);  
	return csFileExtName;
}

//************************************
// Method:    GetFileParentDir ��ȡ�ļ������ļ���
// FullName:  GetFileParentDir
// Access:    public 
// Returns:   CString ���ظ�Ŀ¼������ "D:\\download\\hr\\"
// Qualifier:
// Parameter: CString csFilePath Ŀ¼�����ļ��У����� D:\\download\\hr\\ssssssssss
//************************************
CString GetFileParentDir(CString csFilePath)
{
	CFileFind find;
	BOOL bResult = find.FindFile(csFilePath);  //�ж���Ŀ¼�Ƿ����  

	find.FindNextFile();

	CString RootDir = find.GetRoot();  //��Ŀ¼  
	return RootDir;
}


//************************************
// Method:    FindFileInDir  �ݹ��ȡĿ¼�µ������ļ�
// FullName:  FindFileInDir
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CString rootDir ��Σ��ļ���
// Parameter: vector<CString> & csFileList ���Σ��ļ�·���б�
//************************************
void FindFileInDir(CString rootDir, vector<CString>& csFileList)
{
	// ���ҵ�ǰ·���µ������ļ��к��ļ�
	CString strDir = rootDir;
	strDir += "\\*.*";

	// �����õ��������ļ�����
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strDir);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDirectory() && "." != finder.GetFileName() && ".." != finder.GetFileName())
		{
			//�ݹ����
			CString subDirName = finder.GetFileName();
			//.��ͷ���ļ�������
			if ('.' != subDirName.GetAt(0))
			{
				FindFileInDir(finder.GetFilePath(), csFileList);
			}
		}
		else
		{

			CString strFilePath = finder.GetFilePath();
			CString strSubFileName = finder.GetFileName();
			if (strSubFileName != "." && strSubFileName != "..")
			{
				csFileList.push_back(strFilePath);
			}
		}
	}
	finder.Close();
}

//************************************
// Method:    CreateMultiDirectory �����༶Ŀ¼
// FullName:  CreateMultiDirectory
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: CString strPath
//************************************
BOOL CreateMultiDirectory(CString strPath)
{
	CString strSubPath;
	CString strMsg;
	int nCount = 0;
	int nIndex = 0;
	//ͨ����\�����ָ�·�����Ӷ�����������Ŀ¼��
	do
	{
		nIndex = strPath.Find(_T("\\"), nIndex) + 1;
		nCount++;
	} while ((nIndex - 1) != -1);

	//������ļ��Ļ���������ļ��������ļ��д�����ȥ1�����������Ա���
	nCount -= 1;

	nIndex = 0;
	//��飬������Ŀ¼
	while ((nCount - 1) >= 0)
	{
		nIndex = strPath.Find(_T("\\"), nIndex) + 1;
		if ((nIndex - 1) == -1)
			strSubPath = strPath;
		else
			strSubPath = strPath.Left(nIndex);
		if (!PathFileExists(strSubPath))// - ���Ŀ¼�Ƿ����
		{
			if (!CreateDirectory(strSubPath, NULL))// -�������򴴽�Ŀ¼
			{
				return FALSE;
			}
		}
		nCount--;
	};
	return TRUE;
}

//************************************
// Method:    DoDecodeFile ִ��ʵ��ת���������ļ�
// FullName:  DoDecodeFile
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CString filePathSrc
// Parameter: CString filePahtDest
//************************************
void DoDecodeFile(CString filePathSrc, CString filePathDest)
{
	//1. Ԥ����
	CString csFileNameNoExt = GetFileName(filePathSrc);  //�ļ�·��ȥ����չ������
	CString csFileExtName = GetFileExtName(filePathSrc); //��չ��
				
	//1.1 �м��ļ�·��
	CString filePathTemp = csFileNameNoExt + "_new.tempFileYYDS";

	//1.2 ������������չ������matlab�ļ����޸�Ϊ�ı��ļ�����
	BOOL bBack = FALSE;
	CString oriFilePath = filePathSrc;
	CString newFilePath;
	if (csFileExtName.Compare("m") == 0 || csFileExtName.Compare("sln") == 0)
	{
		newFilePath = csFileNameNoExt + "_new.txt";
		CopyFile(filePathSrc, newFilePath, FALSE);
		filePathSrc = newFilePath;
		bBack = TRUE;
	}

	//2. �����ļ����м��ļ�
	//2.1 ��������
	char tmp[1024];
	CString csCmdLine;
	//sprintf_s(tmp, sizeof(tmp), "start cmd.exe  /c ");
	//sprintf_s(tmp, sizeof(tmp), "cmd.exe /c ");
	//csCmdLine = tmp;
	sprintf_s(tmp, sizeof(tmp), "copy \"%s\" \"%s\" /y ", 
		filePathSrc.GetString(), filePathTemp.GetString());
	csCmdLine.Append(tmp);	
	//printf("%s\n", csCmdLine.GetString());

	CHAR pszCmdLine[MAX_PATH * 2];
	GetSystemDirectory(pszCmdLine, MAX_PATH);
	lstrcat(pszCmdLine, "\\cmd.exe /c ");
	lstrcat(pszCmdLine, csCmdLine.GetString());

	//2.2 ִ������
	//system(csCmdLine);		
	//WinExec(csCmdLine, SW_HIDE);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	// Start the child process. 
	BOOL bSucc = CreateProcess(NULL,   // No module name (use command line)
		pszCmdLine,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		TRUE,          // Set handle inheritance to FALSE
		NORMAL_PRIORITY_CLASS,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi);           // Pointer to PROCESS_INFORMATION structure

	if (!bSucc)
	{
		return;
	}
	// Wait for process to end
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
		
	//3. ������ΪĿ���ļ�	
	//3.1 ���Ŀ���ļ��Ѵ��ڣ���shanchu
	BOOL bFile = PathFileExists(filePathDest);
	if (bFile)
	{
		DeleteFile(filePathDest);
	}

	//3.2 �ļ�������
	//::Sleep(1000);
	//CFile::Rename(filePathTemp, filePathDest);
	MoveFile(filePathTemp, filePathDest);

	//���ƹ����ļ���������ɾ��
	if (bBack)
	{
		DeleteFile(newFilePath);
	}
	printf("ת���ļ� %s --> %s \n", filePathSrc.GetString(), filePathDest.GetString());
}

//************************************
// Method:    DecodeFile ת��һ���ļ�
// FullName:  DecodeFile
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CString filePath ��Σ��ļ�ȫ·����D:\\download\\hr\\softreqhhh202102010201.docx
//************************************
void DecodeFile(CString filePath)
{
	//1. ��ȡ�ļ�·�������Ϣ
	CString csFileFullName = GetFileFullName(filePath);
	CString csFileName = GetFileName(filePath);
	CString csFileExtName = GetFileExtName(csFileFullName);

	//2. ��ȡĿ���ļ���
	//2.2 Ŀ���ļ�·��
	CString csFileDestPath = csFileName + "_new." + csFileExtName;

	//3.����ת��
	DoDecodeFile(filePath, csFileDestPath);		
}


//************************************
// Method:    DoDecodeDirectory ת��һ��Ŀ¼
// FullName:  DoDecodeDirectory
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CString dirSrcPath ��Σ�ԴĿ¼
// Parameter: CString dirDestPath ��Σ�Ŀ��Ŀ¼
//************************************
void DoDecodeDirectory(CString dirSrcPath, CString dirDestPath)
{
	//1. �ҵ����д�ת���ļ�
	vector<CString> csFileSrcList;
	FindFileInDir(dirSrcPath, csFileSrcList);

	//2. ת��ΪĿ���ļ�·��,������ת��
	//2.1 ����Ŀ���ļ�·��	
	for (unsigned int i = 0; i < csFileSrcList.size(); ++i)
	{
		CString srcFilePath = csFileSrcList.at(i);
		CString dstFilePath = srcFilePath;				
		dstFilePath.Replace(dirSrcPath, dirDestPath);
		
		//2.2 �ݹ鴴���ļ���		
		BOOL bSucc = CreateMultiDirectory(dstFilePath);
		if (!bSucc)
		{
			continue;
		}

		//3. �����ļ�ת��
		DoDecodeFile(srcFilePath, dstFilePath);
	}
}

//************************************
// Method:    DecodeDirectory ��һ���ļ��н���ת��
// FullName:  DecodeDirectory
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CString dirPath ��Σ��ļ���Ŀ¼
//************************************
void DecodeDirectory(CString dirPath)
{
	//1. ��ȡ�ļ�·�������Ϣ
	CString csDirName = GetFileFullName(dirPath);	
	CString csDirNameNew = dirPath + "_new";
	
	//2. ����ת��
	DoDecodeDirectory(dirPath, csDirNameNew);	
}


int main(int argc, char* argv[])
{
	//1. ��ȡ�����ļ����ļ���·��
	printf("ȷ������ͨ�ѵ�½.\n");
	printf("�������ļ����ļ���·��,�м��޿ո�.\n");
	string strFileDirIn;
	getline(cin, strFileDirIn);	
	//cout << strFileDirIn << endl;

	//2. �ж���������
	CString strPath = strFileDirIn.c_str();
	BOOL bFile = PathFileExists(strPath);
	if (!bFile)
	{
		printf("�ļ���Ŀ¼ %s ������! \n", strPath.GetString());
		system("pause");
		return 0;
	}
	BOOL bDir = GetFileAttributes(strPath) & FILE_ATTRIBUTE_DIRECTORY;
	if (bDir)
	{
		//�ļ���
		DecodeDirectory(strPath);
	}
	else
	{
		//�ļ�
		DecodeFile(strPath);
	}

	// �ȴ�������˳�
	system("pause");
    return 0;
}


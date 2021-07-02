// DecodeYst.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

//************************************
// Method:    GetFileFullName 获取文件短名称
// FullName:  GetFileFullName
// Access:    public 
// Returns:   CString 返回，softreqhhh202102010201.docx
// Qualifier:
// Parameter: CString csFilePath  入参，文件路径， D:\\download\\hr\\softreqhhh202102010201.docx
//************************************
CString GetFileFullName(CString csFilePath)
{
	int nPos = csFilePath.ReverseFind('\\'); // 文件路径，以'\'斜杠分隔的路径  
	CString csFileFullName;
	// 获取文件全名，包括文件名和扩展名 
	csFileFullName = csFilePath.Right(csFilePath.GetLength() - nPos - 1);  
	return csFileFullName;
}

//************************************
// Method:    GetFileName 获取文件名扩展名前面部分，
// FullName:  GetFileName
// Access:    public 
// Returns:   CString 
// Qualifier:
// Parameter: CString csFileFullName
//************************************
CString GetFileName(CString csFileFullName)
{
	int nPos = csFileFullName.ReverseFind('.');
	// 获取文件名 
	//如 softreqhhh202102010201。
	// 或 "D:\\download\\hr\\softreqhhh202102010201"
	CString  csFileName = csFileFullName.Left(nPos);  
	return csFileName;
}

//************************************
// Method:    GetFileExtName 获取文件扩展名
// FullName:  GetFileExtName
// Access:    public 
// Returns:   CString 返回 "docx"
// Qualifier:
// Parameter: CString csFileFullName
//************************************
CString  GetFileExtName(CString csFileFullName)
{
	int nPos = csFileFullName.ReverseFind('.');
	// 获取扩展名 
	CString  csFileExtName = csFileFullName.Right(csFileFullName.GetLength() - nPos - 1);  
	return csFileExtName;
}

//************************************
// Method:    GetFileParentDir 获取文件所在文件夹
// FullName:  GetFileParentDir
// Access:    public 
// Returns:   CString 返回父目录，形如 "D:\\download\\hr\\"
// Qualifier:
// Parameter: CString csFilePath 目录所在文件夹，形如 D:\\download\\hr\\ssssssssss
//************************************
CString GetFileParentDir(CString csFilePath)
{
	CFileFind find;
	BOOL bResult = find.FindFile(csFilePath);  //判断子目录是否存在  

	find.FindNextFile();

	CString RootDir = find.GetRoot();  //父目录  
	return RootDir;
}


//************************************
// Method:    FindFileInDir  递归获取目录下的所有文件
// FullName:  FindFileInDir
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CString rootDir 入参，文件夹
// Parameter: vector<CString> & csFileList 出参，文件路径列表
//************************************
void FindFileInDir(CString rootDir, vector<CString>& csFileList)
{
	// 查找当前路径下的所有文件夹和文件
	CString strDir = rootDir;
	strDir += "\\*.*";

	// 遍历得到所有子文件夹名
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strDir);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDirectory() && "." != finder.GetFileName() && ".." != finder.GetFileName())
		{
			//递归调用
			CString subDirName = finder.GetFileName();
			//.开头的文件夹跳过
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
// Method:    CreateMultiDirectory 创建多级目录
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
	//通过“\”来分割路径，从而创建各级的目录。
	do
	{
		nIndex = strPath.Find(_T("\\"), nIndex) + 1;
		nCount++;
	} while ((nIndex - 1) != -1);

	//如果是文件的话，最后会把文件名当做文件夹处理，减去1个索引，可以避免
	nCount -= 1;

	nIndex = 0;
	//检查，并创建目录
	while ((nCount - 1) >= 0)
	{
		nIndex = strPath.Find(_T("\\"), nIndex) + 1;
		if ((nIndex - 1) == -1)
			strSubPath = strPath;
		else
			strSubPath = strPath.Left(nIndex);
		if (!PathFileExists(strSubPath))// - 检查目录是否存在
		{
			if (!CreateDirectory(strSubPath, NULL))// -不存在则创建目录
			{
				return FALSE;
			}
		}
		nCount--;
	};
	return TRUE;
}

//************************************
// Method:    DoDecodeFile 执行实际转换，单个文件
// FullName:  DoDecodeFile
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CString filePathSrc
// Parameter: CString filePahtDest
//************************************
void DoDecodeFile(CString filePathSrc, CString filePathDest)
{
	//1. 预处理
	CString csFileNameNoExt = GetFileName(filePathSrc);  //文件路径去掉扩展名部分
	CString csFileExtName = GetFileExtName(filePathSrc); //扩展名
				
	//1.1 中间文件路径
	CString filePathTemp = csFileNameNoExt + "_new.tempFileYYDS";

	//1.2 如果是特殊的扩展名，如matlab文件，修改为文本文件后处理
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

	//2. 拷贝文件到中间文件
	//2.1 构造命令
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

	//2.2 执行命令
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
		
	//3. 重命名为目标文件	
	//3.1 如果目标文件已存在，先shanchu
	BOOL bFile = PathFileExists(filePathDest);
	if (bFile)
	{
		DeleteFile(filePathDest);
	}

	//3.2 文件重命名
	//::Sleep(1000);
	//CFile::Rename(filePathTemp, filePathDest);
	MoveFile(filePathTemp, filePathDest);

	//复制过的文件，用完了删除
	if (bBack)
	{
		DeleteFile(newFilePath);
	}
	printf("转换文件 %s --> %s \n", filePathSrc.GetString(), filePathDest.GetString());
}

//************************************
// Method:    DecodeFile 转换一个文件
// FullName:  DecodeFile
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CString filePath 入参，文件全路径。D:\\download\\hr\\softreqhhh202102010201.docx
//************************************
void DecodeFile(CString filePath)
{
	//1. 获取文件路径相关信息
	CString csFileFullName = GetFileFullName(filePath);
	CString csFileName = GetFileName(filePath);
	CString csFileExtName = GetFileExtName(csFileFullName);

	//2. 获取目标文件名
	//2.2 目标文件路径
	CString csFileDestPath = csFileName + "_new." + csFileExtName;

	//3.进行转换
	DoDecodeFile(filePath, csFileDestPath);		
}


//************************************
// Method:    DoDecodeDirectory 转换一个目录
// FullName:  DoDecodeDirectory
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CString dirSrcPath 入参，源目录
// Parameter: CString dirDestPath 入参，目的目录
//************************************
void DoDecodeDirectory(CString dirSrcPath, CString dirDestPath)
{
	//1. 找到所有待转换文件
	vector<CString> csFileSrcList;
	FindFileInDir(dirSrcPath, csFileSrcList);

	//2. 转换为目标文件路径,并进行转换
	//2.1 构造目标文件路径	
	for (unsigned int i = 0; i < csFileSrcList.size(); ++i)
	{
		CString srcFilePath = csFileSrcList.at(i);
		CString dstFilePath = srcFilePath;				
		dstFilePath.Replace(dirSrcPath, dirDestPath);
		
		//2.2 递归创建文件夹		
		BOOL bSucc = CreateMultiDirectory(dstFilePath);
		if (!bSucc)
		{
			continue;
		}

		//3. 进行文件转换
		DoDecodeFile(srcFilePath, dstFilePath);
	}
}

//************************************
// Method:    DecodeDirectory 对一个文件夹进行转换
// FullName:  DecodeDirectory
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CString dirPath 入参，文件夹目录
//************************************
void DecodeDirectory(CString dirPath)
{
	//1. 获取文件路径相关信息
	CString csDirName = GetFileFullName(dirPath);	
	CString csDirNameNew = dirPath + "_new";
	
	//2. 进行转换
	DoDecodeDirectory(dirPath, csDirNameNew);	
}


int main(int argc, char* argv[])
{
	//1. 获取输入文件或文件夹路径
	printf("确认亿赛通已登陆.\n");
	printf("请输入文件或文件夹路径,中间无空格.\n");
	string strFileDirIn;
	getline(cin, strFileDirIn);	
	//cout << strFileDirIn << endl;

	//2. 判断输入类型
	CString strPath = strFileDirIn.c_str();
	BOOL bFile = PathFileExists(strPath);
	if (!bFile)
	{
		printf("文件或目录 %s 不存在! \n", strPath.GetString());
		system("pause");
		return 0;
	}
	BOOL bDir = GetFileAttributes(strPath) & FILE_ATTRIBUTE_DIRECTORY;
	if (bDir)
	{
		//文件夹
		DecodeDirectory(strPath);
	}
	else
	{
		//文件
		DecodeFile(strPath);
	}

	// 等待，最后退出
	system("pause");
    return 0;
}


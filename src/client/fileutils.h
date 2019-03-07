#ifndef PAKKI_FILESYS
#define PAKKI_FILESYS

#include "utils.h"
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <ctime>
#if __linux__
#include <unistd.h>
#elif defined (_WIN32)
#include <windows.h>
#include <tchar.h>
#endif

typedef struct 
{
	time_t fileTime;
} FileHandle;

u8 get_filehandle(char *path,FileHandle* fileHandle)  
{
	struct stat attr;
	stat(path, &attr);
	if (-1 == stat(path, &attr)){
		return false;
	}
	fileHandle->fileTime = attr.st_mtime;
	return true;
}

u8 compare_file_times(FileHandle rhv,FileHandle lhv)
{
	return difftime(rhv.fileTime,lhv.fileTime) == 0;
}

u8 does_file_exist(const char* path)
{
#if __linux__
	return access( path, F_OK ) != -1;
#elif defined(_WIN32)
	DWORD dwAttrib = GetFileAttributes(path);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
			!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
	not implemented
#endif
}

static void* internal_load_file(const char* path,const char* filetype,
		size_t* fileSize)
{
	FILE* fp = fopen(path,filetype);
	if(fp == NULL ) return NULL;
	fseek(fp, 0, SEEK_END);
	size_t len = ftell(fp);
	rewind(fp);
	//fseek(fp, 0, SEEK_SET);
	void* ptrToMem = 0;
	ptrToMem = malloc(len + 1);
	if(!ptrToMem) return NULL;
	size_t readsize = fread(ptrToMem,1,len,fp);
	if(readsize != len) {
		// something went wrong
		free(ptrToMem);
		ptrToMem = NULL;
		len = 0;
	}
	if(fileSize) *fileSize = len;
	fclose(fp);
	return ptrToMem;
}

static inline void* load_binary_file(char* const path, size_t* fileSize)
{
	return internal_load_file(path,"rb", fileSize);
}

static inline char* load_file(const char* path, size_t* fileSize)
{
	size_t  size;
	char* data = (char*)internal_load_file(path,"rb", &size);
	if(data) {
		data[size]  = '\0';
		if(fileSize) *fileSize = size;
	}
	return data; 
}

#endif //PAKKI_FILESYS

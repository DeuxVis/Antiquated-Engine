#ifndef ANTIQUATED_ASYNC_FILE_H
#define ANTIQUATED_ASYNC_FILE_H


typedef	void(*fnAsyncFileCallback)( const char* szFilename, BYTE* pbMem, int nMemSize, void* pUserObj );

//--------------------------------------------------
//   Allocates a block of memory and loads a file into it asynchronously
//
//  Callback triggered on load complete with the filename, the allocated memory and size.
//  The memory is freed immediately after the callback is triggered
//  If the file could not be opened, the callback is triggered with NULL for the pbMem pointer and nMemSize = -1
//  If the memory could not be allocated, the callback is triggered with NULL for the pbMem pointer and nMemSize = -2
//  Simples
// 
// Use in code like:
// 
//		AsyncFile	fileLoader(szFilename, AsyncLoadCallbackStatic, (void*)pInfo);
//						// (fileLoader can then basically be forgotten about.. The callback will happen eventually)
// 
// Similar for saves:
//
//		AsyncFileSave	fileSave(szFilename, AsyncSaveCallbackStatic, pbMem, nMemSize, (void*)pInfo);
//						// (fileSave can then basically be forgotten about.. The callback will happen eventually)
// 
//----------------------------------------------------
class AsyncFile
{
public:
	AsyncFile( const char* szFilename, fnAsyncFileCallback loadCompleteCallback, void* pUserObj = NULL );

};

class AsyncFileSave
{
public:
	AsyncFileSave( const char* szFilename, fnAsyncFileCallback saveCompleteCallback, BYTE* pbMem, int nMemSize, void* pUserObj = NULL );

};

typedef	void(*fnAsyncFileSelectorCallback)( int error, const char* szFilename );

extern BOOL	SysGetOpenFilenameDialogAsync( const char* szFileFilter, const char* szTitle, const char* szDefaultFolder, int nFlags, fnAsyncFileSelectorCallback fnCallback );
extern BOOL	SysGetSaveFilenameDialogAsync( const char* szFileFilter, const char* szTitle, const char* szDefaultFolder, int nFlags, fnAsyncFileSelectorCallback fnCallback );
extern BOOL	SysSelectFolderDialogAsync( const char* szTitle, const char* szRootFolder, const char* szDefaultFolder, int nFlags, fnAsyncFileSelectorCallback fnCallback );


extern void	AsyncCallbacksUpdate();
extern void AsyncSysShutdown();

#endif
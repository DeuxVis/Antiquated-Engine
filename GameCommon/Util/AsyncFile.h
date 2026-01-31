#ifndef ANTIQUATED_ASYNC_FILE_H
#define ANTIQUATED_ASYNC_FILE_H


//--------------------------------------------------
// 
// 
//   Allocates a block of memory and loads a file into it asynchronously
//
//  Callback triggered on load complete with the filename, the allocated memory and size.
//  The memory is freed immediately after the callback is triggered
//  If the file could not be opened, the callback is triggered with NULL for the pbMem pointer and nMemSize = -1
//  If the memory could not be allocated, the callback is triggered with NULL for the pbMem pointer and nMemSize = -2
//  Simples
//----------------------------------------------------
typedef	void(*fnAsyncFileLoadCallback)( const char* szFilename, BYTE* pbMem, int nMemSize, void* pUserObj );

class AsyncFile
{
public:
	AsyncFile( const char* szFilename, fnAsyncFileLoadCallback loadCompleteCallback, void* pUserObj = NULL );

};


typedef	void(*fnAsyncFileSelectorCallback)( int error, const char* szFilename );

extern BOOL	SysGetOpenFilenameDialogAsync( const char* szFileFilter, const char* szTitle, const char* szDefaultFolder, int nFlags, fnAsyncFileSelectorCallback fnCallback );
extern BOOL	SysGetSaveFilenameDialogAsync( const char* szFileFilter, const char* szTitle, const char* szDefaultFolder, int nFlags, fnAsyncFileSelectorCallback fnCallback );
extern BOOL	SysSelectFolderDialogAsync( const char* szTitle, const char* szDefaultFolder, int nFlags, fnAsyncFileSelectorCallback fnCallback );


extern void	AsyncCallbacksUpdate();
extern void AsyncSysShutdown();

#endif
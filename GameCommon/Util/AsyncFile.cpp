
#include <stdwininclude.h>
#include "StandardDef.h"
#include <vector>
#include <string>
#include "AsyncFile.h"

class AsyncMutex
{
public:
	void		LockMutex() 
	{
		while( mboMutex )
		{
			Sleep(2);
			// TODO - Drop out if this doesnt return after a bit
		}	
		mboMutex = TRUE;
	}
	
	void		UnlockMutex()
	{
		mboMutex = FALSE;
	}
private:
	volatile BOOL	mboMutex = FALSE;
};

struct AsyncFilePendingCallbackResponse
{
	AsyncFilePendingCallbackResponse( fnAsyncFileLoadCallback callback, const char* szFileName, BYTE* pbMem, int nMemSize )
	{
		mLoadCallback = callback;
		mFilename = szFileName;
		mpbMem = pbMem;
		mnMemSize = nMemSize;
	}

	fnAsyncFileLoadCallback		mLoadCallback;
	std::string		mFilename;
	BYTE*			mpbMem;
	int				mnMemSize;
};

class AsyncFileQueueInstance
{
public:
	AsyncFileQueueInstance( const char* szFilename, fnAsyncFileLoadCallback loadCallback )
	{
		mFileName = szFilename;
		mLoadCallback = loadCallback;
	}
	std::string					mFileName;
	fnAsyncFileLoadCallback		mLoadCallback;
};


class AsyncFileManager
{
public:
	static	AsyncFileManager&		Get();
	~AsyncFileManager();

	void	Add( const char* szFilename, fnAsyncFileLoadCallback loadCallback );

	void	LoadNext();
	void	Shutdown();

	void	UpdatePendingCallbacks();

	std::vector<AsyncFileQueueInstance>&		GetLoadQueue() { return( mFileLoadQueue ); };
private:

	AsyncMutex		mLoadQueueMutex;
	std::vector<AsyncFileQueueInstance>		mFileLoadQueue;
	AsyncMutex		mCallbackQueueMutex;
	std::vector<AsyncFilePendingCallbackResponse>		mPendingLoadCallbacks;

	HANDLE mhFileLoadThread = 0;
};

BOOL	msboAsyncFileLoaderKillThread = FALSE;

AsyncFileManager&	AsyncFileManager::Get()
{
static AsyncFileManager		msSingleton;

	return( msSingleton );
}

void	AsyncFileManager::Shutdown()
{
	if ( mhFileLoadThread != 0 )
	{
		msboAsyncFileLoaderKillThread = TRUE;
		Sleep(10);
		CloseHandle( mhFileLoadThread );
		mhFileLoadThread = 0;
	}
}

AsyncFileManager::~AsyncFileManager()
{
	Shutdown();
}

void	AsyncFileManager::LoadNext()
{
	if ( !mFileLoadQueue.empty() )
	{
		mLoadQueueMutex.LockMutex();
		AsyncFileQueueInstance	instance = mFileLoadQueue[0];
		mFileLoadQueue.erase(mFileLoadQueue.begin());
		mLoadQueueMutex.UnlockMutex();

		FILE*		pFile = SysFileOpen( instance.mFileName.c_str(), "rb");
		if ( pFile )
		{
		int		nMemSize = SysGetFileSize( pFile ); 
		BYTE*		pbMem = (BYTE*)( SystemMalloc( nMemSize ) );
			if ( pbMem )
			{
				SysFileRead( pbMem, nMemSize, 1, pFile );
				SysFileClose( pFile );

				AsyncFilePendingCallbackResponse	response( instance.mLoadCallback, instance.mFileName.c_str(), pbMem, nMemSize);
				mCallbackQueueMutex.LockMutex();
				mPendingLoadCallbacks.push_back( response );
				mCallbackQueueMutex.UnlockMutex();
			}
			else
			{
				AsyncFilePendingCallbackResponse	response( instance.mLoadCallback, instance.mFileName.c_str(), NULL, -2 );
				mCallbackQueueMutex.LockMutex();
				mPendingLoadCallbacks.push_back( response );
				mCallbackQueueMutex.UnlockMutex();
			}
		}
		else
		{
			AsyncFilePendingCallbackResponse	response( instance.mLoadCallback, instance.mFileName.c_str(), NULL, -1 );
			mCallbackQueueMutex.LockMutex();
			mPendingLoadCallbacks.push_back( response );
			mCallbackQueueMutex.UnlockMutex();
		}
	}

}

long WINAPI AsyncFileLoadThread(long lParam)
{ 
	while( !msboAsyncFileLoaderKillThread )
	{
		AsyncFileManager::Get().LoadNext();
		Sleep(5);
	}
	return( 0 );
}

void		AsyncFileManager::Add( const char* szFilename, fnAsyncFileLoadCallback loadCallback )
{
	mLoadQueueMutex.LockMutex();
	AsyncFileQueueInstance		instance( szFilename, loadCallback );
	mFileLoadQueue.push_back( instance );
	mLoadQueueMutex.UnlockMutex();

	if ( mhFileLoadThread == 0 )
	{
	uint32 iID;

		mhFileLoadThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AsyncFileLoadThread,0,0,(LPDWORD)&iID);

	}
}

void	AsyncFileManager::UpdatePendingCallbacks()
{
	if ( !mPendingLoadCallbacks.empty() )
	{
		mCallbackQueueMutex.LockMutex();
		for( AsyncFilePendingCallbackResponse callback : mPendingLoadCallbacks )
		{
			callback.mLoadCallback( callback.mFilename.c_str(), callback.mpbMem, callback.mnMemSize );				
			if ( callback.mpbMem )
			{
				SystemFree( callback.mpbMem );
			}
		}
		mPendingLoadCallbacks.clear();
		mCallbackQueueMutex.UnlockMutex();
	}
}

//--------------------------------

AsyncFile::AsyncFile( const char* szFilename, fnAsyncFileLoadCallback loadCallback )
{
	AsyncFileManager::Get().Add( szFilename, loadCallback );
}


//-----------------------------------------
class AsyncFileSelector
{
public:
	static	AsyncFileSelector&		Get();
	~AsyncFileSelector();

	BOOL	Add( int mode, const char* szFileFilter, const char* szTitle, const char* szDefaultFolder, int nFlags, fnAsyncFileSelectorCallback fnCallback );

	void	ThreadFunc();
	void	UpdatePendingCallbacks();
	void	Shutdown();

private:

	HANDLE mhFileSelectorThread = 0;
	std::string		mFileFilter;
	std::string		mDialogTitle;
	std::string		mDefaultFolder;
	int				mFlags;
	int				mMode;
	fnAsyncFileSelectorCallback		mfnCallback = NULL;
	
	std::string		mPendingResultFilename;
	int				mPendingResultErrorCode;
	BOOL			mbCallbackPending = FALSE;
};

BOOL		msboAsyncFileSelectorKillThread = FALSE;

AsyncFileSelector&		AsyncFileSelector::Get()
{
static AsyncFileSelector	mSingleton;
	return( mSingleton );
}

void	AsyncFileSelector::Shutdown()
{
	if ( mhFileSelectorThread != 0 )
	{
		msboAsyncFileSelectorKillThread = TRUE;
		Sleep(10);
		CloseHandle( mhFileSelectorThread );
		mhFileSelectorThread = 0;
	}
}

AsyncFileSelector::~AsyncFileSelector()
{
	Shutdown();
}
void	AsyncFileSelector::ThreadFunc()
{
	while( !msboAsyncFileSelectorKillThread )
	{
		if ( mfnCallback )
		{
		char	acSelectedFile[1024];
		BOOL	bRet;
			acSelectedFile[0] = 0;

			if ( mMode == 0 )
			{
				bRet = SysGetOpenFilenameDialog( mFileFilter.c_str(), mDialogTitle.c_str(), mDefaultFolder.c_str(), mFlags, acSelectedFile );
			}
			else
			{
				bRet = SysGetSaveFilenameDialog( mFileFilter.c_str(), mDialogTitle.c_str(), mDefaultFolder.c_str(), mFlags, acSelectedFile );
			}

			mPendingResultFilename = acSelectedFile;

			if ( bRet == TRUE )
			{
				mPendingResultErrorCode = 0;
			}
			else
			{
				mPendingResultErrorCode = -1;
			}
			mbCallbackPending = TRUE;

			while( mbCallbackPending )
			{
				Sleep( 5 );
			}
			mfnCallback = NULL;
		}
		Sleep( 5 );
	}
}

void	AsyncFileSelector::UpdatePendingCallbacks()
{
	if ( mbCallbackPending )
	{
 		mfnCallback( mPendingResultErrorCode, mPendingResultFilename.c_str() );			
		mbCallbackPending = FALSE;
	}
}

long WINAPI AsyncFileSelectorThread(long lParam)
{ 
	AsyncFileSelector::Get().ThreadFunc();
	return( 0 );
}
BOOL	AsyncFileSelector::Add( int mode, const char* szFileFilter, const char* szTitle, const char* szDefaultFolder, int nFlags, fnAsyncFileSelectorCallback fnCallback )
{
	// Only allow 1 file dialog open at a time
	if ( !mfnCallback )
	{
		mFileFilter = szFileFilter;
		mDialogTitle = szTitle;
		mDefaultFolder = szDefaultFolder;
		mFlags = nFlags;
		mMode = mode;
		if ( mhFileSelectorThread == 0 )
		{
		uint32 iID;
			mhFileSelectorThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AsyncFileSelectorThread,0,0,(LPDWORD)&iID);
		}
		mfnCallback = fnCallback;
		return( TRUE );
	}
	return( FALSE );
}


BOOL	SysGetOpenFilenameDialogAsync( const char* szFileFilter, const char* szTitle, const char* szDefaultFolder, int nFlags, fnAsyncFileSelectorCallback fnCallback )
{
char		acFullPathToDefaultFolder[512];

	SysGetCurrentDir( 256, acFullPathToDefaultFolder );
	strcat( acFullPathToDefaultFolder, "\\");
	strcat( acFullPathToDefaultFolder, szDefaultFolder );

	return( AsyncFileSelector::Get().Add( 0, szFileFilter, szTitle, acFullPathToDefaultFolder, nFlags, fnCallback ) );
}

BOOL	SysGetSaveFilenameDialogAsync( const char* szFileFilter, const char* szTitle, const char* szDefaultFolder, int nFlags, fnAsyncFileSelectorCallback fnCallback )
{
char		acFullPathToDefaultFolder[512];

	SysGetCurrentDir( 256, acFullPathToDefaultFolder );
	strcat( acFullPathToDefaultFolder, "\\");
	strcat( acFullPathToDefaultFolder, szDefaultFolder );

	return( AsyncFileSelector::Get().Add( 1, szFileFilter, szTitle, acFullPathToDefaultFolder, nFlags, fnCallback ) );
}

//---------------------------------------

void	AsyncCallbacksUpdate()
{
	AsyncFileSelector::Get().UpdatePendingCallbacks();
	AsyncFileManager::Get().UpdatePendingCallbacks();

}

void AsyncSysShutdown()
{
	AsyncFileSelector::Get().Shutdown();
	AsyncFileManager::Get().Shutdown();
}

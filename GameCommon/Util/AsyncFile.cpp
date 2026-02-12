
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

class AsyncFileQueueInstance
{
public:
	AsyncFileQueueInstance() {}

	AsyncFileQueueInstance( const char* szFilename, fnAsyncFileLoadCallback loadCallback, void* pUserParam )
	{
		mFileName = szFilename;
		mLoadCallback = loadCallback;
		mpUserParam = pUserParam;
	}
	std::string					mFileName;
	void*						mpUserParam;
	fnAsyncFileLoadCallback		mLoadCallback;
};


struct AsyncFilePendingCallbackResponse
{
	AsyncFilePendingCallbackResponse( AsyncFileQueueInstance queueInstance, BYTE* pbMem, int nMemSize )
	{
		mQueueInstance = queueInstance;
		mpbMem = pbMem;
		mnMemSize = nMemSize;
	}

	AsyncFileQueueInstance		mQueueInstance;
	BYTE*			mpbMem;
	int				mnMemSize;
};



class AsyncFileManager
{
public:
	static	AsyncFileManager&		Get();
	~AsyncFileManager();

	void	Add( const char* szFilename, fnAsyncFileLoadCallback loadCallback, void* pUserParam );

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

				AsyncFilePendingCallbackResponse	response( instance, pbMem, nMemSize);
				mCallbackQueueMutex.LockMutex();
				mPendingLoadCallbacks.push_back( response );
				mCallbackQueueMutex.UnlockMutex();
			}
			else
			{
				AsyncFilePendingCallbackResponse	response( instance, NULL, -2 );
				mCallbackQueueMutex.LockMutex();
				mPendingLoadCallbacks.push_back( response );
				mCallbackQueueMutex.UnlockMutex();
			}
		}
		else
		{
			AsyncFilePendingCallbackResponse	response( instance, NULL, -1 );
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

void		AsyncFileManager::Add( const char* szFilename, fnAsyncFileLoadCallback loadCallback, void* pUserParam )
{
	mLoadQueueMutex.LockMutex();
	AsyncFileQueueInstance		instance( szFilename, loadCallback, pUserParam );
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
			callback.mQueueInstance.mLoadCallback( callback.mQueueInstance.mFileName.c_str(), callback.mpbMem, callback.mnMemSize, callback.mQueueInstance.mpUserParam );				
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

AsyncFile::AsyncFile( const char* szFilename, fnAsyncFileLoadCallback loadCallback, void* pUserObj )
{
	AsyncFileManager::Get().Add( szFilename, loadCallback, pUserObj );
}


//-----------------------------------------
class AsyncFileSelector
{
public:
	static	AsyncFileSelector&		Get();
	~AsyncFileSelector();

	BOOL	Add( int mode, const char* szFileFilter, const char* szTitle, const char* szDefaultFolder, int nFlags, fnAsyncFileSelectorCallback fnCallback, const char* szRootFolder = NULL );

	void	ThreadFunc();
	void	UpdatePendingCallbacks();
	void	Shutdown();

private:

	HANDLE mhFileSelectorThread = 0;
	std::string		mFileFilter;
	std::string		mDialogTitle;
	std::string		mDefaultFolder;
	std::string		mRootFolder;
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
		char	szFileFilter[1024];
		char*	pcRunner = szFileFilter;

			if ( mMode != 2 )
			{
				strcpy( szFileFilter, mFileFilter.c_str() );
				while( *pcRunner != 0 )
				{
					if ( *pcRunner == '\n')
					{
						*pcRunner = 0;
					}
					pcRunner++;
				}
			}
			acSelectedFile[0] = 0;

			switch( mMode )
			{
			case 0:
				bRet = SysGetOpenFilenameDialog( szFileFilter, mDialogTitle.c_str(), mDefaultFolder.c_str(), mFlags, acSelectedFile );
				break;
			case 1:
				bRet = SysGetSaveFilenameDialog( szFileFilter, mDialogTitle.c_str(), mDefaultFolder.c_str(), mFlags, acSelectedFile );
				break;
			case 2:
				bRet = SysBrowseForFolderDialog( mDialogTitle.c_str(), mRootFolder.c_str(), acSelectedFile, mDefaultFolder.c_str() );
				break;
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

BOOL	AsyncFileSelector::Add( int mode, const char* szFileFilter, const char* szTitle, const char* szDefaultFolder, int nFlags, fnAsyncFileSelectorCallback fnCallback, const char* szRootFolder )
{
	// Only allow 1 file dialog open at a time
	if ( !mfnCallback )
	{
		if ( szFileFilter )
		{
			mFileFilter = szFileFilter;
		}
		else
		{
			mFileFilter = "";
		}
		mDialogTitle = szTitle;
		mDefaultFolder = szDefaultFolder;
		if ( szRootFolder == NULL )
		{
		char		acCurrentDir[MAX_PATH];	
			SysGetCurrentDir( MAX_PATH, acCurrentDir );		
			mRootFolder = acCurrentDir;
		}
		else
		{
			mRootFolder = szRootFolder;
		}
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

BOOL	SysSelectFolderDialogAsync( const char* szTitle, const char* szRootFolder, const char* szDefaultFolder, int nFlags, fnAsyncFileSelectorCallback fnCallback )
{
char		acFullPathToDefaultFolder[512];

	// HACK - Detect absolute path 
	if (szDefaultFolder[1] != ':')
	{
		SysGetCurrentDir(256, acFullPathToDefaultFolder);
		strcat(acFullPathToDefaultFolder, "\\");
		strcat(acFullPathToDefaultFolder, szDefaultFolder);
	}
	else
	{
		strcpy(acFullPathToDefaultFolder, szDefaultFolder);
	}

	return( AsyncFileSelector::Get().Add( 2, NULL, szTitle, acFullPathToDefaultFolder, nFlags, fnCallback, szRootFolder ) );

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

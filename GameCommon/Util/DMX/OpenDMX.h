#ifndef OPENDMX_H
#define OPENDMX_H

//required for DLL loading and some data types
#include <windows.h>

//FTDI library
#include "FTD2XX.H"

//class OpenDMX based on C# code
class OpenDMX
{
public:
	OpenDMX(bool isVerbose);
	
	void		Shutdown();
	
	bool start();
	void printErrorCode( int res ); 
	void setDMXValue(int channel, unsigned char value);
	void zerosDMXValue();
	void writeData();
	int write(); 
	bool initOpenDMX();

	void	GetDeviceInfo();


private:

	static long	ReadingProcStatic(long);
	long	ReadingProc(long);

	BYTE buffer [513];
	int bufferLength; 
	FT_HANDLE m_FThandle; 
	bool done; 
	bool connected; 
	DWORD bytesWritten;
	FT_STATUS status;
	HINSTANCE m_dllHandle;

	unsigned char bits_8; 
	unsigned char stop_bits_2; 
	unsigned char parity_none; 
	unsigned short flow_none; 
	unsigned long purge_rx; 
	unsigned long purge_tx; 
	bool verbose; //controls output. set to true for debugging
};

#endif
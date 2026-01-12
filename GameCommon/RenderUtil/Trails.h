#ifndef GAMECOMMON_RENDERUTIL_TRAILS_H
#define GAMECOMMON_RENDERUTIL_TRAILS_H


typedef int		TRAIL_HANDLE;

extern TRAIL_HANDLE		TrailCreate( int nMode, const VECT* pxStartPoint, int nFadeTimeMS, float fBandScale, float fAlpha );

extern void		TrailUpdate( TRAIL_HANDLE hHandle, const VECT* pxPos, BOOL bVisible );
extern void		TrailUpdateEx( TRAIL_HANDLE hHandle, const VECT* pxPos, BOOL bVisible, uint32 ulUpdateIntervalMS );

extern void		TrailSetScale( TRAIL_HANDLE hHandle, float fBandScale );
extern void		TrailSetTint( TRAIL_HANDLE hHandle, uint32 ulCol );
extern void		TrailSetAlpha( TRAIL_HANDLE hHandle, float fAlpha );
extern void		TrailDelete( TRAIL_HANDLE hHandle, BOOL bDeleteImmediately );

//--------------------------------------------------------------

extern void		TrailsInitialise( void );

extern void		TrailsUpdateAll( float fDelta );

extern void		TrailsRenderAll( void );

extern void		TrailsShutdown( void );





#endif

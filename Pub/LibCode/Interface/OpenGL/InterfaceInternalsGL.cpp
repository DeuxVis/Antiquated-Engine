
#include <windows.h>			// For OpenGL
#include <stdio.h>

#include "InterfaceInternalsGL.h"
#include <gl/gl.h>

#include <StandardDef.h>
#include <Interface.h>

#include "Font/Font.h"
#include "TextureLoader/InterfaceTextureManager.h"
#include "../Common/Overlays/TexturedOverlays.h"
#include "../Common/InterfaceUtil.h"


static HDC     ms_InterfaceDC   = NULL;
static HGLRC   ms_InterfaceGLRenderContext = NULL;

float				mfInterfaceFPS = 0.0f;


int		mnInterfaceDrawX = 0;
int		mnInterfaceDrawY = 0;

/***************************************************************************
 * Function    : InterfaceGetFPS
 * Params      :
 * Returns     :
 * Description : 
 ***************************************************************************/
INTERFACE_API float	InterfaceGetFPS( void )
{
	return( mfInterfaceFPS );
}

INTERFACE_API void	InterfaceSetGlobalParam( INTF_DRAW_PARAM nParam, int nState )
{
	switch( nParam )
	{
	case INTF_LINES_ALPHA:
// todo		if ( nState == 0 ) mboRenderLineAlpha = FALSE; else mboRenderLineAlpha = TRUE;
		break;
	case INTF_TEXTURE_FILTERING:
// todo		if ( nState == 0 ) InterfaceTurnOffTextureFiltering(); else InterfaceTurnOnTextureFiltering(nState);
		break;
	case INTF_ANISOTROPIC:
// todo		if ( nState == 0 ) mboAnistropic = FALSE; else mboAnistropic = TRUE;	
		break;
	case INTF_TEXTURERECT_LIMIT:
		InterfaceTexturedOverlaysSetLimit( nState );
		break;
	}
}

//---------------------------------------------------------------------------
// Function    : InterfaceGLSetupPixelFormat
//---------------------------------------------------------------------------
void		InterfaceGLSetupPixelFormat( HDC hDC )
{
   unsigned char pixelType = PFD_TYPE_RGBA;
	int      s_bpp   = 32;

   PIXELFORMATDESCRIPTOR pfd = 
   {
      sizeof( PIXELFORMATDESCRIPTOR ),
      1,
      PFD_DRAW_TO_WINDOW |	PFD_SUPPORT_OPENGL |	PFD_DOUBLEBUFFER,
      pixelType,
      s_bpp,
      0, 0, 0, 0, 0, 0,
      0,
      0,
      0,
      0, 0, 0, 0,
      16,                 // 16-bit depth buffer
      0,                  // no stencil buffer
      0,                  // no aux buffers
      PFD_MAIN_PLANE,			// main layer 
      0,	
      0, 0, 0
   };
   int  selected_pf;
   
   if ( ( selected_pf = ChoosePixelFormat( hDC, &pfd ) ) == 0 )
   {
      MessageBox( 0, "Failed to find acceptable pixel format", "GL Sphere Error", MB_ICONERROR | MB_OK );
   }
   
   if ( !SetPixelFormat( hDC, selected_pf, &pfd) )
   {
      MessageBox( 0, "Failed to SetPixelFormat", "GL Sphere Error", MB_ICONERROR | MB_OK );
   }
}

//---------------------------------------------------------------------------
// Function    : InterfaceInitDisplayDevice
//---------------------------------------------------------------------------
INTERFACE_API void	InterfaceInitDisplayDevice( BOOL boMinRenderPageSize )
{
HWND	hWnd = InterfaceGetWindow();

	ms_InterfaceDC = GetDC( hWnd );
/*
	s_hDC = GetDC( hWnd );

      if ( !SphereSetupPixelFormat( s_hDC ) )
         exit( 1 );
      SphereSetupPalette( s_hDC );
      s_hglrc = wglCreateContext( s_hDC );
      wglMakeCurrent( s_hDC, s_hglrc );
*/
	InterfaceGLSetupPixelFormat( ms_InterfaceDC );

	ms_InterfaceGLRenderContext = wglCreateContext( ms_InterfaceDC );
	wglMakeCurrent( ms_InterfaceDC, ms_InterfaceGLRenderContext );

//	glViewport( 0,0, InterfaceGetWidth(), InterfaceGetHeight() );

	int		sizeX = InterfaceGetWidth();
	int		sizeY = InterfaceGetHeight();
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, sizeX, sizeY, 0, 0, 1);

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

 
}

//---------------------------------------------------------------------------
// Function    : InterfaceInit
//---------------------------------------------------------------------------
INTERFACE_API void InterfaceInit( BOOL bUseDefaultFonts )
{
	glewInit();

	InterfaceTextureManagerInit();

	InitialiseFont();

	InitTexturedOverlays();

}

uint32		mulLastPresentTick = 0;

//---------------------------------------------------------------------------
// Function    : InterfacePresent
//---------------------------------------------------------------------------
INTERFACE_API void InterfacePresent( void )
{
/*
	uint32		ulTick = SysGetTick();
	uint32		ulTickGap = ulTick - mulLastPresentTick;

	char	acString[256];
	sprintf( acString, "Tickgap = %dms", ulTickGap );
	mulLastPresentTick = ulTick;
*/
	SwapBuffers( ms_InterfaceDC );

	InterfaceTextureManagerUpdate();
}


//---------------------------------------------------------------------------
// Function    : InterfaceEndRender
//---------------------------------------------------------------------------
INTERFACE_API void InterfaceEndRender( void)
{

} 


//---------------------------------------------------------------------------
// Function    : InterfaceBeginRender
//---------------------------------------------------------------------------
INTERFACE_API void InterfaceBeginRender( void)
{

 } 



//---------------------------------------------------------------------------
// Function    : InterfaceDraw
//---------------------------------------------------------------------------
INTERFACE_API void InterfaceDraw( void)
{
	int		sizeX = InterfaceGetWidth();
	int		sizeY = InterfaceGetHeight();
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, sizeX, sizeY, 0, 0, 1);

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	DrawTexturedOverlays(0);
//	RenderOverlays( 0 );
	RenderStrings( 0 );

//	DrawBufferedJpegs(0);
	DrawTexturedOverlays(1);
//	RenderOverlays( 1 );
	RenderStrings( 1 );
//	DrawBufferedJpegs(1);

	DrawTexturedOverlays(2);
//	DrawBufferedJpegs(2);
	RenderStrings( 2 );

	ClearStrings();
}



//---------------------------------------------------------------------------
// Function    : InterfaceFree
//---------------------------------------------------------------------------
INTERFACE_API void InterfaceFree( void)
{
HWND	hWnd = InterfaceGetWindow();

	if ( ms_InterfaceGLRenderContext )
	{
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( ms_InterfaceGLRenderContext );
		ms_InterfaceGLRenderContext = NULL;
	}
	ReleaseDC( hWnd, ms_InterfaceDC );
	InterfaceTextureManagerFree();

} 


//---------------------------------------------------------------------------
// Function    : InterfaceNewFrame
//---------------------------------------------------------------------------
INTERFACE_API int	InterfaceNewFrame( uint32 ulClearCol )
{
float	fRed, fGreen, fBlue, fAlpha;

	InterfaceUnpackCol( ulClearCol, &fRed, &fGreen, &fBlue, &fAlpha );
	glClearColor( fRed, fGreen, fBlue, fAlpha );
	glDepthMask( GL_TRUE );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	return( 0 );
}






//---------------------------------------------------------------------------
// Function    : InterfaceGetWidth
//---------------------------------------------------------------------------
INTERFACE_API int InterfaceGetWidth( void )
{
	// Oy wtf
	return( 800 );
	/*
	if ( mnInterfaceDrawWidth != 0 )
	{
		return( mnInterfaceDrawWidth );
	}
	return( mnRenderSurfaceWidth );
	*/
}


//---------------------------------------------------------------------------
// Function    : InterfaceGetHeight
//---------------------------------------------------------------------------
INTERFACE_API int InterfaceGetHeight( void )
{
	// Oy wtf
	return( 600 );
/*
	if ( mnInterfaceDrawHeight != 0 )
	{
		return( mnInterfaceDrawHeight );
	}
	return( mnRenderSurfaceHeight );
*/
}


//---------------------------------------------------------------------------
// Function    : InterfaceIsSmall
//---------------------------------------------------------------------------
INTERFACE_API BOOL InterfaceIsSmall ( void )
{
	return( false );
}


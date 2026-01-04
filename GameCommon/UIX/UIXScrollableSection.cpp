
#include "StandardDef.h"
#include "InterfaceEx.h"
#include "../UI/UI.h"
#include "UIXScrollableSection.h"


void	UIXScrollableSection::Initialise(  )
{
}


	//		extern int		UIScrollablePageGetPosition( int nHandle );
// todo extern void		UIScrollablePageDestroy( int nHandle );
void		UIXScrollableSection::OnMouseWheel( float fOffset )
{
	mScrollPosition += (int)( fOffset );
	if ( mScrollPosition < 0 ) mScrollPosition = 0;
	int nBarH = (int)( mFullBarHeight * mfBarScale );
	if ( mScrollPosition + nBarH > mFullBarHeight ) mScrollPosition = mFullBarHeight - nBarH;

}


UIXRECT		UIXScrollableSection::OnRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
int		childContentsHeight = GetChildContentsHeight();

	if ( childContentsHeight > displayRect.h )
	{
		pInterface->DrawAllElements();
		pInterface->SetRenderCanvas();
		mbIsUsingClippingCanvas = TRUE;

		UIXRECT		localRect = GetDisplayRect();
		UIXRECT		occupyRect = displayRect;
		UIXRECT		drawRect = localRect;

		drawRect.x += displayRect.x;
		drawRect.y += displayRect.y;
				
		if ( UIHoverItem( drawRect.x, drawRect.y, drawRect.w, drawRect.h ) == TRUE )
		{
			UIX::SetMousewheelHoverObject( this );
		}

		mPageRenderRect = drawRect;

		int		nScrollbarBoxW = 12;
		int		nScrollbarBoxH = drawRect.h - 2;
		int		nBarW = nScrollbarBoxW - 4;
		int		nMaxBarH = nScrollbarBoxH - 2;

		int		nScrollbarBoxX = drawRect.x + drawRect.w - 12;
		int		nScrollbarBoxY = drawRect.y + 1;

		float	fBarScale = (float)( displayRect.h ) / (float)( childContentsHeight );
		if ( fBarScale > 1.0f ) fBarScale = 1.0f;
		mfBarScale = fBarScale;
		mFullBarHeight = nMaxBarH;

		pInterface->Rect( 0, nScrollbarBoxX, nScrollbarBoxY, nScrollbarBoxW, nScrollbarBoxH, 0xd0202020 );

		mLastRender.x = nScrollbarBoxX;
		mLastRender.y = nScrollbarBoxY;
		mLastRender.w = nScrollbarBoxW;
		mLastRender.h = nScrollbarBoxH;

		int		nBarX = nScrollbarBoxX + 2;
		int		nBarY = nScrollbarBoxY + 1;

		nBarY += (int)( mScrollPosition );
		int nBarH = (int)( nMaxBarH * fBarScale );

		pInterface->Rect( 1, nBarX, nBarY, nBarW, nBarH, 0xc0505050 );

		if ( UIHoverItem( nBarX, nBarY, nBarW, nBarH ) == TRUE )
		{
		int		nMouseX, nMouseY;
				
			UIGetCurrentCursorPosition( &nMouseX, &nMouseY );
			UIHoverIDSet( UIX_SCROLLABLE_SECTION_SCROLLBAR, 0, GetID() );
			mHoverOffsetY = nMouseY - nBarY;
		}

		occupyRect.h = 0;
		occupyRect.y = 0;
		occupyRect.w -= 16;
		return occupyRect;
	}
	else
	{
		mbIsUsingClippingCanvas = FALSE;
		mScrollPosition = 0;
		return( UIXRECT( displayRect.x, 0, displayRect.w, 0) );
	}
}

void	UIXScrollableSection::OnPostChildrenRender( InterfaceInstance* pInterface )
{
	if ( mbIsUsingClippingCanvas )
	{
		pInterface->DrawAllElements();
		pInterface->CopyRenderCanvasToBackBuffer( mPageRenderRect.x, mPageRenderRect.y, mPageRenderRect.w, mPageRenderRect.h );
	}
}

int			UIXScrollableSection::GetScrollPosition()
{
	if ( mfBarScale > 0.0f )
	{
		return( (int)( mScrollPosition / mfBarScale ) ); 
	}
	return 0;
}

void	UIXScrollableSection::HoldHandler( uint32 ulElementIndex, BOOL bIsHeld, BOOL bFirstPress )
{
int		nMouseX, nMouseY;
				
	UIGetCurrentCursorPosition( &nMouseX, &nMouseY );
	if ( bFirstPress )
	{
		mbDidGrabScrollbar = TRUE;
		mPressPosOffsetY = mHoverOffsetY;
		mPressPosScreenY = nMouseY;
	}
	else if ( bIsHeld ) 
	{
		mScrollPosition = (int)( (nMouseY - mPressPosScreenY) / mfBarScale );
		if ( mScrollPosition < 0 ) mScrollPosition = 0;
		int nBarH = (int)( mFullBarHeight * mfBarScale );
		
		if ( mScrollPosition + nBarH > mFullBarHeight ) mScrollPosition = mFullBarHeight - nBarH;
	}
	else  // Just released
	{
		mbDidGrabScrollbar = FALSE;
	}

}


void	UIXScrollableSection::HoldHandlerStatic( int nButtonID, uint32 ulParam, uint32 ulIDParam, BOOL bIsHeld, BOOL bFirstPress )
{
UIXScrollableSection*		pSection = (UIXScrollableSection*)UIX::GetUIXObjectByID( ulIDParam );

	if ( pSection )
	{
		pSection->HoldHandler( ulParam, bIsHeld, bFirstPress );
	}
}


void		UIXScrollableSection::RegisterControlHandlers()
{
	UIRegisterHoldHandler( UIX_SCROLLABLE_SECTION_SCROLLBAR, HoldHandlerStatic );

}

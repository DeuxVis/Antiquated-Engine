
#include "StandardDef.h"
#include "InterfaceEx.h"
#include "../UI/UI.h"
#include "UIXScrollableSection.h"



void	UIXScrollbar::RenderDirect(InterfaceInstance* pInterface, UIXRECT rect, int nViewPageHeight, int nFullContentsHeight )
{
int		nScrollbarBoxW = rect.w;
int		nScrollbarBoxH = rect.h;
int		nBarW = nScrollbarBoxW - 4;
int		nMaxBarH = nScrollbarBoxH - 2;
int		nScrollbarBoxX = rect.x;
int		nScrollbarBoxY = rect.y;

	float	fBarScale = (float)(nViewPageHeight) / (float)(nFullContentsHeight);
	if (fBarScale > 1.0f) fBarScale = 1.0f;
	mfBarScale = fBarScale;
	mFullBarHeight = nMaxBarH;

	mfHeightPerUnit = (float)rect.h / (float)nFullContentsHeight;
	if (mfHeightPerUnit == 0.0f) mfHeightPerUnit = 1.0f;

	// Background
	pInterface->Rect(1, nScrollbarBoxX, nScrollbarBoxY, nScrollbarBoxW, nScrollbarBoxH, 0xd0202020);

	int		nBarX = nScrollbarBoxX + 2;
	int		nBarY = nScrollbarBoxY + 1;

	nBarY += (int)(mScrollPositionScreen);
	int nBarH = (int)(nMaxBarH * fBarScale);

	// Bar 
	pInterface->Rect(1, nBarX, nBarY, nBarW, nBarH, 0xc0505050);

	if (UIHoverItem(nBarX, nBarY, nBarW, nBarH) == TRUE)
	{
		int		nMouseX, nMouseY;

		UIGetCurrentCursorPosition(&nMouseX, &nMouseY);
		UIHoverIDSet(UIX_SCROLLBAR, 0, GetID());
		mHoverOffsetY = nMouseY - nBarY;
	}

}

BOOL		UIXScrollbar::HoldHandler(uint32 ulElementIndex, BOOL bIsHeld, BOOL bFirstPress)
{
	int		nMouseX, nMouseY;

	UIGetCurrentCursorPosition(&nMouseX, &nMouseY);
	if (bFirstPress)
	{
		mbDidGrabScrollbar = TRUE;
		mPressPosOffsetY = mHoverOffsetY;
		mPressPosScreenY = nMouseY;
		mHoldStartScrollPosScreen = mScrollPositionScreen;
	}
	else if (bIsHeld)
	{
		mHoldMoveDistanceY = (int)((nMouseY - mPressPosScreenY));
		mScrollPositionScreen = mHoldStartScrollPosScreen + mHoldMoveDistanceY;

		if (mScrollPositionScreen < 0) mScrollPositionScreen = 0;

		int nBarH = (int)(mFullBarHeight * mfBarScale);
		if (mScrollPositionScreen + nBarH > mFullBarHeight) mScrollPositionScreen = mFullBarHeight - nBarH;

		mScrollPosition = (int)(mScrollPositionScreen / mfHeightPerUnit);
	}
	else  // Just released
	{
		mbDidGrabScrollbar = FALSE;
	}
	return( FALSE );
}

BOOL		UIXScrollbar::HoldHandlerStatic(int nButtonID, uint32 ulParam, uint32 ulIDParam, BOOL bIsHeld, BOOL bFirstPress)
{
	UIXScrollbar* pScrollbar = (UIXScrollbar*)UIX::FindUIXObjectByID(ulIDParam);

	if (pScrollbar)
	{
		return( pScrollbar->HoldHandler(ulParam, bIsHeld, bFirstPress) );
	}
	return( FALSE );
}

void		UIXScrollbar::RegisterControlHandlers()
{
	UIRegisterHoldHandler(UIX_SCROLLBAR, HoldHandlerStatic);

}

int		UIXScrollbar::GetScrollPosition()
{
	return( mScrollPosition );
}

void	UIXScrollbar::OnMouseWheel(float fOffset)
{
	mScrollPositionScreen += (int)(fOffset);
	if (mScrollPositionScreen < 0) mScrollPositionScreen = 0;

	int nBarH = (int)(mFullBarHeight * mfBarScale);
	if (mScrollPositionScreen + nBarH > mFullBarHeight) mScrollPositionScreen = mFullBarHeight - nBarH;

	if (mfHeightPerUnit > 0.0f)
	{
		mScrollPosition = (int)(mScrollPositionScreen / mfHeightPerUnit);
	}
	
}

void	UIXScrollbar::StoreScrollState( UIXScrollbarRestoreState* pxOut )
{
	pxOut->mfHeightPerUnit = mfHeightPerUnit;
	pxOut->mScrollPosition = mScrollPosition;
	pxOut->mScrollPositionScreen = mScrollPositionScreen;
	pxOut->mFullBarHeight = mFullBarHeight;
	pxOut->mContentsHeight = mChildContentsHeight;
}
	
void	UIXScrollbar::RestoreScrollState( const UIXScrollbarRestoreState* pxIn )
{
	mfHeightPerUnit = pxIn->mfHeightPerUnit;
	mScrollPosition = pxIn->mScrollPosition;
	mScrollPositionScreen = pxIn->mScrollPositionScreen;
	mFullBarHeight = pxIn->mFullBarHeight;
	mChildContentsHeight = pxIn->mContentsHeight;
}

//------------------------------------------------------------------------------------------------
// 
UIXScrollableSection::~UIXScrollableSection()
{
	if ( mpScrollbar )
	{
		UIX::DeleteObject( mpScrollbar );
		mpScrollbar = NULL;
	}
}


void	UIXScrollableSection::Initialise()
{
	mpScrollbar = new UIXScrollbar(this, UIX::GetNextObjectID(), UIXRECT(0, 0, 0, 0));
}


//		extern int		UIScrollablePageGetPosition( int nHandle );
// todo extern void		UIScrollablePageDestroy( int nHandle );
void		UIXScrollableSection::OnMouseWheel(float fOffset)
{
	mpScrollbar->OnMouseWheel(fOffset);
}


UIXRECT		UIXScrollableSection::OnRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
	if ( GetChildContentsHeight() > 0 )
	{
	int		childContentsHeight = GetChildContentsHeight() + 4;

		if ( childContentsHeight > displayRect.h )
		{
			pInterface->DrawAllElements();
			pInterface->SetRenderCanvas();
			mbIsUsingClippingCanvas = TRUE;

			UIXRECT		localRect = GetLocalPositionRect();
			UIXRECT		occupyRect = displayRect;
			UIXRECT		drawRect = localRect;

			drawRect.x += displayRect.x;
			drawRect.y += displayRect.y;
				
			if ( UIHoverItem( drawRect.x, drawRect.y, drawRect.w, drawRect.h ) == TRUE )
			{
				UIX::SetMousewheelHoverObject( this );
			}

			mPageRenderRect = drawRect;

			mScrollbarLastRender.x = drawRect.x + drawRect.w - 12;
			mScrollbarLastRender.y = drawRect.y + 1;;
			mScrollbarLastRender.w = 12;
			mScrollbarLastRender.h = displayRect.h;

			mpScrollbar->RenderDirect(pInterface, mScrollbarLastRender, displayRect.h, childContentsHeight);

			occupyRect.h = 0;
			occupyRect.y = 0;
			occupyRect.w = -16;
			return occupyRect;
		}
		else
		{
			mbIsUsingClippingCanvas = FALSE;
			mpScrollbar->SetScrollPosition(0);
			return( UIXRECT( displayRect.x, 0, displayRect.w, 0) );
		}
	}
	return( UIXRECT( displayRect.x, 0, displayRect.w, 0) );
}

void	UIXScrollableSection::OnPostChildrenRender( InterfaceInstance* pInterface )
{
	if ( mbIsUsingClippingCanvas )
	{
		pInterface->DrawAllElements();
		pInterface->CopyRenderCanvasToBackBuffer( mPageRenderRect.x, mPageRenderRect.y, mPageRenderRect.w, mPageRenderRect.h );
	}
}

void	UIXScrollableSection::StoreScrollState( UIXScrollbarRestoreState* pxOut )
{
	mpScrollbar->StoreScrollState( pxOut );
	pxOut->mContentsHeight = mChildContentsHeight;

}

void	UIXScrollableSection::RestoreScrollState( const UIXScrollbarRestoreState* pxIn )
{
	mpScrollbar->RestoreScrollState( pxIn );
	mChildContentsHeight = pxIn->mContentsHeight;
}

int			UIXScrollableSection::GetScrollPosition()
{
	return( mpScrollbar->GetScrollPosition() );
}


void		UIXScrollableSection::RegisterControlHandlers()
{
	UIXScrollbar::RegisterControlHandlers();

}

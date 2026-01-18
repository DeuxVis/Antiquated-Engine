
#include "StandardDef.h"
#include "InterfaceEx.h"
#include "../UI/UI.h"
#include "UIXCollapsableSection.h"


void	UIXCollapsableSection::Initialise( int mode, const char* szTitle, BOOL bStartCollapsed, int draggableType )
{
	mTitle = szTitle;	
	mbIsCollapsed = bStartCollapsed;
	mMode = mode;
	mDragItemType = draggableType;
}


void	UIXCollapsableSection::ToggleCollapsed()
{
	mbIsCollapsed = !mbIsCollapsed;
}

void	UIXCollapsableSection::OnSelected( int nButtonID, uint32 ulParam )
{
	ToggleCollapsed();
}

void	UIXCollapsableSection::UpdateUIStateData( UIStateData* pData )
{
	pData->mbTabOpened = !mbIsCollapsed;
}


UIXRECT		UIXCollapsableSection::OnRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
UIXRECT		renderRect = GetActualRenderRect( displayRect );
int	X = renderRect.x;
int	Y = renderRect.y;
int	W = renderRect.w;
int	H = renderRect.h;
uint32		ulCol = 0x90303030;
uint32		ulTextCol = 0xd0d0d0d0;
int		headerH = 20;
int		nHeaderOffsetX = 0;
int			nHeaderW = W;
BOOL	bMouseIsOverSectionHeader = FALSE;

	switch( mMode )
	{
	case 1:
		ulCol = 0x90303040;
		nHeaderOffsetX = 20;
		break;
	case 2:
		ulCol = 0x90404040;
		ulTextCol = 0xd0f0c080;
		nHeaderOffsetX = 5;
//		nHeaderW -= 44;
//		headerH = 20;
		break;
	default:
		break;
	}

	mLastRender = UIXRECT( X,Y,nHeaderW, headerH );
	pInterface->Rect( 0, X, Y, nHeaderW, headerH, ulCol );
		
	if ( UIHoverItem( X, Y, nHeaderW, headerH ) == TRUE )
	{
		UIHoverIDSet( UIX_COLLAPSABLE_SECTION_HEADER, 0, GetID() );
		bMouseIsOverSectionHeader = TRUE;
	}

//	UIButtonRegion( UIX_COLLAPSABLE_SECTION_HEADER, X, Y, W, headerH, GetID() );

	if ( CheckDragHoverRegion( UIXRECT( X, Y, nHeaderW, headerH ) ) )
	{
		pInterface->Rect( 0, X, Y + headerH, nHeaderW, 2, 0xd08080c0 );	
		UIX::HoverAcceptDragItem( this );
	}

	pInterface->Text( 1, X + 20 + nHeaderOffsetX, Y + 4, ulTextCol, 3, mTitle.c_str() );

	if ( mbCollapsable )
	{
		if ( !mbIsCollapsed )
		{
			pInterface->Triangle( 1, X + 4 + nHeaderOffsetX, Y + 8, X + 12 + nHeaderOffsetX, Y + 8, X + 8 + nHeaderOffsetX, Y + 16, 0xa0a0a0a0, 0xa0a0a0a0, 0xa0a0a0a0 );	
		}
		else
		{
			pInterface->Triangle( 1, X + 4 + nHeaderOffsetX, Y + 6, X + 10 + nHeaderOffsetX, Y + 10, X + 4 + nHeaderOffsetX, Y + 14, 0xa0a0a0a0, 0xa0a0a0a0, 0xa0a0a0a0 );	
		}
	}

	displayRect.h = headerH + 1;
	displayRect.y = headerH + 1;		// displayRect.y returns the lowest point we drew to

	if ( ( UIX::GetDragItemType() != 0 ) &&
		 ( UIX::GetDragItemType() == mDragItemType ) &&
		 ( UIX::GetDragItemSourceParam() == GetID() ) )
	{
	UIXRECT		xCurrMouse;

		UIGetCurrentCursorPosition( &xCurrMouse.x, &xCurrMouse.y );
		UIXRECT	xGrabOffset( xCurrMouse.x - mDragRectMouseOriginal.x, xCurrMouse.y - mDragRectMouseOriginal.y, 0, 0 );

		// Don't show the drag image if we're still hovering over the original section header
		if ( bMouseIsOverSectionHeader == FALSE )
		{
		UIXRECT xNewRect = mDragRectOriginal;

			xNewRect.x += xGrabOffset.x;
			xNewRect.y += xGrabOffset.y;

			pInterface->Rect( 1, xNewRect.x, xNewRect.y, xNewRect.w, xNewRect.h, 0x40303030 );
			pInterface->Text( 2, xNewRect.x + 5, xNewRect.y + 2, 0x80a0a0a0, 0, mTitle.c_str() );
		}
	
	}
	return displayRect;
}

void	UIXCollapsableSection::HoldHandler( uint32 ulElementIndex, BOOL bIsHeld, BOOL bFirstPress )
{
	if ( bFirstPress )
	{
		mDragRectOriginal = GetLastRenderRect();
		UIX::SetDragItemType( mDragItemType, this, GetID() );
		UIGetCurrentCursorPosition( &mDragRectMouseOriginal.x, &mDragRectMouseOriginal.y );
	}
	else if ( bIsHeld ) 
	{
	
	}
	else  // Just released
	{
	int		mouseX, mouseY;

		UIX::EndDragItemType( mDragItemType );

		// If cursor has remained within the original section rect, treat it as a press (collapse the tab)
		UIGetCurrentCursorPosition( &mouseX, &mouseY );

		UIX::CheckForPress( this, mLastRender, UIX_COLLAPSABLE_SECTION_HEADER, 0 );
	}
}


void	UIXCollapsableSection::HoldHandlerStatic( int nButtonID, uint32 ulParam, uint32 ulIDParam, BOOL bIsHeld, BOOL bFirstPress )
{
UIXCollapsableSection*		pSection = (UIXCollapsableSection*)UIX::GetUIXObjectByID( ulIDParam );

	if ( pSection )
	{
		pSection->HoldHandler( ulParam, bIsHeld, bFirstPress );
	}
}


void		UIXCollapsableSection::RegisterControlHandlers()
{
	UIRegisterHoldHandler( UIX_COLLAPSABLE_SECTION_HEADER, HoldHandlerStatic );

}

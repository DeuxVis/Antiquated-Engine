
#include "StandardDef.h"
#include "InterfaceEx.h"

#include "../Platform/Platform.h"
#include "../UI/UI.h"
#include "UIXButton.h"

UIXButton::~UIXButton()
{
	if (mhImageTexture != NOTFOUND)
	{
		InterfaceReleaseTexture(mhImageTexture);
	}
}

void	UIXButton::EndEdit()
{
	if ( mfnLabelEditCallback )
	{
		mTitle = PlatformKeyboardGetInputString(FALSE);
		PlatformKeyboardSetInputString( "" );
		if ( mfnLabelEditCallback(this, mTitle.c_str() ) == false )
		{
			mTitle = mPreEditTitle;
		}
	}
	UIX::SetTextEditFocus(NULL);
}

int		UIXButton::OnKeyboardMessage( int nResponseCode, const char* szInputText )
{
	switch( nResponseCode )
	{
	case 1:	// press enter
		EndEdit();
		break;
	case 2:		// press TAB

		break;
	}
	return( 0 );
}

int		UIXButton::ButtonEditLabelKeyboardMessageHandlerStatic( int nResponseCode, const char* szInputText, void* pUserObj )
{
	UIXButton*		pButton = (UIXButton*)pUserObj;
	return(pButton->OnKeyboardMessage(nResponseCode, szInputText));
}


void	UIXButton::EnableLabelEdit( fnLabelEditCallback callbackFunc )
{
	mfnLabelEditCallback = callbackFunc; 
	mPreEditTitle = mTitle;
	UIX::SetTextEditFocus( this );
	PlatformKeyboardRegisterHandler( ButtonEditLabelKeyboardMessageHandlerStatic, this );
	PlatformKeyboardActivate( 0, mTitle.c_str(), "" );
}

void	UIXButton::SetDraggable( int nDragItemType, uint32 ulDragParam )
{
	mDragItemType = nDragItemType;
	mDragItemParam = ulDragParam;
}

void	UIXButton::Initialise( eUIXBUTTON_MODE mode, const char* szTitle, uint32 ulButtonID, uint32 ulButtonParam, BOOL bIsBlocking, uint32 ulCol, int iconNum )
{
	if ( szTitle )
	{
		mTitle = szTitle;
	}
	mMode = mode;
	mulButtonID = ulButtonID;
	mulButtonParam = ulButtonParam;
	mbIsBlocking = bIsBlocking;
	mulCol = ulCol;
	mIconNum = iconNum;
	if ( mode == UIXBUTTON_IMAGE )
	{
		mhImageTexture = mIconNum;
	}
}
	
UIXRECT		UIXButton::OnRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
UIXRECT		localRect = GetLocalPositionRect();
UIXRECT		drawRect = GetActualRenderRect( displayRect );

	SetLastRenderRect(drawRect);
	
	if ( UIX::IsMouseHover( drawRect ) == TRUE )
	{
		UIHoverIDSet( UIX_BUTTON, 0, GetID() );
	}
	if ( CheckDragHoverRegion( UIXRECT( drawRect.x, drawRect.y, drawRect.w, drawRect.h ) ) )
	{
		pInterface->OutlineBox( 1, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0x808080c0 );	
		UIX::HoverAcceptDragItem( this );
	}

	if ( mbIsBeingDragged )
	{
	int		nMouseX, nMouseY;
	int		nMoveDist;

		// Check we've moved at least a bit away from the click pos otherwise its not really a drag
		UIGetCurrentCursorPosition( &nMouseX, &nMouseY );
		nMoveDist = abs(mDragRectMouseOriginal.x - nMouseX) + abs(mDragRectMouseOriginal.y - nMouseY );
		if ( nMoveDist > 4 )
		{
		UIXRECT	xGrabOffset( nMouseX - mDragRectMouseOriginal.x, nMouseY - mDragRectMouseOriginal.y, 0, 0 );
		UIXRECT xNewRect = mDragRectOriginal;

			xNewRect.x += xGrabOffset.x;
			xNewRect.y += xGrabOffset.y;
			pInterface->Rect( 2, xNewRect.x, xNewRect.y, xNewRect.w, xNewRect.h, 0x40303030 );
			pInterface->Text( 2, xNewRect.x + 5, xNewRect.y + 2, 0x80a0a0a0, 0, mTitle.c_str() );	
		}
	}

	switch( mMode)
	{
	case UIXBUTTON_NORMAL:
	default:
		{
		eUIBUTTON_MODE_FLAGS		modeFlags = UIBUTTON_FLAG_SMALL_FONT;

			if (mfnLabelEditCallback != NULL)
			{
				modeFlags = (eUIBUTTON_MODE_FLAGS)(modeFlags | UIBUTTON_FLAG_LABEL_EDIT);
			}
	
			UIButtonDraw( mulButtonID, drawRect.x, drawRect.y, drawRect.w, drawRect.h, mTitle.c_str(), modeFlags, mulButtonParam, GetID() );
			if ( mulRightPressButtonID )
			{
				UIX::CheckForRightButtonPress( this, drawRect, mulRightPressButtonID, mulRightPressButtonParam );	
			}
		}
		break;
	case UIXBUTTON_IMAGE_PATH:
		if ( mbHasAttemptedLoadTexture == FALSE )
		{
			// TODO - Switch to async
			mhImageTexture = pInterface->GetTexture( mTitle.c_str(), 0 );
			mbHasAttemptedLoadTexture = TRUE;
		}

		if ( mhImageTexture != NOTFOUND )
		{
			int		nOverlay = pInterface->CreateNewTexturedOverlay( 0, mhImageTexture );
			pInterface->TexturedRect( nOverlay, drawRect.x, drawRect.y, drawRect.w, drawRect.h, mulCol, 0.0f, 0.0f, 1.0f, 1.0f );
		}
		else
		{
			pInterface->Rect( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0x90404040 );
		}

		if ( mulButtonID != 0 )
		{	
			if ( UIX::CheckForPress( this, drawRect, mulButtonID, mulButtonParam ) )
			{		
				pInterface->Rect( 1, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0x20FFFFFF );				
			}
		}
		break;
	case UIXBUTTON_IMAGE:
		if ( mfnDynamicButtonImageHandler != NULL )
		{
		int		hTexture = mfnDynamicButtonImageHandler( this, mpImageHandlerParam );

			if ( hTexture != NOTFOUND )
			{
			int		nOverlay = pInterface->CreateNewTexturedOverlay( 0, hTexture );
				pInterface->TexturedRect( nOverlay, drawRect.x, drawRect.y, drawRect.w, drawRect.h, mulCol, 0.0f, 0.0f, 1.0f, 1.0f );
			}
			else
			{
				pInterface->Rect( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0x90404040 );
			}		
		}
		else if ( mhImageTexture != NOTFOUND )
		{
			int		nOverlay = pInterface->CreateNewTexturedOverlay( 0, mhImageTexture );
			pInterface->TexturedRect( nOverlay, drawRect.x, drawRect.y, drawRect.w, drawRect.h, mulCol, 0.0f, 0.0f, 1.0f, 1.0f );
		}
		else
		{
			pInterface->Rect( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0x90404040 );
		}

		if ( mulButtonID != 0 )
		{	
			if ( UIX::CheckForPress( this, drawRect, mulButtonID, mulButtonParam ) )
			{		
				pInterface->Rect( 1, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0x30FFe0c0 );				
			}
		}
		if ( mulRightPressButtonID )
		{
			UIX::CheckForRightButtonPress( this, drawRect, mulRightPressButtonID, mulRightPressButtonParam );	
		}
		break;
	case UIXBUTTON_ICON:
		{
			UIButtonDraw( mulButtonID, drawRect.x, drawRect.y, drawRect.w, drawRect.h, "", UIBUTTON_FLAG_NONE, mulButtonParam, GetID() );

			UIXRECT	 iconRect = drawRect;

			// TEMP assumes icons are all 19x19
			iconRect.x += ((drawRect.w-19)/2);
			iconRect.y += ((drawRect.h-19)/2);
			iconRect.w = 19;
			iconRect.h = 19;

			UIX::DrawIcon( pInterface, mIconNum, iconRect, 0xE0D0D0D0 );
		}
		break;

	case UIXBUTTON_RECT_ICON:
		pInterface->Rect( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, mulCol );
		if ( mulButtonID != 0 )
		{
			if ( UIX::CheckForPress( this, drawRect, mulButtonID, mulButtonParam ) )
			{		
				pInterface->Rect( 1, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0x20FFFFFF );				
			}
		}
		drawRect.x += ((drawRect.w - 19)/2);
		drawRect.y += ((drawRect.h - 19)/2);
		drawRect.w = 19;
		drawRect.h = 19;
		UIX::DrawIcon( pInterface, mIconNum, drawRect, 0xE0D0D0D0 );
		break;		
	case UIXBUTTON_PLAIN_RECT:
		pInterface->Rect( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, mulCol );

		if ( mulButtonID != 0 )
		{
			if ( UIX::CheckForPress( this, drawRect, mulButtonID, mulButtonParam ) )
			{		
				pInterface->Rect( 1, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0x20FFFFFF );				
			}
		}
		// TODO - Properly centre the title
		pInterface->Text( 1, drawRect.x + 6, drawRect.y + 1, 0xd0a0a0a0, 1, mTitle.c_str() );
		break;
	case UIXBUTTON_TEXT_WITH_COLLAPSABLE:
		{
		int		nFont = 3;
		int		stringHeight = pInterface->GetStringHeight( mTitle.c_str(), nFont );
		
			pInterface->TextRight( 0, drawRect.x + drawRect.w, drawRect.y + ((drawRect.h-stringHeight)/2), 0xD0c0c0d0, 3, mTitle.c_str() );

			if ( UIX::CheckForPress( this, drawRect, mulButtonID, mulButtonParam ) )
			{			
			UIXRECT		cogRect = drawRect;
				cogRect.x += 2;
				cogRect.y += 6;
				cogRect.h -= 4;
				cogRect.w = cogRect.h;
				if ( !mbIsCollapsed )
				{
					pInterface->Triangle( 1, cogRect.x, cogRect.y, cogRect.x + 8, cogRect.y, cogRect.x + 4, cogRect.y + 8, 0xa0a0a0a0, 0xa0a0a0a0, 0xa0a0a0a0 );	
				}
				else
				{
					pInterface->Triangle( 1, cogRect.x, cogRect.y, cogRect.x + 6, cogRect.y + 4, cogRect.x, cogRect.y + 8, 0xa0a0a0a0, 0xa0a0a0a0, 0xa0a0a0a0 );	
				}
			}
		}
		break;
	case UIXBUTTON_TEXT_WITH_SETTINGS:
		{
		int		nFont = 3;
		int		stringHeight = pInterface->GetStringHeight( mTitle.c_str(), nFont );
		
			pInterface->TextRight( 0, drawRect.x + drawRect.w, drawRect.y + ((drawRect.h-stringHeight)/2), 0xD0c0c0d0, 3, mTitle.c_str() );

			if ( UIX::CheckForPress( this, drawRect, mulButtonID, mulButtonParam ) )
			{
			UIXRECT		cogRect = drawRect;

				cogRect.x += 2;
				cogRect.y += 2;
				cogRect.h -= 4;
				cogRect.w = cogRect.h;
				// Draw  
				UIX::DrawIcon( pInterface, 2, cogRect, 0xa0a0a0a0 );
			}
		}

		break;

	}
	displayRect.h = localRect.h + 1;
	displayRect.y = localRect.h + 1; //drawRect.y + drawRect.h + 1;		// displayRect.y returns the lowest point we drew to

	if ( !mbIsBlocking )
	{
		displayRect.h = 0;
	}
	return( displayRect );
}


BOOL		UIXButton::HoldHandler( uint32 ulElementIndex, BOOL bIsHeld, BOOL bFirstPress )
{
	if ( mDragItemType != 0 )
	{
		if ( bFirstPress )
		{
			mbIsBeingDragged = TRUE;
			mDragRectOriginal = GetLastRenderRect();
			UIX::SetDragItemType( mDragItemType, this, mDragItemParam );
			UIGetCurrentCursorPosition( &mDragRectMouseOriginal.x, &mDragRectMouseOriginal.y );
		}
		else if ( bIsHeld ) 
		{
		
		}
		else  // Just released
		{
			if ( ( mbIsBeingDragged ) &&
				 ( UIX::GetDragDestinationHover() != NULL ) &&
				 ( UIX::GetDragDestinationHover() != this ) )
			{
				mbIsBeingDragged = FALSE;
				UIX::EndDragItemType( mDragItemType );
				return( TRUE );
			}
			mbIsBeingDragged = FALSE;
		}
	}
	return( FALSE );
}


BOOL		UIXButton::HoldHandlerStatic( int nButtonID, uint32 ulParam, uint32 ulIDParam, BOOL bIsHeld, BOOL bFirstPress )
{
UIXButton*		pButton = (UIXButton*)UIX::FindUIXObjectByID( ulIDParam );

	if ( pButton )
	{
		return( pButton->HoldHandler( ulParam, bIsHeld, bFirstPress ) );
	}
	return( FALSE );
}

void		UIXButton::RegisterControlHandlers()
{
	UIRegisterHoldHandler( UIX_BUTTON, HoldHandlerStatic );

}
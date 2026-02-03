
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
		UIX::DrawIcon( pInterface, mIconNum, UIXRECT(drawRect.x + 1, drawRect.y + 1, drawRect.w - 2, drawRect.h - 2), 0xE0D0D0D0 );
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


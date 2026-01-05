
#include "StandardDef.h"
#include "InterfaceEx.h"

#include "../UI/UI.h"
#include "UIXButton.h"


void	UIXButton::Initialise( eUIXBUTTON_MODE mode, const char* szTitle, uint32 ulButtonID, uint32 ulButtonParam, BOOL bIsBlocking, uint32 ulCol, int iconNum )
{
	mTitle = szTitle;
	mMode = mode;
	mulButtonID = ulButtonID;
	mulButtonParam = ulButtonParam;
	mbIsBlocking = bIsBlocking;
	mulCol = ulCol;
	mIconNum = iconNum;
}
	
UIXRECT		UIXButton::OnRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
UIXRECT		localRect = GetDisplayRect();
UIXRECT		drawRect = GetActualRenderRect( displayRect );

	switch( mMode)
	{
	case UIXBUTTON_NORMAL:
	default:
		UIButtonDraw( mulButtonID, drawRect.x, drawRect.y, drawRect.w, drawRect.h, mTitle.c_str(), 5, mulButtonParam, GetID() );
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
		UIX::DrawIcon( pInterface, mIconNum, UIXRECT(drawRect.x + 1, drawRect.y + 1, drawRect.w - 2, drawRect.h - 2), 0xE0A0A0A0 );
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
		
			pInterface->TextRight( 0, drawRect.x + drawRect.w, drawRect.y + ((drawRect.h-stringHeight)/2), 0xD0E0E0E0, 3, mTitle.c_str() );

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


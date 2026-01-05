	

#include "StandardDef.h"
#include "InterfaceEx.h"
#include "../UI/UI.h"
#include "UIXShape.h"


void	UIXShape::Initialise( int mode, BOOL bBlocks, uint32 ulCol1, uint32 ulCol2, uint32 ulButtonID, uint32 ulButtonParam )
{
	mulCol1 = ulCol1;
	mulCol2 = ulCol2;
	mMode = mode;
	mBlocks = bBlocks;
	mulButtonID = ulButtonID;
	mulButtonParam = ulButtonParam;
}
	
UIXRECT		UIXShape::OnRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
UIXRECT		drawRect = GetActualRenderRect( displayRect );

	switch( mMode )
	{
	case 0:
	default:
		pInterface->Rect( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, mulCol1 );

		if ( mulButtonID != 0 )
		{
			if ( UIX::CheckForPress( this, drawRect, mulButtonID, mulButtonParam ) )
			{		
				pInterface->Rect( 1, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0x20FFFFFF );				
			}
		}
		break;
	case 1:
		pInterface->OutlineBox( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, mulCol1 );
		break;
	}

	if ( mBlocks == FALSE )
	{
		displayRect.h = 0;//pInterface->GetStringHeight( mText.c_str(), mFont );
	}
	else
	{
		displayRect.h = GetDisplayRect().h;
	}
	displayRect.y = GetDisplayRect().y + GetDisplayRect().h;		// displayRect.y returns the lowest point we drew to

	return displayRect;
}



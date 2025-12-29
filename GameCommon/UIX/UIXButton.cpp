
#include "StandardDef.h"
#include "InterfaceEx.h"

#include "../UI/UI.h"
#include "UIXButton.h"


void	UIXButton::Initialise( int mode, const char* szTitle, uint32 ulButtonID, uint32 ulButtonParam, BOOL bIsBlocking )
{
	mTitle = szTitle;
	mMode = mode;
	mulButtonID = ulButtonID;
	mulButtonParam = ulButtonParam;
	mbIsBlocking = bIsBlocking;
}
	
UIXRECT		UIXButton::OnRender( InterfaceInstance* pInstance, UIXRECT displayRect )
{
UIXRECT		localRect = GetDisplayRect();

	UIButtonDraw( mulButtonID, displayRect.x + localRect.x, displayRect.y + localRect.y, localRect.w, localRect.h, mTitle.c_str(), mMode, mulButtonParam, GetID() );
	displayRect.h = localRect.h + 1;
	displayRect.y = localRect.h + 1;		// displayRect.y returns the lowest point we drew to

	if ( !mbIsBlocking )
	{
		displayRect.h = 0;
	}
	return( displayRect );
}


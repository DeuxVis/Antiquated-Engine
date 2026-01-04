#include "StandardDef.h"
#include "InterfaceEx.h"
#include "UIXModalPopup.h"



void	UIXModalPopup::Initialise( )
{


}


UIXRECT		UIXModalPopup::OnRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
UIXRECT		drawRect = GetActualRenderRect( displayRect );
//UIXRECT		drawRect = GetDisplayRect();

	// Force interface draw here so the popup is displayed above everything else (we assume the modalPopup is added
	// later in the UIX than everything else)
	pInterface->Draw();

	pInterface->Rect( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0xFF101010 );
			
	displayRect.h = 0;
	displayRect.y = 0;	
	return( displayRect );
}



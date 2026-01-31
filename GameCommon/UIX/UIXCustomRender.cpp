	

#include "StandardDef.h"
#include "InterfaceEx.h"
#include "UIXCustomRender.h"


void	UIXCustomRender::Initialise( fnCustomRenderCallback renderFunc, uint32 ulUserParam )
{
	mfnRenderCallback = renderFunc;
	mulUserParam = ulUserParam;

}
	
UIXRECT		UIXCustomRender::OnRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
UIXRECT		renderRect = GetActualRenderRect( displayRect );
UIXRECT		occupyRect;

	occupyRect = mfnRenderCallback( this, pInterface, renderRect, mulUserParam );

	displayRect.h = 0;  // This is the position relative to our parent that any children will start drawing at
	displayRect.y = GetLocalPositionRect().h + GetLocalPositionRect().y;	
	return displayRect;
}



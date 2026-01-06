	

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

	displayRect.h = 0;//
	displayRect.y = GetDisplayRect().y + occupyRect.h;	
	return displayRect;
}



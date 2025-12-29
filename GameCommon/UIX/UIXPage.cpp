
#include "StandardDef.h"
#include "InterfaceEx.h"
#include "UIX.h"
#include "UIXPage.h"

void	UIXPage::Initialise( const char* szTitle, BOOL bUseClipping )
{
	mbUseClipping = bUseClipping;
}

UIXRECT		UIXPage::OnRender( InterfaceInstance* pInterface, UIXRECT rect )
{
	if ( mbUseClipping )
	{
		pInterface->DrawAllElements();
		mPageRenderRect = rect;
	//	pInterface->SetViewport( rect.x, rect.y, rect.w, rect.h );
		pInterface->SetRenderCanvas();
	}

	rect.y = 0;
	rect.h = 0;
	return rect;
}

void	UIXPage::OnPostChildrenRender( InterfaceInstance* pInterface )
{
	if ( mbUseClipping )
	{
		pInterface->DrawAllElements();
		pInterface->CopyRenderCanvasToBackBuffer( mPageRenderRect.x, mPageRenderRect.y, mPageRenderRect.w, mPageRenderRect.h );
	//	pInterface->SetViewport( 0, 0, pInterface->GetWidth(), pInterface->GetHeight() );
	}
}

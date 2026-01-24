
#include "StandardDef.h"
#include "InterfaceEx.h"
#include "../UI/UI.h"

#include "UIXTabBar.h"

UIXTabBarEntry::UIXTabBarEntry( UIXTabBar* pParent, int index, const char* szTitle, uint32 ulUserParam )
{ 
	mpParent = pParent;
	mIndex = index;
	mTitle = szTitle; 
	mulUserParam = ulUserParam; 
}

UIXRECT			UIXTabBarEntry::Render( InterfaceInstance* pInterface, UIXRECT rect )
{
	uint32	ulTextCol = 0xd0d0d0d0;
	uint32	ulBoxCol = 0x80202020;

	if ( mbSelected )
	{
		ulBoxCol = 0xa0404040;
		ulTextCol = 0xd0F0F0F0;
	}

	int		nFont = 0;
	int		nStringWidth = pInterface->GetStringWidth(mTitle.c_str(), nFont );
	int		nBoxWidth = nStringWidth + 12;

	pInterface->Rect( 0, rect.x, rect.y, nBoxWidth, rect.h, ulBoxCol );
	pInterface->Text( 1, rect.x + 6, rect.y + 4, ulTextCol, nFont, mTitle.c_str() );

	if ( mpParent->IsSelectable() )
	{
		UIXRECT		selectRect = rect;
		selectRect.w = nBoxWidth;
		UIX::CheckForPress( mpParent, selectRect, UIX_TAB_SELECT, mIndex );
	}

	rect.x += nBoxWidth + 1;

	return rect;
}

//-----------------------------------------------
void	UIXTabBar::OnShutdown()
{
	for( UIXTabBarEntry* pElement : mTabList )
	{
		delete pElement;
	}
	mTabList.clear();
}

bool	UIXTabBar::OnSelected( int nButtonID, uint32 ulParam )
{
	if ( mSelectionChangedCallback )
	{
		mSelectionChangedCallback( this, ulParam );
	}
	return false;
}

void	UIXTabBar::Initialise( int mode )
{
	mMode = mode;
}

UIXRECT		UIXTabBar::OnRender( InterfaceInstance* pInterface, UIXRECT rect )
{
UIXRECT		lineRect = GetActualRenderRect( rect );

	for( UIXTabBarEntry* pElement : mTabList )
	{
		lineRect = pElement->Render( pInterface, lineRect );
	}

	rect.h = GetDisplayRect().h + 1;
	rect.y = GetDisplayRect().h + 1;
	return( rect );
}


UIXTabBarEntry*		UIXTabBar::AddElement(	const char* szElementName, uint32 ulElementParam )
{
UIXTabBarEntry*	pNewElement = new UIXTabBarEntry( this, mulNextElementIndex++, szElementName, ulElementParam );

	mTabList.push_back( pNewElement );
	return( pNewElement );
}


const UIXTabBarEntry*	UIXTabBar::GetElementByListIndex( int index ) const
{
	return( mTabList[index] );
}

void		UIXTabBar::RegisterControlHandlers()
{

}


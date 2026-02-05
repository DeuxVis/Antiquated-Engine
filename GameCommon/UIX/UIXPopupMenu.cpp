
#include <stdarg.h>
#include "StandardDef.h"
#include "InterfaceEx.h"
#include "UIX.h"
#include "UIXPopupMenu.h"



void	UIXPopupMenuItem::Initialise( const char* szText, fnSelectedCallback selectCallback, uint32 ulSelectParam )
{
	mText = szText;
	SetSelectedCallback( selectCallback, ulSelectParam );
}

UIXPopupMenuItem*		UIXPopupMenuItem::AddMenuItem( const char* szMenuItemName, fnSelectedCallback selectCallback, uint32 ulSelectParam, BOOL bIsCheckbox, BOOL bIsChecked )
{
UIXPopupMenuItem*		pNewMenuItem = new UIXPopupMenuItem(this, UIX::GetNextObjectID(), UIXRECT(0, 0, 0, 0));

	pNewMenuItem->Initialise(szMenuItemName, selectCallback, ulSelectParam );
	pNewMenuItem->SetCheckbox( bIsCheckbox, bIsChecked );
	GetChildObjectList().push_back( pNewMenuItem );
	return( pNewMenuItem );
}

/*
void		UIXPopupMenuItem::OnCloseAllMenus()
{
	mbIsExpanded = false;
	mspExpandedMenuTopLayer = NULL;
}
*/

bool		UIXPopupMenuItem::OnSelected( int nButtonID, uint32 ulParam )
{
	if (GetChildObjectList().size() > 0)
	{
		if ( mbIsExpanded )
		{
			mbIsExpanded = false;
//			mspExpandedMenuTopLayer = NULL;	
		}
		else
		{
/*			if ( mspExpandedMenuTopLayer != NULL )
			{
				mspExpandedMenuTopLayer->SetExpanded( false );
				mspExpandedMenuTopLayer = NULL;
			}
*/
			mbIsExpanded = true;
//			mspExpandedMenuTopLayer = this;
		}
	}
	return( true );
}

/*
void		UIXPopupMenuItem::OnEscape()
{
	mbIsExpanded = false;
	mspExpandedMenuTopLayer = NULL;
}
*/

void		UIXPopupMenuItem::DoRender( InterfaceInstance* pInterface, UIXRECT rect )
{
	int		nNumChildren = GetChildObjectList().size();
	UIXPopupMenuItem* pMenuItem;

	rect.h = nNumChildren * 20 + 4;
	pInterface->ShadedRect(0, rect.x, rect.y, rect.w, rect.h, 0xFF202020, 0xFF202020, 0xFF181818, 0xFF181818);
	rect.y += 4;
	for ( UIXObject* pItem : GetChildObjectList() )
	{
		pMenuItem = (UIXPopupMenuItem*)pItem;
		rect.h = 16;
		uint32		ulTextCol = 0xd0d0d0d0;
		uint32		ulHilightCol = 0x80808080;

		// If no sub-items and no selection callback, this menu item is greyed out
		if ( ( pMenuItem->GetChildObjectList().size() == 0 ) &&
			 ( pMenuItem->HasSelectionCallback() == false ) )
		{
			ulTextCol = 0x80808080;
		}
		else if (UIX::IsMouseHover(rect))
		{
			pInterface->Rect(1, rect.x, rect.y, rect.w, rect.h, ulHilightCol);
			ulTextCol = 0xe0e0e0e0;
			UIX::CheckForPress(pMenuItem, rect, UIX_MENU_ITEM, 0);
		}
		pInterface->Text(0, rect.x + 5, rect.y + 1, ulTextCol, 0, pMenuItem->GetText());
		rect.y += 18;
	}
}

//---------------------------------------------------------------------
UIXPopupMenuItem*	UIXPopupMenu::AddHeaderMenuItem( const char* text, ... )
{
UIXPopupMenuItem*		pNewMenuItem = new UIXPopupMenuItem(this, UIX::GetNextObjectID(), UIXRECT(0, 0, 0, 0));
char		acString[2048];
va_list		marker;
uint32*		pArgs;

	pArgs = (uint32*)( &text ) + 1;

    va_start( marker, text );     
	vsprintf( acString, text, marker );

	pNewMenuItem->Initialise(acString, NULL, 0 );
	pNewMenuItem->SetHeader( TRUE );
	GetChildObjectList().push_back( pNewMenuItem );
	return( pNewMenuItem );
}

UIXPopupMenuItem*	UIXPopupMenu::AddMenuItem( const char* szMenuItemName, fnSelectedCallback selectedCallback, uint32 ulSelectParam, BOOL bIsCheckbox, BOOL bIsChecked )
{
UIXPopupMenuItem*		pNewMenuItem = new UIXPopupMenuItem(this, UIX::GetNextObjectID(), UIXRECT(0, 0, 0, 0));

	pNewMenuItem->Initialise(szMenuItemName, selectedCallback, ulSelectParam );
	pNewMenuItem->SetCheckbox( bIsCheckbox, bIsChecked );
	GetChildObjectList().push_back( pNewMenuItem );
	return( pNewMenuItem );
}

void	UIXPopupMenu::Initialise( )
{
}

UIXRECT		UIXPopupMenu::OnRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
UIXRECT		drawRect = GetActualRenderRect( displayRect );
//UIXRECT		drawRect = GetLocalPositionRect();

	// Force interface draw here so the popup is displayed above everything else (we assume the modalPopup is added
	// later in the UIX than everything else)
	pInterface->Draw();

	pInterface->Rect( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0xFF101010 );
			
	displayRect.h = 0;
	displayRect.y = 0;	
	return( displayRect );
}

void		UIXPopupMenu::OnPostRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
UIXRECT		renderRect = GetActualRenderRect( displayRect );
UIXRECT		selectRect;
uint32		ulTextCol = 0xD0C0C0C0;
uint32		ulHilightCol = 0x80808080;

	pInterface->ShadedRect( 0, renderRect.x, renderRect.y, renderRect.w, renderRect.h, 0xFF101010, 0xFF101010, 0xFF080808, 0xFF080808 );
	pInterface->OutlineBox( 0, renderRect.x, renderRect.y, renderRect.w, renderRect.h, 0x90606060 );

	UIXRECT	lineDividerRect( renderRect.x + 2, 0, renderRect.w - 2, 2 );
	UIXRECT	lineRect( renderRect.x + 4, renderRect.y + 4, renderRect.w - 4, 18 );

	for ( UIXObject* pChild : GetChildObjectList() )
	{
		UIXPopupMenuItem* pMenuItem = (UIXPopupMenuItem*)pChild;
		ulTextCol = 0xD0C0C0C0;

		if ( *pMenuItem->GetText() == '-' )
		{
 			lineDividerRect.y = lineRect.y;	
			pInterface->Rect( 0, lineDividerRect.x, lineDividerRect.y, lineDividerRect.w, lineDividerRect.h, 0xd0303030 );
			lineRect.y += 4;
		}
		else
		{	
			selectRect = lineRect;
			selectRect.x -= 3;
			pMenuItem->SetLastRenderRect(selectRect);

			if ( pMenuItem->IsChecked() )
			{
			UIXRECT		checkedIconRect = lineRect;
			
				checkedIconRect.y += 2;
				checkedIconRect.h -= 8;
				checkedIconRect.w = checkedIconRect.h;
				checkedIconRect.x += 2;

				// TODO - This should be tick icon or somethin
				pInterface->Rect( 0, checkedIconRect.x, checkedIconRect.y, checkedIconRect.w, checkedIconRect.h, 0xC05050d0 );
			}

			if ( pMenuItem->HasSelectionCallback() )
			{
				if ( UIX::IsMouseHover( selectRect ) )
				{
					pInterface->Rect( 0, selectRect.x, selectRect.y, selectRect.w, selectRect.h, ulHilightCol );	
					ulTextCol = 0xe0e0e0e0;

					UIX::CheckForPress( pMenuItem, selectRect, UIX_POPUP_MENU_ITEM, 0 );
				}
			}
			else
			{
				ulTextCol = 0x80606060;	
			}

			if ( pMenuItem->IsCheckbox() )
			{
				pInterface->Text(0, lineRect.x + 30, lineRect.y, ulTextCol, 0, pMenuItem->GetText() );		
			}
			else if ( pMenuItem->IsHeader() )
			{
				ulTextCol = 0xd0C0A070;
				pInterface->TextLimitWidth(0, lineRect.x + 5, lineRect.y, lineRect.w - 6, ulTextCol, 3, pMenuItem->GetText() );		
			}
			else
			{
				pInterface->Text(0, lineRect.x + 5, lineRect.y, ulTextCol, 0, pMenuItem->GetText() );
			}
			lineRect.y += lineRect.h + 1;
		}
	}

	// ------------------------------- Render all child items
	for ( UIXObject* pChild : GetChildObjectList() )
	{
		UIXPopupMenuItem* pMenuItem = (UIXPopupMenuItem*)pChild;
		if (pMenuItem->IsExpanded())
		{
			UIXRECT		itemRect = pMenuItem->GetLastRenderRect();
			itemRect.y += itemRect.h;
			if ( itemRect.w < 150 ) itemRect.w = 150;
			itemRect.h = pInterface->GetHeight() - (itemRect.y + 10);
			pMenuItem->DoRender( pInterface, itemRect );
		}
	}

}



#include <stdarg.h>
#include "StandardDef.h"
#include "InterfaceEx.h"

#include "../Platform/Platform.h"
#include "UIX.h"
#include "UIXPopupMenu.h"



void	UIXPopupMenuItem::Initialise( const char* szText, fnSelectedCallback selectCallback, uint32 ulSelectParam )
{
	mText = szText;
	SetSelectedCallback( selectCallback, ulSelectParam );
}


UIXPopupMenuItem*		UIXPopupMenuItem::AddHeaderMenuItem( const char* text, ... )
{
UIXPopupMenuItem*		pNewMenuItem = new UIXPopupMenuItem(this, UIX::GetNextObjectID(), UIXRECT(0, 0, 0, 0));
char		acString[2048];
va_list		marker;
uint32*		pArgs;

	pArgs = (uint32*)( &text ) + 1;

    va_start( marker, text );     
	vsprintf( acString, text, marker );

	pNewMenuItem->Initialise( acString );
	pNewMenuItem->SetHeader( TRUE );
	GetChildObjectList().push_back( pNewMenuItem );
	return( pNewMenuItem );


}

void		UIXPopupMenuItem::SetExpanded( BOOL bFlag )
{
	mbIsExpanded = bFlag;
	mfExpandDelayTime = 0.0f;
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

void	UIXPopupMenuItem::OnHover( )
{
	if ( mbIsExpanded == FALSE )
	{
		mfExpandDelayTime += PlatformGetFrameDelta();
		if ( mfExpandDelayTime > 0.25f )
		{
		UIXPopupMenu*		pParentMenu = (UIXPopupMenu*)GetParent();
			
			pParentMenu->ClearExpandedChildren();
			mbIsExpanded = TRUE;
		}
	}
}

int		UIXPopupMenuItem::Display( InterfaceInstance* pInterface, UIXRECT lineRect )
{
uint32	ulTextCol = 0xD0C0C0C0;
UIXRECT	lineDividerRect( lineRect.x + 2, 0, lineRect.w - 2, 2 );
UIXRECT		selectRect;
uint32		ulHilightCol = 0x80808080;
int			nHeightUsed = lineRect.h;

	if ( *GetText() == '-' )
	{
		lineDividerRect.y = lineRect.y;	
		pInterface->Rect( 0, lineDividerRect.x, lineDividerRect.y, lineDividerRect.w, lineDividerRect.h, 0xd0303030 );
		nHeightUsed = 4;
	}
	else
	{	
		selectRect = lineRect;
		selectRect.x -= 3;
		SetLastRenderRect(selectRect);

		if ( IsChecked() )
		{
		UIXRECT		checkedIconRect = lineRect;
		
			checkedIconRect.h = 19;
			checkedIconRect.w = checkedIconRect.h;
			checkedIconRect.y -= 3;

			UIX::DrawIcon( pInterface, 11, checkedIconRect, 0xd0d0d0d0 );

			// TODO - This should be tick icon or somethin
//			pInterface->Rect( 0, checkedIconRect.x, checkedIconRect.y, checkedIconRect.w, checkedIconRect.h, 0xC05050d0 );
		}

		if ( HasSelectionCallback() )
		{
			if ( UIX::IsMouseHover( selectRect ) )
			{
				pInterface->Rect( 0, selectRect.x, selectRect.y, selectRect.w, selectRect.h, ulHilightCol );	
				ulTextCol = 0xe0e0e0e0;

				UIX::CheckForPress( this, selectRect, UIX_POPUP_MENU_ITEM, 0 );

				if ( HasRightClickSelectionCallback() )
				{
					UIX::CheckForRightButtonPress(this, selectRect, UIX_RIGHT_CLICK_SELECT, 0);
				}
			}
		}
		else if ( HasChildren() )
		{
		UIXRECT		triRect = selectRect;

			triRect.x += triRect.w - 15;
			triRect.y += 3;
			pInterface->Triangle( 1, triRect.x + 4, triRect.y, triRect.x + 10, triRect.y + 4, triRect.x + 4, triRect.y + 8, 0xa0a0a0a0, 0xa0a0a0a0, 0xa0a0a0a0 );	

			if ( UIX::IsMouseHover( selectRect ) )
			{
				OnHover();
			}
		}
		else
		{
			ulTextCol = 0x80606060;	
		}


		if ( IsCheckbox() )
		{
			pInterface->Text(0, lineRect.x + 30, lineRect.y, ulTextCol, 0, GetText() );		
		}
		else if ( IsHeader() )
		{
			ulTextCol = 0xd0C0A070;
			pInterface->TextLimitWidth(0, lineRect.x + 5, lineRect.y, lineRect.w - 6, ulTextCol, 3, GetText() );		
			nHeightUsed = 14;
		}
		else
		{
			pInterface->Text(0, lineRect.x + 5, lineRect.y, ulTextCol, 0, GetText() );
		}
	}
	return( nHeightUsed );
}

void		UIXPopupMenuItem::DoRender( InterfaceInstance* pInterface, UIXRECT rect )
{
	int		nNumChildren = GetChildObjectList().size();
	UIXPopupMenuItem* pMenuItem;

	rect.h = nNumChildren * 20 + 4;
	pInterface->ShadedRect( 0, rect.x, rect.y, rect.w, rect.h, 0xFF101010, 0xFF101010, 0xFF080808, 0xFF080808 );
	pInterface->OutlineBox( 0, rect.x, rect.y, rect.w, rect.h, 0x90606060 );

	rect.y += 4;
	rect.h = 17;
	for ( UIXObject* pItem : GetChildObjectList() )
	{
		pMenuItem = (UIXPopupMenuItem*)pItem;
		rect.y += pMenuItem->Display( pInterface, rect ) + 1;
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

void	UIXPopupMenu::ClearExpandedChildren()
{
	for ( UIXObject* pChild : GetChildObjectList() )
	{
		UIXPopupMenuItem* pMenuItem = (UIXPopupMenuItem*)pChild;
		pMenuItem->SetExpanded( FALSE );
	}
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
		lineRect.y += pMenuItem->Display( pInterface, lineRect ) + 1;
	}

	// ------------------------------- Render all child items
	for ( UIXObject* pChild : GetChildObjectList() )
	{
		UIXPopupMenuItem* pMenuItem = (UIXPopupMenuItem*)pChild;
		if (pMenuItem->IsExpanded())
		{
			UIXRECT		itemRect = pMenuItem->GetLastRenderRect();
			itemRect.x += itemRect.w + 5;
			itemRect.h = pInterface->GetHeight() - (itemRect.y + 10);
			pMenuItem->DoRender( pInterface, itemRect );
		}
	}

}



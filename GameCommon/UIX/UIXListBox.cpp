
#include "StandardDef.h"
#include "InterfaceEx.h"
#include "../UI/UI.h"

#include "UIXListBox.h"

UIXListBoxEntry::UIXListBoxEntry( UIXListBox* pListbox, const char* szTitle, uint32 ulUserParam, BOOL bSelectable )
{ 
	mpListBox = pListbox; 
	mTitle = szTitle; 
	mulUserParam = ulUserParam; 
	mIndex = pListbox->GetNextIndex( this ); 
	mbSelectable = bSelectable;
}

UIXListBoxEntry*	UIXListBoxEntry::AddSubElement( const char* szElementName, uint32 ulElementParam, BOOL bSelectable )
{
UIXListBoxEntry*	pNewElement = new UIXListBoxEntry( mpListBox, szElementName, ulElementParam, bSelectable );

	mChildren.push_back( pNewElement );
	return( pNewElement );
}


UIXRECT			UIXListBoxEntry::Render( InterfaceInstance* pInstance, UIXRECT rect )
{
	mLastRender = rect;
	mLastRender.h = 21;
	uint32	ulTextCol = 0xd0e0e0e0;

	if ( mbSelectable )
	{
		if ( UIX::IsMouseHover( rect.x, rect.y, rect.w, 19 ) == TRUE )
		{
			UIHoverIDSet( UIX_LISTBOX, mIndex, mpListBox->GetID() );
			ulTextCol = 0xE0F0F0F0;
			pInstance->Rect( 0, rect.x, rect.y, rect.w, 19, 0x40404040 );
		}

		if ( mpListBox->IsSelectable() )
		{
		UIXRECT		selectRect = rect;
			selectRect.h = 19;
			UIX::CheckForPress( mpListBox, selectRect, UIX_LISTBOX_SELECT, mulUserParam );
		}
	}
	else
	{
		ulTextCol = 0xa0808080;
	}

	pInstance->Text( 1, rect.x + 5, rect.y + 2, ulTextCol, 0, mTitle.c_str() );
	rect.ConsumeVertical( 22 );

	if ( !mChildren.empty() )
	{
		rect.ConsumeHorizontal( 25 );
		for( UIXListBoxEntry* pElement : mChildren )
		{
			pElement->Render( pInstance, rect );
			rect.ConsumeVertical( 22 );
		}
		rect.ConsumeHorizontal( -25 );	
	}

	return rect;
}

//-----------------------------------------------
void	UIXListBox::OnShutdown()
{
	for( UIXListBoxEntry* pElement : mListBoxEntries )
	{
		delete pElement;
	}
	mListBoxEntries.clear();
}

bool	UIXListBox::OnSelected( int nButtonID, uint32 ulParam )
{
	if ( mSelectionChangedCallback )
	{
		mSelectionChangedCallback( this, ulParam );
	}
	return false;
}

void	UIXListBox::Initialise( int mode, BOOL bContentsDraggable, int dragItemType )
{
	mMode = mode;
	mbContentsDraggable = bContentsDraggable;
	SetDraggable(dragItemType, 0);
}

UIXRECT		UIXListBox::OnRender( InterfaceInstance* pInterface, UIXRECT rect )
{
UIXRECT		lineRect = GetActualRenderRect( rect );

	for( UIXListBoxEntry* pElement : mListBoxEntries )
	{
		lineRect = pElement->Render( pInterface, lineRect );
	}

	// Render the drag item if appropriate
	if ( IsDragHoldActive() &&  mpDragHeldEntry )
	{
	UIXRECT	xGrabOffset = GetDragOffset();
	UIXRECT xNewRect = GetInitialDragRect();

		xNewRect.x += xGrabOffset.x;
		xNewRect.y += xGrabOffset.y;

		pInterface->Rect( 1, xNewRect.x, xNewRect.y, xNewRect.w, xNewRect.h, 0x40303030 );
		pInterface->Text( 2, xNewRect.x + 5, xNewRect.y + 2, 0x80a0a0a0, 0, mpDragHeldEntry->GetText().c_str() );
	}

	// 
	// TODO - Return correct occupyRect
	rect.h = GetLocalPositionRect().h + 1;
	rect.y = GetLocalPositionRect().h + 1;
	return( rect );
}


UIXListBoxEntry*		UIXListBox::AddElement(	const char* szElementName, uint32 ulElementParam, BOOL bSelectable )
{
UIXListBoxEntry*	pNewElement = new UIXListBoxEntry( this, szElementName, ulElementParam, bSelectable );

	mListBoxEntries.push_back( pNewElement );
	return( pNewElement );
}

BOOL	UIXListBox::OnDragHoldUpdate( uint32 ulIndex, BOOL bIsHeld, BOOL bFirstPress )
{
	if ( bFirstPress )
	{
		UIXListBoxEntry* pListBoxElement = mListBoxEntriesFlatList[ulIndex];
		if ( pListBoxElement )
		{
			mpDragHeldEntry = pListBoxElement;
			ActivateDragHold( pListBoxElement->GetLastRenderRect(), pListBoxElement->mIndex );
			return( TRUE );
		}
	}
	else if ( !bIsHeld )  // Just released
	{
		mpDragHeldEntry = NULL;
	}
	return( FALSE );
}


const UIXListBoxEntry*	UIXListBox::GetElementByListIndex( int index ) const
{
	return( mListBoxEntriesFlatList[index] );
}





#include "StandardDef.h"
#include "InterfaceEx.h"
#include "../UI/UI.h"

#include "UIXListBox.h"

UIXListBoxEntry::UIXListBoxEntry( UIXListBox* pListbox, const char* szTitle, uint32 ulUserParam )
{ 
	mpListBox = pListbox; 
	mTitle = szTitle; 
	mulUserParam = ulUserParam; 
	mIndex = pListbox->GetNextIndex( this ); 
}

UIXListBoxEntry*	UIXListBoxEntry::AddSubElement( const char* szElementName, uint32 ulElementParam )
{
UIXListBoxEntry*	pNewElement = new UIXListBoxEntry( mpListBox, szElementName, ulElementParam );

	mChildren.push_back( pNewElement );
	return( pNewElement );
}


UIXRECT			UIXListBoxEntry::Render( InterfaceInstance* pInstance, UIXRECT rect )
{
	mLastRender = rect;
	mLastRender.h = 21;
	uint32	ulTextCol = 0xd0d0d0d0;

	if ( UIHoverItem(  rect.x, rect.y, rect.w, 19 ) == TRUE )
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
	mDragItemType = dragItemType;
}

UIXRECT		UIXListBox::OnRender( InterfaceInstance* pInterface, UIXRECT rect )
{
UIXRECT		lineRect = GetActualRenderRect( rect );

	for( UIXListBoxEntry* pElement : mListBoxEntries )
	{
		lineRect = pElement->Render( pInterface, lineRect );
	}

	// Render the drag item if appropriate
	if ( mpDragHeldEntry )
	{
	UIXRECT		xCurrMouse;

		UIGetCurrentCursorPosition( &xCurrMouse.x, &xCurrMouse.y );
		UIXRECT	xGrabOffset( xCurrMouse.x - mDragRectMouseOriginal.x, xCurrMouse.y - mDragRectMouseOriginal.y, 0, 0 );

		UIXRECT xNewRect = mDragRectOriginal;

		xNewRect.x += xGrabOffset.x;
		xNewRect.y += xGrabOffset.y;

		pInterface->Rect( 1, xNewRect.x, xNewRect.y, xNewRect.w, xNewRect.h, 0x40303030 );
		pInterface->Text( 2, xNewRect.x + 5, xNewRect.y + 2, 0x80a0a0a0, 0, mpDragHeldEntry->GetText().c_str() );
	
	}
	// 
	// TODO - Return correct occupyRect
	rect.h = GetDisplayRect().h + 1;
	rect.y = GetDisplayRect().h + 1;
	return( rect );
}


UIXListBoxEntry*		UIXListBox::AddElement(	const char* szElementName, uint32 ulElementParam )
{
UIXListBoxEntry*	pNewElement = new UIXListBoxEntry( this, szElementName, ulElementParam );

	mListBoxEntries.push_back( pNewElement );
	return( pNewElement );
}

void	UIXListBox::HoldHandler( uint32 ulIndex, BOOL bIsHeld, BOOL bFirstPress )
{
UIXListBoxEntry*		pListBoxElement = mListBoxEntriesFlatList[ulIndex];

	if ( bFirstPress )
	{
		if ( pListBoxElement )
		{
			mpDragHeldEntry = pListBoxElement;
			mDragRectOriginal = pListBoxElement->GetLastRenderRect();
			UIX::SetDragItemType( mDragItemType, this, pListBoxElement->mIndex );
			UIGetCurrentCursorPosition( &mDragRectMouseOriginal.x, &mDragRectMouseOriginal.y );
		}
	}
	else if ( bIsHeld ) 
	{
		
	}
	else  // Just released
	{
		mpDragHeldEntry = NULL;
		UIX::EndDragItemType( mDragItemType );
	}

}


void	UIXListBox::HoldHandlerStatic( int nButtonID, uint32 ulParam, uint32 ulIDParam, BOOL bIsHeld, BOOL bFirstPress )
{
UIXListBox*		pListBox = (UIXListBox*)UIX::FindUIXObjectByID( ulIDParam );

	if ( pListBox )
	{
		pListBox->HoldHandler( ulParam, bIsHeld, bFirstPress );
	}
}

const UIXListBoxEntry*	UIXListBox::GetElementByListIndex( int index ) const
{
	return( mListBoxEntriesFlatList[index] );
}

void		UIXListBox::RegisterControlHandlers()
{
	UIRegisterHoldHandler( UIX_LISTBOX, HoldHandlerStatic );

}


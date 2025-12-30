
#include "StandardDef.h"
#include "InterfaceEx.h"

#include "../UI/UI.h"
#include "UIXDropdown.h"

UIXDropdownEntry::UIXDropdownEntry( UIXDropdown* pParent, const char* szTitle, uint32 ulUserParam )
{
	mTitle = szTitle;
	mulUserParam = ulUserParam;
	mpDropdown = pParent;

}
	

//-------------------------------------


void	UIXDropdown::Initialise( int mode )
{
}

void	UIXDropdown::ToggleExpanded()
{ 
	mbIsExpanded = !mbIsExpanded;
	if ( mbIsExpanded )
	{
		UIX::SetModalObject( this );
	}	
	else
	{
		if ( UIX::GetModalObject() == this )
		{
			UIX::SetModalObject( NULL );
		}
	}
}


UIXRECT		UIXDropdown::OnRender( InterfaceInstance* pInterface, UIXRECT rect )
{
UIXRECT		renderRect = GetActualRenderRect( rect );
UIXRECT		occupyRect = rect;
int			entryIndex = 0;

	pInterface->Rect( 1, renderRect.x, renderRect.y, renderRect.w, renderRect.h, 0xa0202020 );
	pInterface->OutlineBox( 1, renderRect.x, renderRect.y, renderRect.w, renderRect.h, 0xa0606060 );
	pInterface->Triangle( 1, renderRect.x + renderRect.w - 9, renderRect.y + 3, renderRect.x + renderRect.w - 3, renderRect.y + 3, renderRect.x + renderRect.w - 6, renderRect.y + 9, 0xA0A0A0A0, 0xA0A0A0A0, 0xA0A0A0A0 );

	if ( mpSelectedEntry )
	{
		pInterface->Text( 1, renderRect.x + 3, renderRect.y + 3, 0xD0D0D0D0, 3, mpSelectedEntry->GetText().c_str() );
	}
		
	if ( ( UIX::GetModalObject() == NULL ) ||
		 ( UIX::GetModalObject() == this ) ) 
	{
		if ( UIIsPressed( renderRect.x, renderRect.y, renderRect.w, renderRect.h ) == TRUE )
		{
			UIPressIDSet( UIX_DROPDOWN_HEADER, GetID() );
		}
	}
	
	if ( mbIsExpanded )
	{
	UIXRECT		expandedRect = renderRect;

		expandedRect.y += renderRect.h + 1;
		expandedRect.h = (mDropdownEntries.size() * 14) + 4;
		// Show list..

		pInterface->Rect( 2, expandedRect.x, expandedRect.y, expandedRect.w, expandedRect.h, 0xF0202020 );
		pInterface->OutlineBox( 2, expandedRect.x, expandedRect.y, expandedRect.w, expandedRect.h, 0xF0606060 );

		UIXRECT		lineRect = expandedRect;
		lineRect.y += 2;
		lineRect.h = 14;

		for ( auto listEntry : mDropdownEntries )
		{
			pInterface->Text( 2, lineRect.x + 3, lineRect.y, 0xc0d0d0d0, 3, listEntry->GetText().c_str() );
			if ( UIHoverItem( lineRect.x, lineRect.y, lineRect.w, lineRect.h ) == TRUE )
			{
				pInterface->Rect( 2, lineRect.x, lineRect.y, lineRect.w, lineRect.h, 0xa0808080 );
				UIPressIDSet( UIX_DROPDOWN_ENTRY, GetID(), entryIndex );			
			}
			lineRect.y += 14;
			entryIndex++;
		}
	}

	occupyRect.h = 0;
	occupyRect.y = renderRect.h + 1;
	return occupyRect;
}

void	UIXDropdown::SetSelectedElementName( const char* szName )
{
int		index = 0;
	for ( auto listEntry : mDropdownEntries )
	{
		if ( stricmp( szName, listEntry->GetText().c_str() ) == 0 )
		{
			mpSelectedEntry = listEntry;
			mnSelectedIndex = index;
			if ( mSelectionChangedCallback )
			{
				mSelectionChangedCallback( this, mpSelectedEntry->mulUserParam );
			}
			return;
		}
		index++;
	}

}
void		UIXDropdown::OnUpdate( float fDelta )
{
	if ( mValueUpdateFunc )
	{
	int nNewVal = (int)mValueUpdateFunc( this, mnSelectedIndex );

		if ( mnSelectedIndex != nNewVal )
		{
			if ( ( nNewVal >= 0 ) &&
				 ( nNewVal < (int)mDropdownEntries.size() ) )
			{
				mnSelectedIndex = nNewVal;
				mpSelectedEntry = mDropdownEntries[nNewVal];
			}
		}
	}
}


void		UIXDropdown::SetSelectedElementIndex( int index )
{
	if ( index < (int)mDropdownEntries.size() )
	{
		mpSelectedEntry = mDropdownEntries[index];
		mnSelectedIndex = index;

		if ( mSelectionChangedCallback )
		{
			mSelectionChangedCallback( this, mpSelectedEntry->mulUserParam );
		}
	}
	else
	{
		mpSelectedEntry = NULL;
		mnSelectedIndex = NOTFOUND;
	}
}

UIXDropdownEntry*	UIXDropdown::AddElement( const char* szElementName, uint32 ulElementParam )
{
UIXDropdownEntry*		pNewEntry = new UIXDropdownEntry( this, szElementName, ulElementParam );

	mDropdownEntries.push_back( pNewEntry );
	return( pNewEntry );
}

const UIXDropdownEntry*		UIXDropdown::GetElementByListIndex( int index ) const
{
	if ( index < (int)mDropdownEntries.size() )
	{
		return( mDropdownEntries[index] );
	}
	return( NULL );
}
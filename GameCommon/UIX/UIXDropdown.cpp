
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

UIXDropdown::~UIXDropdown()
{
	SAFE_DELETE( mpScrollbar );
	for ( UIXDropdownEntry* pListEntry : mDropdownEntries )
	{
		delete pListEntry;
	}
	mDropdownEntries.clear();
}


void	UIXDropdown::Initialise( int mode )
{
	mpScrollbar = new UIXScrollbar(this, UIX::GetNextObjectID(), UIXRECT(0, 0, 0, 0));

}

void		UIXDropdown::OnMouseWheel(float fOffset)
{
	mpScrollbar->OnMouseWheel(fOffset);
}

void	UIXDropdown::ToggleExpanded()
{ 
	mbIsExpanded = !mbIsExpanded;

}

void	UIXDropdown::OnSelected( int nButtonID, uint32 ulParam)
{
	switch( nButtonID )
	{
	case UIX_DROPDOWN_ENTRY:
		SetSelectedElementIndex( ulParam );
		ToggleExpanded();
		break;
	case UIX_DROPDOWN_HEADER:
		ToggleExpanded();
		break;
	default:
		PANIC_IF(TRUE, "UIXDropdown::OnSelected - Unknown button ID");
		break;
	}
}


void	UIXDropdown::OnEscape()
{
	mbIsExpanded = FALSE;
}



UIXRECT		UIXDropdown::OnRender( InterfaceInstance* pInterface, UIXRECT rect )
{
UIXRECT		renderRect = GetActualRenderRect( rect );
UIXRECT		occupyRect = rect;

pInterface->Rect( 1, renderRect.x, renderRect.y, renderRect.w, renderRect.h, 0xa0202020 );
	pInterface->OutlineBox( 1, renderRect.x, renderRect.y, renderRect.w, renderRect.h, 0xa0606060 );
	pInterface->Triangle( 1, renderRect.x + renderRect.w - 9, renderRect.y + 3, renderRect.x + renderRect.w - 3, renderRect.y + 3, renderRect.x + renderRect.w - 6, renderRect.y + 9, 0xA0A0A0A0, 0xA0A0A0A0, 0xA0A0A0A0 );

	if ( mpSelectedEntry )
	{
		pInterface->Text( 1, renderRect.x + 3, renderRect.y + 3, 0xD0D0D0D0, 3, mpSelectedEntry->GetText().c_str() );
	}
	
//	if ( ( UIX::GetModalObject() == NULL ) ||
//		 ( UIX::GetModalObject() == this ) ) 
//	{
		UIX::CheckForPress( this, renderRect, UIX_DROPDOWN_HEADER, 0 );
//	}


	occupyRect.h = 0;//GetDisplayRect().h + 1;
	occupyRect.y = GetDisplayRect().y + GetDisplayRect().h + 1;
	return occupyRect;
}

void		UIXDropdown::OnPostRender( InterfaceInstance* pInterface, UIXRECT rect )
{
UIXRECT		renderRect = GetActualRenderRect( rect );
int			entryIndex = 0;
	
	if ( mbIsExpanded )
	{
	UIXRECT		expandedRect = renderRect;
	int			nMaxDropdownH;
	bool		bScrollbarRequired = false;
	int			nMaxNumEntriesInView = 1;


		
		expandedRect.y += renderRect.h + 1;
		expandedRect.h = (mDropdownEntries.size() * 14) + 4;
		// Show list..

		nMaxDropdownH = pInterface->GetHeight() - (expandedRect.y + 4);
		nMaxNumEntriesInView = (nMaxDropdownH - 4) / 14;
		if (nMaxNumEntriesInView < 1) nMaxNumEntriesInView = 1;

		if (nMaxNumEntriesInView < 5)
		{
			expandedRect.h = (mDropdownEntries.size() * 14) + 4;
			expandedRect.y = renderRect.y - expandedRect.h;
			if (expandedRect.y < 10) expandedRect.y = 10;
			nMaxDropdownH = renderRect.y - expandedRect.y;
			nMaxNumEntriesInView = (nMaxDropdownH - 4) / 14;
			if (nMaxNumEntriesInView < 1) nMaxNumEntriesInView = 1;
		}

		UIXRECT		lineRect = expandedRect;
		lineRect.y += 2;
		lineRect.h = 14;

		if (expandedRect.h > nMaxDropdownH)
		{
			bScrollbarRequired = true;

			UIXRECT		scrollbarRect;

			scrollbarRect.x = expandedRect.x + expandedRect.w - 14;
			scrollbarRect.w = 13;
			scrollbarRect.y = expandedRect.y + 1;
			scrollbarRect.h = nMaxDropdownH - 2;

			mpScrollbar->RenderDirect(pInterface, scrollbarRect, nMaxNumEntriesInView, mDropdownEntries.size() );

			lineRect.w -= 13;
			expandedRect.h = nMaxDropdownH;
		}

		if (UIHoverItem(expandedRect.x, expandedRect.y, expandedRect.w, expandedRect.h) == TRUE)
		{
			UIX::SetMousewheelHoverObject(this);
		}
		pInterface->Rect( 0, expandedRect.x, expandedRect.y, expandedRect.w, expandedRect.h, 0xF0202020 );
		pInterface->OutlineBox( 0, expandedRect.x, expandedRect.y, expandedRect.w, expandedRect.h, 0xF0606060 );

		for ( auto listEntry : mDropdownEntries )
		{
			if ((entryIndex >= mpScrollbar->GetScrollPosition()) &&
				(entryIndex < mpScrollbar->GetScrollPosition() + nMaxNumEntriesInView))
			{
				pInterface->TextLimitWidth(2, lineRect.x + 3, lineRect.y, lineRect.w, 0xc0d0d0d0, 3, listEntry->GetText().c_str());

				if (UIX::CheckForPress(this, lineRect, UIX_DROPDOWN_ENTRY, entryIndex))
				{
					pInterface->Rect(1, lineRect.x, lineRect.y, lineRect.w, lineRect.h, 0xa0808080);
				}
				lineRect.y += 14;
			}
			entryIndex++;
		}
	}
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
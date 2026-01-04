	

#include "StandardDef.h"
#include "InterfaceEx.h"
#include "../UI/UI.h"
#include "UIXCheckbox.h"


void	UIXCheckbox::Initialise( UIX_CHECKBOX_MODE mode, BOOL bIsChecked, const char* szText, fnSelectedCallback selectedFunc )
{
	mbIsChecked = bIsChecked;
	mfnSelectedCallback = selectedFunc;
	mMode = mode;
	mText = szText;
}

void	UIXCheckbox::OnPressed( void )
{
	mbIsChecked = !mbIsChecked;
	if ( mfnSelectedCallback )
	{
		mfnSelectedCallback( this );
	}
}

	
UIXRECT		UIXCheckbox::OnRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
UIXRECT		drawRect = GetActualRenderRect( displayRect );

	switch( mMode )
	{
	case STANDARD_CHECKBOX:
		pInterface->OutlineBox( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0xC0505050 );

		if ( mbIsChecked )
		{
			pInterface->Rect( 0, drawRect.x + 1, drawRect.y + 2, drawRect.w - 3, drawRect.h - 3, 0xd0808080 );
		}
		break;
	case POPUP_MENU_LIST:
		if ( mbIsChecked )
		{
		UIXRECT		checkedIconRect = drawRect;
			
			checkedIconRect.y += 2;
			checkedIconRect.h -= 8;
			checkedIconRect.w = checkedIconRect.h;
			checkedIconRect.x += 2;

			pInterface->Rect( 0, checkedIconRect.x, checkedIconRect.y, checkedIconRect.w, checkedIconRect.h, 0xC05050d0 );
		}


		if ( UIX::IsMouseHover( drawRect ))
		{
			pInterface->Rect( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0xb0303030 );	
		}
		pInterface->Text( 1, drawRect.x + 30, drawRect.y + 2, 0xD0A0A0A0, 3, mText.c_str() );
		break;
	}

	UIX::CheckForPress( this, drawRect, UIX_CHECKBOX, 0 );
	
	displayRect.h = 0;
	displayRect.y = GetDisplayRect().y + GetDisplayRect().h;		// displayRect.y returns the lowest point we drew to
	return displayRect;
}



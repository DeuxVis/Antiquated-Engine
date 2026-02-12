	

#include "StandardDef.h"
#include "InterfaceEx.h"
#include "../UI/UI.h"
#include "UIXCheckbox.h"


void	UIXCheckbox::Initialise( UIX_CHECKBOX_MODE mode, BOOL bIsChecked, const char* szText, fnSelectedCallback selectedFunc, uint32 ulSelectParam )
{
	mbIsChecked = bIsChecked;
	SetSelectedCallback( selectedFunc, ulSelectParam );
	mMode = mode;
	mText = szText;
}

bool	UIXCheckbox::OnSelected( int nButtonID, uint32 ulParam )
{
	mbIsChecked = !mbIsChecked;
	return( true );
}

	
UIXRECT		UIXCheckbox::OnRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
UIXRECT		drawRect = GetActualRenderRect( displayRect );

	switch( mMode )
	{
	case HILIGHT_CHECKBOX2:
		{
			uint32	ulCol = 0x90303040;
			uint32	ulTextCol = 0xD0A0A0A0;

			if (mbIsChecked)
			{
				ulCol = 0xC0C08010;
				ulTextCol = 0xF0F0c080;
			}
			pInterface->Rect(0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, ulCol);

			// TODO - Properly centre the title
			pInterface->Text(1, drawRect.x + 6, drawRect.y + 2, ulTextCol, 0, mText.c_str());
			if (UIX::IsMouseHover(drawRect))
			{
				pInterface->Rect(0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0x40c0b0a0);
			}
		}
		break;
	case HILIGHT_CHECKBOX:
		{
		uint32	ulCol = 0xd0202020;
		uint32	ulTextCol = 0xD0A0A0A0;

			if ( mbIsChecked )
			{
				ulCol = 0xe0383840;
				ulTextCol = 0xF0D0D0D0;
			}
			pInterface->Rect( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, ulCol );

			// TODO - Properly centre the title
			pInterface->Text( 1, drawRect.x + 6, drawRect.y + 1, ulTextCol, 0, mText.c_str() );
			if (UIX::IsMouseHover(drawRect))
			{
				pInterface->Rect(0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0x40c0b0a0);
			}
		}
		break;
	case ICON_CHECKBOX:
		{
		int		nIcon = mnOffIcon;
		uint32	ulCol = 0xd0202020;

			if ( mbIsChecked )
			{
				nIcon = mnOnIcon;
				ulCol = 0xe0383840;
			}
			pInterface->Rect( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, ulCol );
		
			UIX::DrawIcon( pInterface, nIcon, UIXRECT(drawRect.x + 1, drawRect.y + 1, drawRect.w - 2, drawRect.h - 2), 0xE0A0A0A0 );
		}
		break;
	case STANDARD_CHECKBOX:
		pInterface->OutlineBox( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0xC0505050 );

		if ( mbIsChecked )
		{
			pInterface->Rect( 0, drawRect.x + 1, drawRect.y + 2, drawRect.w - 3, drawRect.h - 3, 0xd0808080 );
		}
		break;
	case STANDARD_CHECKBOX_WITH_LABEL:
		{
		uint32		ulTextCol = 0xd0e0e0e0;
			pInterface->OutlineBox(0, drawRect.x, drawRect.y, drawRect.h, drawRect.h, 0xC0505050);

			if (mbIsChecked)
			{
				pInterface->Rect(0, drawRect.x + 1, drawRect.y + 2, drawRect.h - 3, drawRect.h - 3, 0xd0808080);
			}
			pInterface->Text(1, drawRect.x + drawRect.h + 2, drawRect.y + 3, ulTextCol, 3, mText.c_str());
		}
		break;
	case POPUP_MENU_LIST:
		{
		uint32		ulTextCol = 0xd0e0e0e0;
			if ( mbIsChecked )
			{
			UIXRECT		checkedIconRect = drawRect;
			
				checkedIconRect.y += 2;
				checkedIconRect.h -= 8;
				checkedIconRect.w = checkedIconRect.h;
				checkedIconRect.x += 2;

				// TODO - This sho
				pInterface->Rect( 0, checkedIconRect.x, checkedIconRect.y, checkedIconRect.w, checkedIconRect.h, 0xC05050d0 );
			}

			if ( HasSelectionCallback() )
			{
				if ( UIX::IsMouseHover( drawRect ))
				{
					pInterface->Rect( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0xb0303030 );	
					ulTextCol = 0xE0F0F0F0;
				}
			}
			else
			{
				ulTextCol = 0x60606060;
			}
			pInterface->Text( 1, drawRect.x + 30, drawRect.y + 2, ulTextCol, 0, mText.c_str() );
		}
		break;
	}

	if (UIX::IsMouseHover(drawRect))
	{
		UIX::CheckForPress(this, drawRect, UIX_CHECKBOX, 0);
	}
	
	displayRect.h = 0;
	displayRect.y = GetLocalPositionRect().y + GetLocalPositionRect().h + 1;		// displayRect.y returns the lowest point we drew to
	return displayRect;
}



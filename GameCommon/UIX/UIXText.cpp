	

#include "StandardDef.h"
#include "InterfaceEx.h"
#include "UIXText.h"


void	UIXText::Initialise( const char* szTitle, uint32 ulCol, int font, UIX_TEXT_FLAGS fontFlags )
{
	mText = szTitle;
	mulCol = ulCol;
	mFont = font;
	mFontFlags = fontFlags;
}
	
UIXRECT		UIXText::OnRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
UIXRECT		drawRect = GetActualRenderRect( displayRect );

	switch( mFontFlags )
	{
	case ALIGN_RIGHT:
		pInterface->TextRight( 1, drawRect.x + drawRect.w, drawRect.y, mulCol, mFont, mText.c_str() );
		break;
	default:
		pInterface->Text( 1, drawRect.x, drawRect.y, mulCol, mFont, mText.c_str() );
		break;
	}
	// TODO - Use mFontFlags if set
	// TODO - Make this TextLimitWidth

	displayRect.h = 0;//
	displayRect.y = GetDisplayRect().y + pInterface->GetStringHeight( mText.c_str(), mFont );		// displayRect.y returns the lowest point we drew to
	return displayRect;
}



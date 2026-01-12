
#include <stdarg.h>

#include "StandardDef.h"
#include "InterfaceEx.h"
#include "UIXTextBox.h"


void	UIXTextBox::Initialise( int mode, const char* szTitle )
{
	mText = szTitle;
	mMode = mode;
}

void	UIXTextBox::SetText( const char* text, ... )
{
char		acString[1024];
va_list		marker;
uint32*		pArgs;

	pArgs = (uint32*)( &text ) + 1;

	va_start( marker, text );     
	vsprintf( acString, text, marker );
	mText = acString;

}

UIXRECT		UIXTextBox::OnRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
UIXRECT		drawRect = GetActualRenderRect( displayRect );

	pInterface->OutlineBox( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0x40404040 );
	pInterface->Text( 1, drawRect.x + 4, drawRect.y + (drawRect.h-10)/2, 0xd0e0e0e0, 3, mText.c_str() );

	// TODO - Use mFontFlags if set
	// TODO - Make this TextLimitWidth

	displayRect.h = 0;//
	displayRect.y = GetDisplayRect().y + pInterface->GetStringHeight( mText.c_str(), 3 );		// displayRect.y returns the lowest point we drew to
	return displayRect;


}


#ifndef UIX_TEXT_BOX_H
#define UIX_TEXT_BOX_H

#include <string>
#include "UIX.h"

class UIXTextBox : public UIXObject
{
friend class UIX;
friend class UIXSlider;
public:
	void	SetText( const char* szText, ... );

protected:
	UIXTextBox( UIXObject* pxParent, uint32 uID, UIXRECT rect ) : UIXObject( pxParent, uID, rect ) {}

	void	Initialise( int mode, const char* szTitle );
	
	virtual UIXRECT		OnRender( InterfaceInstance* pInterface, UIXRECT pDisplayRect );
	static void		RegisterControlHandlers();
	void			OnPressed( uint32 ulParam );


private:
	static int		TextBoxKeyboardMessageHandlerStatic( int nResponseCode, const char* szInputText, void* pUserObj );
	
	int		OnKeyboardMessage( int nResponseCode, const char* szInputText );
	void	EndEdit();
	BOOL	OnSelect();

	std::string			mText;
	std::string			mPreEditText;
	int					mMode;
};






#endif
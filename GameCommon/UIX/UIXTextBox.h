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
	UIXTextBox( uint32 uID, UIXRECT rect ) : UIXObject( uID, rect ) {}

	void	Initialise( int mode, const char* szTitle );
	
	virtual UIXRECT		OnRender( InterfaceInstance* pInterface, UIXRECT pDisplayRect );

private:
	std::string			mText;
	int					mMode;
};






#endif
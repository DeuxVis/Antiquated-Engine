#ifndef UIX_BUTTON_H
#define UIX_BUTTON_H

#include <string>
#include "UIX.h"

class UIXButton : public UIXObject
{
friend class UIX;
protected:
	UIXButton( UIXObject* pxParent, uint32 uID, UIXRECT rect ) : UIXObject( pxParent, uID, rect ) {}

	void	Initialise( eUIXBUTTON_MODE mode, const char* szTitle, uint32 ulButtonID, uint32 ulButtonParam, BOOL bIsBlocking = TRUE, uint32 ulCol = 0xD0404040, int iconNum = 0 );
	
	virtual UIXRECT		OnRender( InterfaceInstance* pInstance, UIXRECT pDisplayRect );

private:
	std::string			mTitle;
	uint32				mulButtonID = 0;
	uint32				mulButtonParam = 0;
	eUIXBUTTON_MODE		mMode = UIXBUTTON_NORMAL;
	BOOL				mbIsBlocking = TRUE;
	uint32				mulCol = 0xd0404040;
	int					mIconNum = 0;
};






#endif
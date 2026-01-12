#ifndef UIX_PAGE_H
#define UIX_PAGE_H

#include <string>
#include "UIX.h"

class UIXPage : public UIXObject
{
public:
	UIXPage( uint32 uID, UIXRECT rect ) : UIXObject( uID, rect ) {}

	void		Initialise( const char* szTitle, BOOL bUseClipping );
	
	virtual UIXRECT		OnRender( InterfaceInstance* pInterface, UIXRECT rect );
	virtual void		OnPostChildrenRender( InterfaceInstance* pInterface );

private:
	UIXRECT		mPageRenderRect;
	BOOL		mbUseClipping = FALSE;
	std::string		mTitle;

};





#endif
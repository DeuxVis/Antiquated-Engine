#ifndef UIX_TEXT_H
#define UIX_TEXT_H

#include <string>
#include "UIX.h"

typedef	void(*fnDynamicTextUpdateCallback)( UIXObject* pxSourceObject, uint32 ulSelectParam, char* pcTextBuffer );

class UIXText : public UIXObject
{
friend class UIX;
public:
	void	SetDynamicTextFunction( fnDynamicTextUpdateCallback func ) { mDynamicTextFunc = func; }

protected:
	UIXText( UIXObject* pxParent, uint32 uID, UIXRECT rect ) : UIXObject( pxParent, uID, rect ) {}

	void	Initialise( const char* szTitle, uint32 ulCol, int font, UIX_TEXT_FLAGS fontFlags );
	
	virtual UIXRECT		OnRender( InterfaceInstance* pInstance, UIXRECT pDisplayRect );

private:
	std::string			mText;
	uint32				mulCol = 0xC0C0C0C0;
	int					mFont = 0;
	UIX_TEXT_FLAGS					mFontFlags = NONE;
	fnDynamicTextUpdateCallback		mDynamicTextFunc = NULL;

};






#endif
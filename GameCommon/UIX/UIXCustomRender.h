#ifndef UIX_CUSTOMRENDER_H
#define UIX_CUSTOMRENDER_H

#include <string>
#include "UIX.h"

class UIXCustomRender : public UIXObject
{
friend class UIX;
protected:
	UIXCustomRender( UIXObject* pxParent, uint32 uID, UIXRECT rect ) : UIXObject( pxParent, uID, rect ) {}

	void	Initialise( fnCustomRenderCallback renderFunc, uint32 ulUserParam );
	
	virtual UIXRECT		OnRender( InterfaceInstance* pInstance, UIXRECT pDisplayRect );

private:
	fnCustomRenderCallback		mfnRenderCallback;
	uint32			mulUserParam;

};






#endif
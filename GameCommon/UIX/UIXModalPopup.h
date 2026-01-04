#ifndef UIX_MODAL_POPUP_H
#define UIX_MODAL_POPUP_H

#include "UIX.h"

class UIXModalPopup : public UIXObject
{
friend class UIX;
public:

protected:
	UIXModalPopup( uint32 uID, UIXRECT rect ) : UIXObject( uID, rect ) {}

	void	Initialise( );

	virtual UIXRECT		OnRender( InterfaceInstance* pInterface, UIXRECT pDisplayRect );

	virtual int		GetSelectionPriorityLayer() { return( 2 ); }
private:

};












#endif
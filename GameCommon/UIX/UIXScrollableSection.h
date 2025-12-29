#ifndef UIX_SCROLLABLE_SECTION_H
#define UIX_SCROLLABLE_SECTION_H

#include <string>
#include "UIX.h"

class UIXScrollableSection : public UIXObject
{
friend class UIX;
public:

protected:
	UIXScrollableSection( uint32 uID, UIXRECT rect ) : UIXObject( uID, rect ) {}

	void	Initialise( );

	virtual UIXRECT		OnRender( InterfaceInstance* pInstance, UIXRECT pDisplayRect );

	void			HoldHandler( uint32 ulElementIndex, BOOL bIsHeld, BOOL bFirstPress );
	static void		HoldHandlerStatic( int nButtonID, uint32 ulParam, uint32 ulIndex, BOOL bIsHeld, BOOL bFirstPress );
	static void		RegisterControlHandlers();
	
	UIXRECT					GetLastRenderRect() const { return( mLastRender ); }
	virtual int				GetScrollPosition(); 
	virtual void		OnPostChildrenRender( InterfaceInstance* pInterface );
	virtual void		OnMouseWheel( float fOffset );

private:

	UIXRECT				mLastRender;
	UIXRECT				mPageRenderRect;

	int					mSectionInternalSize = 0;
	int					mScrollPosition = 0;
	
	int					mHoverOffsetY = 0;
	int					mPressPosScreenY;
	int					mPressPosOffsetY;
	float				mfBarScale;
	int					mFullBarHeight;

	BOOL				mbDidGrabScrollbar = FALSE;
	BOOL				mbIsUsingClippingCanvas = FALSE;

};






#endif
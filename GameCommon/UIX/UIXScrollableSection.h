#ifndef UIX_SCROLLABLE_SECTION_H
#define UIX_SCROLLABLE_SECTION_H

#include <string>
#include "UIX.h"

class UIXScrollbar : public UIXObject
{
public:
	UIXScrollbar(uint32 uID, UIXRECT rect) : UIXObject(uID, rect) {}

	void	RenderDirect(InterfaceInstance* pInterface, UIXRECT pDisplayRect, int nViewPageHeight, int nFullContentsHeight );

	void			HoldHandler(uint32 ulElementIndex, BOOL bIsHeld, BOOL bFirstPress);
	static void		HoldHandlerStatic(int nButtonID, uint32 ulParam, uint32 ulIndex, BOOL bIsHeld, BOOL bFirstPress);
	static void		RegisterControlHandlers();

	virtual int		GetScrollPosition();
	virtual void	OnMouseWheel(float fOffset);

private:

	UIXRECT				mScrollbarLastRender;
	int					mScrollPosition = 0;
	int					mScrollPositionScreen = 0;
	int					mHoverOffsetY = 0;
	int					mHoldMoveDistanceY = 0;
	int					mHoldStartScrollPosScreen = 0;
	int					mPressPosScreenY;
	int					mPressPosOffsetY;
	float				mfBarScale;
	float				mfHeightPerUnit;
	int					mFullBarHeight;

	BOOL				mbDidGrabScrollbar = FALSE;

};

class UIXScrollableSection : public UIXObject
{
friend class UIX;
public:

protected:
	UIXScrollableSection( uint32 uID, UIXRECT rect ) : UIXObject( uID, rect ) {}

	void	Initialise( );

	virtual UIXRECT		OnRender( InterfaceInstance* pInterface, UIXRECT pDisplayRect );

	void			HoldHandler( uint32 ulElementIndex, BOOL bIsHeld, BOOL bFirstPress );
	static void		HoldHandlerStatic( int nButtonID, uint32 ulParam, uint32 ulIndex, BOOL bIsHeld, BOOL bFirstPress );
	static void		RegisterControlHandlers();
	
	UIXRECT					GetLastRenderRect() const { return(mScrollbarLastRender); }
	virtual int				GetScrollPosition(); 
	virtual void		OnPostChildrenRender( InterfaceInstance* pInterface );
	virtual void		OnMouseWheel( float fOffset );

private:

	UIXRECT				mScrollbarLastRender;
	UIXRECT				mPageRenderRect;

	int					mSectionInternalSize = 0;
	BOOL				mbIsUsingClippingCanvas = FALSE;

	UIXScrollbar*		mpScrollbar = NULL;
};






#endif
#ifndef UIX_SCROLLABLE_SECTION_H
#define UIX_SCROLLABLE_SECTION_H

#include <string>
#include "UIX.h"

struct UIXScrollbarRestoreState
{
	int		mScrollPosition = 0;
	int		mScrollPositionScreen = 0;
	float	mfHeightPerUnit = 1.0f;
	int		mFullBarHeight = 100;
	int		mContentsHeight = 0;
};
	


class UIXScrollbar : public UIXObject
{
public:
	UIXScrollbar(UIXObject* pxParent, uint32 uID, UIXRECT rect ) : UIXObject( pxParent, uID, rect ) {}

	void	RenderDirect(InterfaceInstance* pInterface, UIXRECT pDisplayRect, int nViewPageHeight, int nFullContentsHeight );

	BOOL			HoldHandler(uint32 ulElementIndex, BOOL bIsHeld, BOOL bFirstPress);
	static BOOL		HoldHandlerStatic(int nButtonID, uint32 ulParam, uint32 ulIndex, BOOL bIsHeld, BOOL bFirstPress);
	static void		RegisterControlHandlers();

	void	StoreScrollState( UIXScrollbarRestoreState* pxOut );
	void	RestoreScrollState( const UIXScrollbarRestoreState* pxIn );

	void			SetScrollPosition(int value) { mScrollPosition = value; }
	virtual void	OnMouseWheel(float fOffset);
	virtual int		GetScrollPosition();

private:

	UIXRECT				mScrollbarLastRender;
	int					mScrollPosition = 0;
	int					mScrollPositionScreen = 0;
	int					mHoverOffsetY = 0;
	int					mHoldMoveDistanceY = 0;
	int					mHoldStartScrollPosScreen = 0;
	int					mPressPosScreenY = 0;
	int					mPressPosOffsetY = 0;
	float				mfBarScale = 1.0f;
	float				mfHeightPerUnit = 1.0f;
	int					mFullBarHeight = 100;

	BOOL				mbDidGrabScrollbar = FALSE;

};

class UIXScrollableSection : public UIXObject
{
friend class UIX;
public:

	void	StoreScrollState( UIXScrollbarRestoreState* pxOut );
	void	RestoreScrollState( const UIXScrollbarRestoreState* pxIn );

	virtual int				GetScrollPosition(); 

protected:
	UIXScrollableSection( UIXObject* pxParent, uint32 uID, UIXRECT rect ) : UIXObject( pxParent, uID, rect ) {}
	virtual ~UIXScrollableSection();

	void	Initialise( );

	virtual UIXRECT		OnRender( InterfaceInstance* pInterface, UIXRECT pDisplayRect );

	void			HoldHandler( uint32 ulElementIndex, BOOL bIsHeld, BOOL bFirstPress );
	static void		HoldHandlerStatic( int nButtonID, uint32 ulParam, uint32 ulIndex, BOOL bIsHeld, BOOL bFirstPress );
	static void		RegisterControlHandlers();
	
	UIXRECT					GetLastRenderRect() const { return(mScrollbarLastRender); }
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
#ifndef UIX_COLLAPSABLE_SECTION_H
#define UIX_COLLAPSABLE_SECTION_H

#include <string>
#include "UIX.h"

class UIXCollapsableSection : public UIXObject
{
friend class UIX;
public:
	void	UpdateUIStateData( UIStateData* pData );
	void	SetNotCollapsable() { mbCollapsable = FALSE; mbIsCollapsed = FALSE;}
protected:
	UIXCollapsableSection( UIXObject* pxParent, uint32 uID, UIXRECT rect ) : UIXObject( pxParent, uID, rect ) {}

	void	Initialise( UIXRECT headerRect, int mode, const char* szTitle, BOOL bStartCollapsed, int draggableType = 0 );
	void	ToggleCollapsed();

	virtual bool	OnSelected( int nButtonID, uint32 ulParam );
	virtual UIXRECT		OnRender( InterfaceInstance* pInstance, UIXRECT pDisplayRect );

	BOOL			HoldHandler( uint32 ulElementIndex, BOOL bIsHeld, BOOL bFirstPress );
	static BOOL		HoldHandlerStatic( int nButtonID, uint32 ulParam, uint32 ulIndex, BOOL bIsHeld, BOOL bFirstPress );
	static void		RegisterControlHandlers();
	
	UIXRECT					GetLastRenderRect() const { return( mLastHeaderRender ); }

private:
	virtual bool		ShouldDisplayChildren() { return !mbIsCollapsed; }

	std::string			mTitle;
	BOOL				mbIsCollapsed = FALSE;
	BOOL				mbCollapsable = TRUE;
	int					mMode = 0;

	UIXRECT				mLastHeaderRender;

	int					mDragItemType = 0;
	UIXRECT				mDragRectOriginal;
	UIXRECT				mDragRectMouseOriginal;
	UIXRECT				mHeaderRect;
};






#endif
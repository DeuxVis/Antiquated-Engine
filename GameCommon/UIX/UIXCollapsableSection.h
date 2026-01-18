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

	void	Initialise( int mode, const char* szTitle, BOOL bStartCollapsed, int draggableType = 0 );
	void	ToggleCollapsed();

	virtual void	OnSelected( int nButtonID, uint32 ulParam );
	virtual UIXRECT		OnRender( InterfaceInstance* pInstance, UIXRECT pDisplayRect );

	void			HoldHandler( uint32 ulElementIndex, BOOL bIsHeld, BOOL bFirstPress );
	static void		HoldHandlerStatic( int nButtonID, uint32 ulParam, uint32 ulIndex, BOOL bIsHeld, BOOL bFirstPress );
	static void		RegisterControlHandlers();
	
	UIXRECT					GetLastRenderRect() const { return( mLastRender ); }

private:
	virtual bool		ShouldDisplayChildren() { return !mbIsCollapsed; }

	std::string			mTitle;
	BOOL				mbIsCollapsed = FALSE;
	BOOL				mbCollapsable = TRUE;
	int					mMode = 0;

	UIXRECT				mLastRender;

	int					mDragItemType = 0;
	UIXRECT				mDragRectOriginal;
	UIXRECT				mDragRectMouseOriginal;

};






#endif
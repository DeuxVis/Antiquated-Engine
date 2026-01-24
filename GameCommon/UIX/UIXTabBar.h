#ifndef UIX_TAB_BAR_H
#define UIX_TAB_BAR_H

#include <string>
#include <vector>
#include "UIX.h"

class UIXTabBar;


class UIXTabBarEntry
{
friend class UIXTabBar;
public:

	const std::string&		GetText() const { return mTitle; }

	void		SetSelected( bool bFlag ) { mbSelected = bFlag; }
protected:
	UIXTabBarEntry( UIXTabBar* pParent, int index, const char* szTitle, uint32 ulUserParam );

	UIXRECT				Render( InterfaceInstance* pInstance, UIXRECT pDisplayRect );

private:
	std::string						mTitle;
	uint32							mulUserParam = 0;
	UIXTabBar*						mpParent = NULL;
	uint32							mIndex = 0;
	bool							mbSelected = false;
};

typedef	void(*fnTabBarSelectionChangedCallback)( UIXObject* pxSourceObject, uint32 ulElementParam );


class UIXTabBar : public UIXObject
{
friend class UIX;
friend class UIXTabBarEntry;
public:
	UIXTabBarEntry*		AddElement(	const char* szElementName, uint32 ulElementParam );

	void		SetSelectionChangedCallback( fnTabBarSelectionChangedCallback func ) { mSelectionChangedCallback = func; }

	const UIXTabBarEntry*		GetElementByListIndex( int index ) const;

protected:
	UIXTabBar( UIXObject* pxParent, uint32 uID, UIXRECT rect ) : UIXObject( pxParent, uID, rect ) {}

	void	Initialise( int mode );

	virtual UIXRECT		OnRender( InterfaceInstance* pInstance, UIXRECT pDisplayRect );
	virtual bool		OnSelected( int nButtonID, uint32 ulParam );
	virtual void		OnShutdown();

	static void		RegisterControlHandlers();

	BOOL			IsSelectable( ) { return( mSelectionChangedCallback != NULL ); }
private:

	BOOL				mbContentsDraggable = FALSE;
	int					mMode = 0;
	uint32				mulNextElementIndex = 0;
	std::vector<UIXTabBarEntry*>	mTabList;

	fnTabBarSelectionChangedCallback		mSelectionChangedCallback = NULL;
};






#endif
#ifndef UIX_LISTBOX_H
#define UIX_LISTBOX_H

#include <string>
#include <vector>
#include "UIX.h"

class UIXListBox;

class UIXListBoxEntry
{
friend class UIXListBox;
public:

	const std::string&		GetText() const { return mTitle; }

protected:
	UIXListBoxEntry( UIXListBox* pListbox, const char* szTitle, uint32 ulUserParam );

	UIXRECT					Render( InterfaceInstance* pInstance, UIXRECT pDisplayRect );
	UIXListBoxEntry*		AddSubElement( const char* szElementName, uint32 ulElementParam );

	UIXRECT					GetLastRenderRect() const { return( mLastRender ); }
private:
	std::string						mTitle;
	uint32							mulUserParam;
	UIXListBox*						mpListBox;
	std::vector<UIXListBoxEntry*>	mChildren;
	UIXRECT							mLastRender;
	uint32							mIndex;
};


class UIXListBox : public UIXObject
{
friend class UIX;
friend class UIXListBoxEntry;
public:
	UIXListBoxEntry*		AddElement(	const char* szElementName, uint32 ulElementParam );

	const UIXListBoxEntry*		GetElementByListIndex( int index ) const;
protected:
	UIXListBox( uint32 uID, UIXRECT rect ) : UIXObject( uID, rect ) {}

	void	Initialise( int mode, BOOL bContentsDraggable, int dragItemType );

	virtual UIXRECT		OnRender( InterfaceInstance* pInstance, UIXRECT pDisplayRect );

	void			HoldHandler( uint32 ulElementIndex, BOOL bIsHeld, BOOL bFirstPress );
	static void		HoldHandlerStatic( int nButtonID, uint32 ulParam, uint32 ulIndex, BOOL bIsHeld, BOOL bFirstPress );
	static void		RegisterControlHandlers();

	uint32			GetNextIndex( UIXListBoxEntry* pListBoxEntry ) { mListBoxEntriesFlatList.push_back( pListBoxEntry ); return( mulNextElementIndex++ ); }
private:

	BOOL				mbContentsDraggable = FALSE;
	int					mMode = 0;
	uint32				mulNextElementIndex = 0;
	std::vector<UIXListBoxEntry*>	mListBoxEntries;

	std::vector<UIXListBoxEntry*>	mListBoxEntriesFlatList;

	UIXListBoxEntry*	mpDragHeldEntry = NULL;
	int					mDragItemType = 0;
	UIXRECT				mDragRectOriginal;
	UIXRECT				mDragRectMouseOriginal;
	
};






#endif
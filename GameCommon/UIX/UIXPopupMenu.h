#ifndef UIX_POPUP_MENU_H
#define UIX_POPUP_MENU_H

#include <string>
#include <vector>

#include "UIX.h"

class UIXPopupMenuItem : public UIXObject
{
public:
	UIXPopupMenuItem( UIXObject* pxParent, uint32 uID, UIXRECT rect ) : UIXObject( pxParent, uID, rect ) {}

	void		Initialise( const char* szText, fnSelectedCallback selectedCallback = NULL, uint32 ulSelectParam = 0 );
	void		SetCheckbox( BOOL bIsCheckbox, BOOL bState ) { mbIsCheckbox = bIsCheckbox; mbIsChecked = bState; }
	void		SetHeader( BOOL bIsHeader ) { mbIsHeader = bIsHeader; }

	UIXPopupMenuItem*		AddHeaderMenuItem( const char* szMenuItemName );
	UIXPopupMenuItem*		AddMenuItem( const char* szMenuItemName, fnSelectedCallback selectedCallback = NULL, uint32 ulSelectParam = 0, BOOL bIsCheckbox = FALSE, BOOL bIsChecked = FALSE );
	
	const char* GetText() const { return mText.c_str(); }
		
	void			DoRender( InterfaceInstance* pInterface, UIXRECT rect );
	virtual bool	OnSelected( int nButtonID, uint32 ulParam );
	bool			IsExpanded() const { return mbIsExpanded; }
	BOOL			IsChecked() const { return mbIsChecked; }
	BOOL			IsCheckbox() const { return mbIsCheckbox; }
	BOOL			IsHeader() const { return mbIsHeader; }

	UIXRECT		GetLastRenderRect() const { return mLastRenderRect; }
	void		SetLastRenderRect(UIXRECT rect) { mLastRenderRect = rect; }

protected:
	virtual int			GetSelectionPriorityLayer() { return( 10 ); }

private:

	void			SetExpanded( bool bFlag ) { mbIsExpanded = bFlag; };

	std::string		mText;
	bool			mbIsExpanded = false;	
	BOOL			mbIsChecked = FALSE;
	BOOL			mbIsCheckbox = FALSE;
	BOOL			mbIndentForCheckbox = FALSE;
	BOOL			mbIsHeader = FALSE;
	UIXRECT			mLastRenderRect;
};


class UIXPopupMenu : public UIXObject
{
public:
	UIXPopupMenu( UIXObject* pxParent, uint32 uID, UIXRECT rect ) : UIXObject( pxParent, uID, rect ) {}

	void		Initialise( );
	
	UIXPopupMenuItem*		AddHeaderMenuItem( const char* szMenuItemName, ... );
	UIXPopupMenuItem*		AddMenuItem( const char* szMenuItemName, fnSelectedCallback selectedCallback = NULL, uint32 ulSelectParam = 0, BOOL bIsCheckbox = FALSE, BOOL bIsChecked = FALSE  );

	virtual UIXRECT		OnRender( InterfaceInstance* pInterface, UIXRECT rect );
	virtual void		OnPostRender( InterfaceInstance* pInterface, UIXRECT rect );
private:
	virtual int		GetSelectionPriorityLayer() { return( 10 ); }
	virtual bool		ShouldDisplayChildren() { return false; }

};


#endif // CONTROL_PANEL_INTERFACES_GENERIC_POPUP_MENU_H
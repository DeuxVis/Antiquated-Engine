#ifndef UIX_H
#define UIX_H

#include "StandardDef.h"
#include <vector>
#include <map>

class InterfaceInstance;
class UIXObject;
class UIXPage;
class UIXButton;
class UIXListBox;
class UIXSlider;
class UIXTextBox;
class UIXText;
class UIXCollapsableSection;
class UIXScrollableSection;
class UIXDropdown;
class UIXShape;
class UIXCustomRender;

enum 
{
	UIX_RESERVED_BUTTONIDS = 0x550000,
	UIX_COLLAPSABLE_SECTION_HEADER,
	UIX_SLIDER_BAR,
	UIX_LISTBOX,
	UIX_SLIDER_BAR_MINRANGE,
	UIX_SLIDER_BAR_MAXRANGE,
	UIX_SCROLLABLE_SECTION_SCROLLBAR,
	UIX_DROPDOWN_HEADER,
	UIX_DROPDOWN_ENTRY,
};



enum UIX_TEXT_FLAGS
{
	NONE,
	ALIGN_RIGHT,
};

enum UIX_SLIDER_MODE
{
	VALUE,
	ANGLE,
	VALUERANGE,
};

enum UIX_VALUE_CALLBACK_FLAGS
{
	EMPTY,
	IS_BEING_MODIFIED,
};

typedef	float(*fnValueUpdateCallback)( uint32 ulUIXObjectID, float fUIXValue, float fUIXValue2, uint32 ulUserParam, BOOL bIsUIHeld );
typedef	void(*fnDragReceiveCallback)( UIXObject* pxSourceObject, uint32 ulDragParam, UIXObject* pxDestObject, uint32 ulDragDestParam );

class UIStateData
{
public:
	void			OnUpdate( float fDelta );

	BOOL			mbTabOpened = TRUE;
	// TODO - Need to do something to handle this pointer reference better
	UIXObject*		mpAssociatedUIXObj = NULL;
};

struct UIXRECT
{
	UIXRECT() { x = 0; y = 0; w = 0; h = 0; }
	UIXRECT( int vx, int vy, int vw, int vh) { x = vx; y = vy; w = vw; h = vh; }

	void	ConsumeVertical( int amount ) { y += amount; h -= amount; }
	void	ConsumeHorizontal( int amount ) { x += amount; w -= amount; }

	int x;
	int y;
	int w;
	int h;
};

typedef	UIXRECT(*fnCustomRenderCallback)( InterfaceInstance* pInterface, UIXRECT rect, uint32 ulUserParam1, uint32 ulUserParam2 );

class UIXObject
{
friend class UIX;
public:
	uint32			GetID() { return( mulID ); }

	void		SetDragReceiveCallback( int dragType, fnDragReceiveCallback func, uint32 ulDestParam );

	void*				GetUserObject() { return mpUserObject; }
	void				SetUserObject( void* pObject ) { mpUserObject = pObject; }

	virtual void		UpdateUIStateData( UIStateData* pData ) {}
protected:
	UIXObject( uint32 uID, UIXRECT rect );

	virtual void		OnUpdate( float delta ) {}
	virtual UIXRECT		OnRender( InterfaceInstance* pInterface, UIXRECT rect ) { rect.h = 0; return rect; }
	virtual void		OnShutdown() {}
	virtual void		OnPostChildrenRender( InterfaceInstance* pInterface ) { }
	virtual void		OnMouseWheel( float fAmount ) {}

	void		Update( float delta );
	UIXRECT		Render( InterfaceInstance* pInterface, UIXRECT rect );
	void		Shutdown();

	BOOL		CheckDragHoverRegion( UIXRECT dragReceiveRegion );

	virtual void		OnHoverDragItem( int dragType ) {}
	virtual void		OnReceiveDragItem( int dragType, UIXObject* pxSourceObject, uint32 ulDragParam );
	bool				CanReceiveDragItem( int dragType ) { return( mDragMap[dragType] ); }
	
	UIXRECT		GetDisplayRect() { return( mDisplayRect ); }
	UIXRECT		GetActualRenderRect( UIXRECT parentRect );
	int			GetChildContentsHeight() { return mChildContentsHeight; }
	virtual int			GetScrollPosition() { return( 0 ); }
private:
	virtual bool		ShouldDisplayChildren() { return true; }

	std::vector<UIXObject*>			mContainsList;
	std::map<int, fnDragReceiveCallback>		mDragMap;
	std::map<int, uint32>						mDragMapParams;
	
	uint32			mulID;
	UIXRECT			mDisplayRect;
	void*			mpUserObject = NULL;
	int				mChildContentsHeight = 0;
};


//-------------------------------------------------

class UIX
{
friend class UIXObject;
public:
	static void		Initialise( int mode );
	static void		Update( float delta );
	static void		Render( InterfaceInstance* pInterface );
	static void		Shutdown();
	static void		Reset();
	static void		ButtonPressHandler( int nButtonID, uint32 ulParam, uint32 ulIDParam );
	static void		SliderHoldHandler( int nButtonID, uint32 ulParam, uint32 ulIndex, BOOL bIsHeld, BOOL bFirstPress );
	static void		OnMouseWheel( float fOffset );

	static UIXObject*					AddPage( UIXRECT xRect, const char* szTitle, BOOL bUseClipping = FALSE );
	static UIXCollapsableSection*		AddCollapsableSection( UIXObject* pxContainer, UIXRECT xRect, int mode, const char* szTitle, BOOL bStartCollapsed, int draggableType = 0 );
	static UIXScrollableSection*		AddScrollableSection( UIXObject* pxContainer, UIXRECT xRect );
	static UIXButton*					AddButton( UIXObject* pxContainer, UIXRECT xRect, int mode, const char* szTitle, uint32 ulButtonID, uint32 ulButtonParam, BOOL IsBlocking = TRUE );
	static UIXTextBox*					AddTextBox( UIXObject* pxContainer, UIXRECT xRect );
	static UIXListBox*					AddListBox( UIXObject* pxContainer, UIXRECT xRect, int mode = 0, BOOL bContentsDraggable = FALSE, int dragItemType = 0 );
	static UIXSlider*					AddSlider( UIXObject* pxContainer, UIXRECT xRect, UIX_SLIDER_MODE mode = VALUE, uint32 ulUserParam = 0, float fMin = 0.0f, float fMax = 1.0f, float fInitial = 0.0f, float fMinStep = 0.1f  );
	static UIXDropdown*					AddDropdown( UIXObject* pxContainer, UIXRECT xRect );
	static UIXText*						AddText( UIXObject* pxContainer, UIXRECT xRect, uint32 ulCol = 0xc0c0c0c0, int font = 0, UIX_TEXT_FLAGS fontFlags = NONE,  const char* szTitle = NULL, ... );
	static UIXShape*					AddShape( UIXObject* pxContainer, UIXRECT xRect, int mode = 0, BOOL bBlocks = FALSE, uint32 ulCol1 = 0xC0C0C0C0, uint32 ulCol2 = 0xC0C0C0C0, uint32 ulButtonID = 0, uint32 ulButtonParam = 0 );
	static UIXCustomRender*				AddCustomRender( UIXObject* pxContainer, UIXRECT xRect, fnCustomRenderCallback renderFunc, uint32 ulUserParam1 = 0, uint32 ulUserParam2 = 0  );

	static void							DeleteObject( UIXObject* pObject );
	static UIXObject*					GetUIXObjectByID( uint32 ulObjectID ) { return( msComponentIDMap[ulObjectID]);}

	static void							SetDragItemType( int type, UIXObject* pxFromObject, uint32 param ) { msDragItemType = type; mspDragSource = pxFromObject; msDragSourceParam = param; }
	static int							GetDragItemType() { return msDragItemType; }
	static uint32						GetDragItemSourceParam() { return msDragSourceParam; }
	static void							HoverAcceptDragItem( UIXObject* pxObject ) { mspDragDestinationHover = pxObject; }
	static void							EndDragItemType( int type );

	static void							SetMousewheelHoverObject( UIXObject* pObject ) { mspMousewheelHoverObject = pObject; }
protected:
	static std::map<uint32, UIXObject*>		msComponentIDMap;
private:
	static uint32						msulNextObjectID;
	static std::vector<UIXObject*>		msPagesList;
	static int							msDragItemType;
	static UIXObject*					mspDragDestinationHover;
	static UIXObject*					mspDragSource;
	static UIXObject*					mspMousewheelHoverObject;
	static uint32						msDragSourceParam ;
};





#endif
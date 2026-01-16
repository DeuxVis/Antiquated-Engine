#ifndef UIX_H
#define UIX_H

#include "StandardDef.h"
#include <vector>
#include <string>
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
class UIXCheckbox;
class UIXModalPopup;

#define		MAX_NUM_UIX_ICONS		32

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
	UIX_CHECKBOX,
	UIX_LISTBOX_SELECT,
	UIX_SCROLLBAR,
	UIX_TEXTBOX,
};

enum eUIXBUTTON_MODE
{
	UIXBUTTON_NORMAL,
	UIXBUTTON_PLAIN_RECT,
	UIXBUTTON_TEXT_WITH_SETTINGS,
	UIXBUTTON_RECT_ICON,
	UIXBUTTON_IMAGE,
};

enum UIX_TEXT_FLAGS
{
	NONE = 0,
	ALIGN_RIGHT = 0x1,
	BOLD = 0x2,
	ALIGN_CENTRE = 0x4,
	
};

enum UIX_SLIDER_MODE
{
	VALUE,
	ANGLE,
	VALUERANGE,
	VERTICAL_VALUE,
	SLIDER_PLUSMINUS_VALUE,
};

enum UIX_CHECKBOX_MODE
{
	STANDARD_CHECKBOX,
	POPUP_MENU_LIST,
	ICON_CHECKBOX,
	HILIGHT_CHECKBOX,
	HILIGHT_CHECKBOX2,
};

enum UIX_VALUE_CALLBACK_FLAGS
{
	EMPTY,
	IS_BEING_MODIFIED,
};

typedef	float(*fnValueUpdateCallback)( uint32 ulUIXObjectID, float fUIXValue, float fUIXValue2, uint32 ulUserParam, BOOL bIsUIHeld );
typedef	void(*fnDragReceiveCallback)( UIXObject* pxSourceObject, uint32 ulDragParam, UIXObject* pxDestObject, uint32 ulDragDestParam );
typedef	void(*fnSelectedCallback)( UIXObject* pxSourceObject );

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

typedef	UIXRECT(*fnCustomRenderCallback)( UIXObject* pObj, InterfaceInstance* pInterface, UIXRECT rect, uint32 ulUserParam );

class UIXObject
{
friend class UIX;
public:
	~UIXObject();

	uint32			GetID() { return( mulID ); }

	void		SetDragReceiveCallback( int dragType, fnDragReceiveCallback func, uint32 ulDestParam );

	void*				GetUserObject() { return mpUserObject; }
	void				SetUserObject( void* pObject ) { mpUserObject = pObject; }

	BOOL				DoesHaveUserParamEx( const char* szKey ) { return( (mUserParamExList.find(szKey) != mUserParamExList.end()) ); }
	int					GetUserParamEx( const char* szKey ) { return( mUserParamExList[szKey] ); }
	void				SetUserParamEx( const char* szKey, int nValue ) { mUserParamExList[szKey] = nValue; }

	virtual void		UpdateUIStateData( UIStateData* pData ) {}
	virtual float		OnValueChange( UIXObject* pxSourceObj, float fNewValue ) { return( fNewValue ); }
protected:
	UIXObject( UIXObject* pParent, uint32 uID, UIXRECT rect );

	virtual void		OnUpdate( float delta ) {}
	virtual UIXRECT		OnRender( InterfaceInstance* pInterface, UIXRECT rect ) { rect.h = 0; return rect; }
	virtual void		OnPostRender( InterfaceInstance* pInterface, UIXRECT rect ) {}
	virtual void		OnShutdown() {}
	virtual void		OnPostChildrenRender( InterfaceInstance* pInterface ) { }
	virtual void		OnMouseWheel( float fAmount ) {}

	void		Update( float delta );
	UIXRECT		Render( InterfaceInstance* pInterface, UIXRECT rect );
	void		PostRender(InterfaceInstance* pInterface);	
	void		Shutdown();
	void		KeyUp(int keyCode);

	BOOL		CheckDragHoverRegion( UIXRECT dragReceiveRegion );

	virtual void		OnHoverDragItem( int dragType ) {}
	virtual void		OnReceiveDragItem( int dragType, UIXObject* pxSourceObject, uint32 ulDragParam );
	bool				CanReceiveDragItem( int dragType ) { return( mDragMap[dragType] ); }
	
	UIXRECT		GetDisplayRect() { return( mDisplayRect ); }
	UIXRECT		GetActualRenderRect( UIXRECT parentRect );
	int			GetChildContentsHeight() { return mChildContentsHeight; }
	virtual int			GetScrollPosition() { return( 0 ); }
	virtual int		GetSelectionPriorityLayer() { return( 0 ); }

	virtual void			OnEscape() {}
	UIXObject* GetParent() const { return(mpParent); }
private:
	virtual bool		ShouldDisplayChildren() { return true; }

	std::vector<UIXObject*>			mContainsList;
	std::map<std::string, int>		mUserParamExList;
	std::map<int, fnDragReceiveCallback>		mDragMap;
	std::map<int, uint32>						mDragMapParams;
	
	uint32			mulID;
	UIXRECT			mDisplayRect;
	void*			mpUserObject = NULL;
	int				mChildContentsHeight = 0;
	UIXRECT			mLastRenderDisplayRect;
	UIXObject*		mpParent;
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
	static void		OnKeyUp( int keyCode );

	static void			SetModalObject( UIXObject* pObject ) { mspModalObject = pObject; }
	static UIXObject*	GetModalObject() { return( mspModalObject ); }

	static UIXObject*					AddPage( UIXRECT rect, const char* szTitle, BOOL bUseClipping = FALSE );
	static UIXObject*					AddSubPage( UIXObject* pxContainer, UIXRECT rect, const char* szTitle, BOOL bUseClipping = FALSE );
	static UIXCollapsableSection*		AddCollapsableSection( UIXObject* pxContainer, UIXRECT rect, int mode, const char* szTitle, BOOL bStartCollapsed, int draggableType = 0 );
	static UIXScrollableSection*		AddScrollableSection( UIXObject* pxContainer, UIXRECT rect );
	static UIXButton*					AddButton( UIXObject* pxContainer, UIXRECT rect, eUIXBUTTON_MODE mode, const char* szTitle, uint32 ulButtonID, uint32 ulButtonParam, BOOL IsBlocking = TRUE, uint32 ulCol = 0xD0404040, int iconNum = 0 );
	static UIXTextBox*					AddTextBox( UIXObject* pxContainer, UIXRECT rect, int mode, const char* szDefaultText );
	static UIXListBox*					AddListBox( UIXObject* pxContainer, UIXRECT rect, int mode = 0, BOOL bContentsDraggable = FALSE, int dragItemType = 0 );
	static UIXSlider*					AddSlider( UIXObject* pxContainer, UIXRECT rect, UIX_SLIDER_MODE mode = VALUE, uint32 ulUserParam = 0, float fMin = 0.0f, float fMax = 1.0f, float fInitial = 0.0f, float fMinStep = 0.1f, const char* szText = NULL );
	static UIXDropdown*					AddDropdown( UIXObject* pxContainer, UIXRECT rect );
	static UIXText*						AddText( UIXObject* pxContainer, UIXRECT rect, uint32 ulCol = 0xc0c0c0c0, int font = 0, UIX_TEXT_FLAGS fontFlags = NONE,  const char* szTitle = NULL, ... );
	static UIXShape*					AddShape( UIXObject* pxContainer, UIXRECT rect, int mode = 0, BOOL bBlocks = FALSE, uint32 ulCol1 = 0xC0C0C0C0, uint32 ulCol2 = 0xC0C0C0C0, uint32 ulButtonID = 0, uint32 ulButtonParam = 0 );
	static UIXCustomRender*				AddCustomRender( UIXObject* pxContainer, UIXRECT rect, fnCustomRenderCallback renderFunc, uint32 ulUserParam = 0  );
	static UIXCheckbox*					AddCheckbox( UIXObject* pxContainer, UIXRECT rect, UIX_CHECKBOX_MODE mode, BOOL bIsChecked, const char* szText, fnSelectedCallback selectedFunc );
	static UIXModalPopup*				AddModalPopup( UIXObject* pxContainer, UIXRECT rect );

	static void							DeleteObject( UIXObject* pObject );
	static UIXObject*					GetUIXObjectByID( uint32 ulObjectID ) { return( msComponentIDMap[ulObjectID]);}

	static void							SetDragItemType( int type, UIXObject* pxFromObject, uint32 param ) { msDragItemType = type; mspDragSource = pxFromObject; msDragSourceParam = param; }
	static int							GetDragItemType() { return msDragItemType; }
	static uint32						GetDragItemSourceParam() { return msDragSourceParam; }
	static void							HoverAcceptDragItem( UIXObject* pxObject ) { mspDragDestinationHover = pxObject; }
	static void							EndDragItemType( int type );

	static void							SetMousewheelHoverObject(UIXObject* pObject);

	static void							SetTextEditFocus( UIXObject* pObject ) { mspTextEditFocusObject = pObject; }
	static UIXObject*					GetTextEditFocus() { return( mspTextEditFocusObject ); }

	static BOOL							IsMouseHover( UIXRECT rect );

	static BOOL							CheckForPress( UIXObject* pxObject, UIXRECT rect, uint32 ulButtonID, uint32 ulButtonParam );
	static void							IncrementSelectionPriority();
	static void							DecrementSelectionPriority();

	static void							DrawIcon( InterfaceInstance* pInterface, int iconNum, UIXRECT rect, uint32 ulCol );
	static void							LoadIcon( InterfaceInstance* pInterface, int iconNum, const char* szFilename );
	static void							LoadIconSheet( InterfaceInstance* pInterface, int sheetNum, const char* szFilename );

	static uint32						GetNextObjectID();
protected:

	static std::map<uint32, UIXObject*>		msComponentIDMap;
private:
	static uint32						msulNextObjectID;
	static std::vector<UIXObject*>		msPagesList;
	static int							msDragItemType;
	static UIXObject*					mspDragDestinationHover;
	static UIXObject*					mspDragSource;
	static UIXObject*					mspMousewheelHoverObject;
	static UIXObject*					mspTextEditFocusObject;
	static uint32						msDragSourceParam ;
	static UIXObject*					mspModalObject;
	static int							msSelectionPriority;
	static int							msPressedSelectionPriority;
	static int							msMouseWheelHoverPriority;
	static int					mshUIXIconsList[MAX_NUM_UIX_ICONS];
	static int					mshUIXIconOverlays[MAX_NUM_UIX_ICONS];
};





#endif
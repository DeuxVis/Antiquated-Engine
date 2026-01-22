
#include <stdarg.h>
#include "StandardDef.h"

#include "InterfaceEx.h"
#include "../UI/UI.h"
#include "UIX.h"
#include "UIXButton.h"
#include "UIXDropdown.h"
#include "UIXListBox.h"
#include "UIXPage.h"
#include "UIXSlider.h"
#include "UIXDropdown.h"
#include "UIXShape.h"
#include "UIXCheckbox.h"
#include "UIXTextBox.h"
#include "UIXText.h"
#include "UIXCustomRender.h"
#include "UIXCollapsableSection.h"
#include "UIXScrollableSection.h"
#include "UIXMenu.h"
#include "UIXModalPopup.h"

uint32						UIX::msulNextObjectID = 2001;
std::vector<UIXObject*>		UIX::msPagesList;
std::map<uint32, UIXObject*>	UIX::msComponentIDMap;
int							UIX::msDragItemType = 0;
UIXObject*					UIX::mspDragDestinationHover = NULL;
UIXObject*					UIX::mspDragSource = NULL;	
UIXObject*					UIX::mspModalObject = NULL;
UIXObject*					UIX::mspMousewheelHoverObject = NULL;
UIXObject*					UIX::mspTextEditFocusObject = NULL;
int							UIX::msSelectionPriority = 0;
int							UIX::msPressedSelectionPriority = 0;
int							UIX::msMouseWheelHoverPriority = 0;
int							UIX::mshUIXIconOverlays[MAX_NUM_UIX_ICONS] = { NOTFOUND };
int							UIX::mshUIXIconsList[MAX_NUM_UIX_ICONS] = { NOTFOUND };


uint32						UIX::msDragSourceParam = 0;
//--------------------------------------------------------------------------------------
UIXObject::UIXObject( UIXObject* pParent, uint32 uID, UIXRECT rect )
{
	mpParent = pParent;
	mulID = uID;
	mDisplayRect = rect;
	UIX::msComponentIDMap.emplace( uID, this );

}

UIXObject::~UIXObject()
{
	// todo - Get rid of this by using smart(er) pointers for things like the mousewheel hover
	if ( UIX::mspMousewheelHoverObject == this ) UIX::mspMousewheelHoverObject = NULL;

	// nOTE THIS ALWAYS SHOULD GET DONE THROUGH uix::dELETEoBJECT
}

void	UIXObject::SelectObject( int nButtonID, uint32 ulParam )
{
	// HACK: If OnSelected returns false it indicates it may have deleted the object we were using
	//  so we don't do any further callbacks now. This is unpleasant - needs refactoring
	if ( OnSelected( nButtonID, ulParam ) )
	{
		if ( mfnSelectedCallback )
		{
			mfnSelectedCallback( this, mulSelectParam );
		}
	}
}

void	UIXObject::KeyUp(int keyCode)
{
	switch (keyCode)
	{
	case KEY_ESCAPE:
		OnEscape();
		break;
	default:
		break;
	}

	for (UIXObject* pContainedObject : mContainsList)
	{
		pContainedObject->KeyUp(keyCode);
	}
}

void	UIXObject::Update( float delta )
{
	OnUpdate( delta );
	for ( UIXObject* pContainedObject : mContainsList )
	{
		pContainedObject->Update( delta );
	}
}

void	UIXObject::CloseAllMenus()
{
	OnCloseAllMenus();
	for ( UIXObject* pContainedObject : mContainsList )
	{
		pContainedObject->CloseAllMenus();
	}
}

UIXRECT		UIXObject::GetActualRenderRect( UIXRECT parentRect )
{
UIXRECT		renderRect = GetDisplayRect();		// This is our local position, relative to 0,0 within whatever container we're in

	if ( renderRect.x < 0 )
	{
		renderRect.x = parentRect.w + renderRect.x;	
	}
	else
	{
		renderRect.x += parentRect.x;
	}
	renderRect.y += parentRect.y;

	if ( renderRect.w < 0 )
	{
		renderRect.w = (parentRect.w + renderRect.w) - renderRect.x;
	}

	if ( renderRect.x + renderRect.w > parentRect.x + parentRect.w )
	{
		renderRect.w = (parentRect.w - parentRect.x) - renderRect.x;
	}
	return renderRect;
}

void	UIXObject::PostRender( InterfaceInstance* pInterface )
{
	UIX::msSelectionPriority += GetSelectionPriorityLayer();
	OnPostRender( pInterface, mLastRenderDisplayRect );

	if ( !mContainsList.empty() && ShouldDisplayChildren() )
	{
		for ( UIXObject* pContainedObject : mContainsList )
		{
			pContainedObject->PostRender( pInterface );
		}
	}
	UIX::msSelectionPriority -= GetSelectionPriorityLayer();
}


UIXRECT	UIXObject::Render( InterfaceInstance* pInterface, UIXRECT displayRect )
{
UIXRECT		xUsedRect = displayRect;
UIXRECT		xRect;
UIXRECT		xMaxRect;

	xUsedRect.y = 0;
	xUsedRect.h = 0;

	mLastRenderDisplayRect = displayRect;

	UIX::msSelectionPriority += GetSelectionPriorityLayer();

	xRect = OnRender( pInterface, displayRect );

	xUsedRect.w = xRect.w;		// w is reduced by things like scrollbars occupying space within the page
	xUsedRect.h = xRect.h;
	xUsedRect.y = xRect.y;
	
	if ( !mContainsList.empty() && ShouldDisplayChildren() )
	{
	UIXRECT		xChildDisplayRect = displayRect;
	int			nCursRelY = 0;
		
		xChildDisplayRect.w = xUsedRect.w;
		xChildDisplayRect.y += xUsedRect.h;
		xChildDisplayRect.h -= xUsedRect.h;
		xChildDisplayRect.y -= GetScrollPosition();

		for ( UIXObject* pContainedObject : mContainsList )
		{
			xRect = pContainedObject->Render( pInterface, xChildDisplayRect );

			if ( xRect.y + nCursRelY > xMaxRect.y )
			{
				xMaxRect.y = xRect.y + nCursRelY;		
			}
			xUsedRect.h += xRect.h;
			nCursRelY += xRect.h;
			xChildDisplayRect.y += xRect.h;
			xChildDisplayRect.h -= xRect.h;
		}

		OnPostChildrenRender( pInterface );

		int presetChildBlockSize = GetDisplayRect().h;

		xUsedRect.y += xMaxRect.y;
		if ( xUsedRect.y > xUsedRect.h )
		{
			xUsedRect.h = xUsedRect.y;
		}
		// MaxRect contains the lowest point that was drawn to by children including those bits that dont change the cursor
		// and will tell us if we need to expand the box
		if ( xMaxRect.y > presetChildBlockSize )
		{
			if ( xMaxRect.y > xUsedRect.h )
			{
				xUsedRect.h = xMaxRect.y;
			}
		}
		// The usedRect effectively tracks the cursor line position, not always meaningful as the child UI may never adjust it
		else if ( xUsedRect.h < presetChildBlockSize )
		{
//			xUsedRect.h = fPresetChildBlockSize;
		}
		mChildContentsHeight = xUsedRect.h;
	}
	UIX::msSelectionPriority -= GetSelectionPriorityLayer();
	return( xUsedRect );
}

BOOL	UIXObject::CheckDragHoverRegion( UIXRECT dragReceiveRegion )
{
int	type = UIX::GetDragItemType();

	if ( ( type != 0 ) &&
		 ( CanReceiveDragItem(type) ) )
	{
		if ( UIHoverItem( dragReceiveRegion.x, dragReceiveRegion.y, dragReceiveRegion.w, dragReceiveRegion.h) )
		{
			OnHoverDragItem(type);
			return( TRUE );
		}
	}
	return( FALSE );
}

void	UIXObject::Shutdown()
{
	OnShutdown();

	for ( UIXObject* pContainedObject : mContainsList )
	{
		pContainedObject->Shutdown();
		UIX::DeleteObject( pContainedObject );
	}
	mContainsList.clear();
}

void		UIXObject::OnReceiveDragItem( int dragType, UIXObject* pxSourceObject, uint32 ulDragParam )
{
	if ( mDragMap[dragType] )
	{
		mDragMap[dragType](pxSourceObject, ulDragParam, this, mDragMapParams[dragType] );
	}
}


void		UIXObject::SetDragReceiveCallback( int dragType, fnDragReceiveCallback func, uint32 ulDragDestParam )
{
	mDragMap[dragType] = func;
	mDragMapParams[dragType] = ulDragDestParam;
}

//--------------------------------------------------------------------------------------------------
// IDParam must always be UIXObject's unique ID ( obj->GetID() )  to use this handler
//
void		UIX::ButtonPressHandler( int nButtonID, uint32 ulParam, uint32 ulIDParam )
{
	auto it = msComponentIDMap.find( ulIDParam );
	if ( it != msComponentIDMap.end() )
	{
	UIXObject* pObject = it->second;

		pObject->SelectObject( nButtonID, ulParam );
	}
}
	
void	UIX::OnKeyUp(int keyCode)
{
	switch (keyCode)
	{
	case KEY_ESCAPE:
		for (UIXObject* pxObjects : msPagesList)
		{
			pxObjects->KeyUp( keyCode );
		}
		break;
	default:
		break;
	}

}

void		UIX::OnMouseWheel( float fOffset )
{
	if ( mspMousewheelHoverObject )
	{
		mspMousewheelHoverObject->OnMouseWheel( fOffset );
	}
}

void		UIX::SliderHoldHandler( int nButtonID, uint32 ulIndex, uint32 ulIDParam, BOOL bIsHeld, BOOL bFirstPress  )
{
UIXSlider*		pSlider;

	switch( nButtonID )
	{
	case UIX_SLIDER_BAR_MAXRANGE:
		pSlider = (UIXSlider*)msComponentIDMap[ulIDParam];
		if ( pSlider )
		{
			pSlider->OnMaxRangeHeldUpdate( bIsHeld, bFirstPress );
		}
		break;
	case UIX_SLIDER_BAR_MINRANGE:
		pSlider = (UIXSlider*)msComponentIDMap[ulIDParam];
		if ( pSlider )
		{
			pSlider->OnMinRangeHeldUpdate( bIsHeld, bFirstPress );
		}
		break;
	case UIX_SLIDER_BAR:
		pSlider = (UIXSlider*)msComponentIDMap[ulIDParam];
		if ( pSlider )
		{
			pSlider->OnHeldUpdate( bIsHeld, bFirstPress );
		}
		break;
	default:
		break;
	}
}

void		UIX::SetMousewheelHoverObject(UIXObject* pObject)
{
	if (msSelectionPriority >= msMouseWheelHoverPriority)
	{
		mspMousewheelHoverObject = pObject;
		msMouseWheelHoverPriority = msSelectionPriority;
	}


}

BOOL		UIX::CheckForRightButtonPress( UIXObject* pxObject, UIXRECT rect, uint32 ulButtonID, uint32 ulButtonParam )
{
	if ( msSelectionPriority >= msPressedSelectionPriority )
	{
		if ( UIIsRightPressed( rect.x, rect.y, rect.w, rect.h ) == TRUE )
		{
			UIRightPressIDSet( ulButtonID, ulButtonParam, pxObject->GetID() );
			msPressedSelectionPriority = msSelectionPriority;
		}
		return( IsMouseHover( rect ) );
	}
	return( FALSE );
}

BOOL		UIX::CheckForPress( UIXObject* pxObject, UIXRECT rect, uint32 ulButtonID, uint32 ulButtonParam )
{
	if ( msSelectionPriority >= msPressedSelectionPriority )
	{
		if ( UIIsPressed( rect.x, rect.y, rect.w, rect.h ) == TRUE )
		{
			UIPressIDSet( ulButtonID, ulButtonParam, pxObject->GetID() );
			msPressedSelectionPriority = msSelectionPriority;
		}

		return( IsMouseHover( rect ) );
	}
	return( FALSE );
}

void		UIX::Initialise( int mode )
{
	UIRegisterButtonPressHandler( UIX_COLLAPSABLE_SECTION_HEADER, ButtonPressHandler );
	UIRegisterButtonPressHandler( UIX_DROPDOWN_HEADER, ButtonPressHandler );
	UIRegisterButtonPressHandler( UIX_DROPDOWN_ENTRY, ButtonPressHandler );
	UIRegisterButtonPressHandler( UIX_CHECKBOX, ButtonPressHandler );
	UIRegisterButtonPressHandler( UIX_LISTBOX_SELECT, ButtonPressHandler );
	UIRegisterButtonPressHandler( UIX_TEXTBOX, ButtonPressHandler );
	UIRegisterButtonPressHandler( UIX_MENU_ITEM, ButtonPressHandler );
	
		
	UIRegisterHoldHandler( UIX_SLIDER_BAR, SliderHoldHandler );
	UIRegisterHoldHandler( UIX_SLIDER_BAR_MINRANGE, SliderHoldHandler );
	UIRegisterHoldHandler( UIX_SLIDER_BAR_MAXRANGE, SliderHoldHandler );
	UIXListBox::RegisterControlHandlers();
	UIXCollapsableSection::RegisterControlHandlers();
	UIXScrollableSection::RegisterControlHandlers();
	UIXTextBox::RegisterControlHandlers();
}

void		UIX::Update( float delta )
{
	for( UIXObject* pxObjects : msPagesList )
	{
		pxObjects->Update( delta );
	}
}

void		UIX::Reset()
{
	for( UIXObject* pxObjects : msPagesList )
	{
		DeleteObject( pxObjects );
	}
	msPagesList.clear();
}



BOOL		UIX::IsMouseHover( UIXRECT rect )
{
	return ( UIHoverItem( rect.x, rect.y, rect.w, rect.h ) );
}

void		UIX::Render( InterfaceInstance* pxInterface )
{
UIXRECT		pageDisplayRect;

	for ( int loop = 0; loop < MAX_NUM_UIX_ICONS; loop++ )
	{
		mshUIXIconOverlays[loop] = NOTFOUND;
	}

	msSelectionPriority = 0;
	msPressedSelectionPriority = 0;
	msMouseWheelHoverPriority = 0;

	for( UIXObject* pxObjects : msPagesList )
	{
		pageDisplayRect = pxObjects->GetDisplayRect();
		pxObjects->Render( pxInterface, pageDisplayRect );
	}

	pxInterface->Draw();
	for( UIXObject* pxObjects : msPagesList )
	{
		pxObjects->PostRender( pxInterface );
	}
	// Debug stuff
//	pxInterface->Text( 0, 5, 20, 0xd0f02010, 0, "press-pri: %d", msPressedSelectionPriority );
}

void		UIX::CloseAllMenus()
{
	for( UIXObject* pxPages : msPagesList )
	{
		pxPages->CloseAllMenus();
	}
}


void		UIX::Shutdown()
{
	for( UIXObject* pObject : msPagesList )
	{
		msComponentIDMap[pObject->GetID()] = NULL;
		msComponentIDMap.erase( pObject->GetID() );
		pObject->Shutdown();
		delete pObject;
	}
	msPagesList.clear();
}

void		UIX::DeleteObject( UIXObject* pObject )
{
	// HACK - Should shift to smart pointers
	if ( mspDragDestinationHover == pObject ) mspDragDestinationHover = NULL;

	msPagesList.erase( std::remove(msPagesList.begin(), msPagesList.end(), pObject), msPagesList.end() );
	msComponentIDMap[pObject->GetID()] = NULL;
	msComponentIDMap.erase( pObject->GetID() );
	pObject->Shutdown();
	delete pObject;
}

UIXObject*		UIX::FindUIXObjectByID( uint32 ulObjectID )
{
	auto it = msComponentIDMap.find( ulObjectID );
	if ( it != msComponentIDMap.end() )
	{
		return( it->second );
	}
	return( NULL );
}

void			UIX::DrawIcon( InterfaceInstance* pInterface, int iconNum, UIXRECT rect, uint32 ulCol )
{
	if ( ( iconNum >= 0 ) &&
		 ( iconNum < MAX_NUM_UIX_ICONS ) )
	{
	int		nIconPageNum = iconNum / 36;
	
		iconNum %= 36;
		float	fU = ((iconNum%6) * 19.0f) / 128.0f;
		float	fV = ((iconNum/6) * 19.0f) / 128.0f;
		float	fUW = 19.0f / 128.0f;
		float	fVH = 19.0f / 128.0f;

		if ( mshUIXIconOverlays[nIconPageNum] == NOTFOUND )
		{
		int		hTexture = mshUIXIconsList[nIconPageNum];

			mshUIXIconOverlays[nIconPageNum] = pInterface->CreateNewTexturedOverlay( 1, hTexture );
		}
		pInterface->TexturedRect( mshUIXIconOverlays[nIconPageNum], rect.x, rect.y, rect.w, rect.h, ulCol, fU, fV, fU + fUW, fV + fVH );
	}	
}

void	UIX::LoadIconSheet( InterfaceInstance* pInterface, int sheetNum, const char* szFilename )
{
	mshUIXIconsList[sheetNum] = pInterface->GetTexture( szFilename, 0 );
}

void		UIX::LoadIcon( InterfaceInstance* pInterface, int iconNum, const char* szFilename )
{
	mshUIXIconsList[iconNum] = pInterface->GetTexture( szFilename, 0 );
}


UIXObject*		UIX::AddSubPage( UIXObject* pxContainer, UIXRECT rect, const char* szTitle, BOOL bUseClipping )
{
UIXPage*		pNewPage = new UIXPage( pxContainer, msulNextObjectID++, rect );

	pNewPage->Initialise( szTitle, bUseClipping );
	pxContainer->mContainsList.push_back( pNewPage );
	return( pNewPage );
}

UIXObject*		UIX::AddPage( UIXRECT rect, const char* szTitle, BOOL bUseClipping )
{
UIXPage*		pNewPage = new UIXPage( NULL, msulNextObjectID++, rect );

	pNewPage->Initialise( szTitle, bUseClipping );
	msPagesList.push_back( pNewPage );
	return( pNewPage );
}

UIXText*		UIX::AddText( UIXObject* pxContainer, UIXRECT rect, uint32 ulCol, int font, UIX_TEXT_FLAGS fontFlags, const char* szTitle, ... )
{
UIXText*		pNewText = new UIXText( pxContainer, msulNextObjectID++, rect );
char		acString[1024];
va_list		marker;
uint32*		pArgs;

	pArgs = (uint32*)( &szTitle ) + 1;

    va_start( marker, szTitle );     
	vsprintf( acString, szTitle, marker );
	if ( ulCol == 0 ) ulCol = 0xd0d0d0d0;			// Default col is an offwhite 

	pNewText->Initialise( acString, ulCol, font, fontFlags );
	pxContainer->mContainsList.push_back( pNewText );
	return( pNewText );
}

UIXScrollableSection*		UIX::AddScrollableSection( UIXObject* pxContainer, UIXRECT rect )
{
UIXScrollableSection*		pNewScrollableSection = new UIXScrollableSection( pxContainer, msulNextObjectID++, rect );

	pNewScrollableSection->Initialise();
	pxContainer->mContainsList.push_back( pNewScrollableSection );
	return( pNewScrollableSection );
}

UIXCollapsableSection*		UIX::AddCollapsableSection( UIXObject* pxContainer, UIXRECT rect, int mode, const char* szTitle, BOOL bStartCollapsed, int draggableType )
{
UIXCollapsableSection*		pNewCollapsableSection = new UIXCollapsableSection( pxContainer, msulNextObjectID++, rect );

	pNewCollapsableSection->Initialise( mode, szTitle, bStartCollapsed, draggableType );
	pxContainer->mContainsList.push_back( pNewCollapsableSection );
	return( pNewCollapsableSection );
}

UIXButton*			UIX::AddButton( UIXObject* pxContainer, UIXRECT rect, eUIXBUTTON_MODE mode, const char* szTitle, uint32 ulButtonID, uint32 ulButtonParam, BOOL bIsBlocking, uint32 ulCol, int iconNum  )
{
UIXButton*		pNewButton = new UIXButton( pxContainer, msulNextObjectID++, rect );

	pNewButton->Initialise( mode, szTitle, ulButtonID, ulButtonParam, bIsBlocking, ulCol, iconNum );
	pxContainer->mContainsList.push_back( pNewButton );
	return( pNewButton );
}

uint32		UIX::GetNextObjectID()
{
	return(msulNextObjectID++);
}

UIXCustomRender*	UIX::AddCustomRender( UIXObject* pxContainer, UIXRECT rect, fnCustomRenderCallback renderFunc, uint32 ulUserParam )
{
UIXCustomRender*		pNewCustomRender = new UIXCustomRender( pxContainer, msulNextObjectID++, rect );

	pNewCustomRender->Initialise( renderFunc, ulUserParam );
	pxContainer->mContainsList.push_back( pNewCustomRender );
	return( pNewCustomRender );
}

UIXMenu*			UIX::AddMenuBar( UIXObject* pxContainer, UIXRECT rect )
{
UIXMenu*		pNewMenu = new UIXMenu( pxContainer, msulNextObjectID++, rect );

	pNewMenu->Initialise(  );
	pxContainer->mContainsList.push_back( pNewMenu );
	return( pNewMenu );
}

UIXModalPopup*		UIX::AddModalPopup( UIXObject* pxContainer, UIXRECT rect )
{
UIXModalPopup*		pNewModalPopup = new UIXModalPopup( pxContainer, msulNextObjectID++, rect );

	pNewModalPopup->Initialise(  );
	pxContainer->mContainsList.push_back( pNewModalPopup );
	return( pNewModalPopup );
}


UIXCheckbox*		UIX::AddCheckbox( UIXObject* pxContainer, UIXRECT rect, UIX_CHECKBOX_MODE mode, BOOL bIsChecked, const char* szText, fnSelectedCallback selectedFunc, uint32 ulSelectParam )
{
UIXCheckbox*		pNewCheckbox = new UIXCheckbox( pxContainer, msulNextObjectID++, rect );

	pNewCheckbox->Initialise( mode, bIsChecked, szText, selectedFunc, ulSelectParam );
	pxContainer->mContainsList.push_back( pNewCheckbox );
	return( pNewCheckbox );
}


UIXShape*			UIX::AddShape( UIXObject* pxContainer, UIXRECT rect, int mode, BOOL bBlocks, uint32 ulCol1, uint32 ulCol2, uint32 ulButtonID, uint32 ulButtonParam )
{
UIXShape*		pNewShape = new UIXShape( pxContainer, msulNextObjectID++, rect );

	pNewShape->Initialise( mode, bBlocks, ulCol1, ulCol2, ulButtonID, ulButtonParam );
	pxContainer->mContainsList.push_back( pNewShape );
	return( pNewShape );
}


UIXTextBox*			UIX::AddTextBox( UIXObject* pxContainer, UIXRECT rect, int mode, const char* szDefaultText )
{
UIXTextBox*		pNewTextBox = new UIXTextBox( pxContainer, msulNextObjectID++, rect );

	pNewTextBox->Initialise( mode, szDefaultText );
	pxContainer->mContainsList.push_back( pNewTextBox );
	return( pNewTextBox );
}

UIXListBox*			UIX::AddListBox( UIXObject* pxContainer, UIXRECT rect, int mode, BOOL bContentsDraggable, int dragItemType )
{
UIXListBox*		pNewListbox = new UIXListBox( pxContainer, msulNextObjectID++, rect );

	pNewListbox->Initialise( mode, bContentsDraggable, dragItemType );
	pxContainer->mContainsList.push_back( pNewListbox );
	return( pNewListbox );
}

UIXSlider*			UIX::AddSlider( UIXObject* pxContainer, UIXRECT rect, UIX_SLIDER_MODE mode, uint32 ulUserParam, float fMin, float fMax, float fInitial, float fMinStep, const char* szText )
{
UIXSlider*		pNewSlider = new UIXSlider( pxContainer, msulNextObjectID++, rect );

	pNewSlider->Initialise( mode, ulUserParam, fMin, fMax, fInitial, fMinStep, szText );
	pxContainer->mContainsList.push_back( pNewSlider );
	return( pNewSlider );
}

UIXDropdown*		UIX::AddDropdown( UIXObject* pxContainer, UIXRECT rect )
{
UIXDropdown*		pNewDropdown = new UIXDropdown( pxContainer, msulNextObjectID++, rect );

	pNewDropdown->Initialise( 0 );
	pxContainer->mContainsList.push_back( pNewDropdown );
	return( pNewDropdown );
}


void	UIX::EndDragItemType( int type ) 
{
	if ( msDragItemType == type )
	{
		if ( ( mspDragDestinationHover != NULL ) &&
			 ( mspDragSource != mspDragDestinationHover ) )
		{
			mspDragDestinationHover->OnReceiveDragItem( type, mspDragSource, msDragSourceParam);
			mspDragDestinationHover = NULL;
		}
		msDragItemType = 0; 
	}

}

void	UIStateData::OnUpdate( float fDelta )
{
	if ( mpAssociatedUIXObj ) mpAssociatedUIXObj->UpdateUIStateData( this );
}


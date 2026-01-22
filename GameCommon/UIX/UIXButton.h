#ifndef UIX_BUTTON_H
#define UIX_BUTTON_H

#include <string>
#include "UIX.h"

typedef	bool(*fnLabelEditCallback)( UIXObject* pObject, const char* szInputText );

class UIXButton : public UIXObject
{
friend class UIX;
public:

	void	EnableLabelEdit( fnLabelEditCallback callbackFunc );
	
	void	AddRightButtonPress( uint32 ulButtonID, uint32 ulButtonParam ) { mulRightPressButtonID = ulButtonID; mulRightPressButtonParam = ulButtonParam; }

protected:
	UIXButton( UIXObject* pxParent, uint32 uID, UIXRECT rect ) : UIXObject( pxParent, uID, rect ) {}
	~UIXButton();

	void	Initialise( eUIXBUTTON_MODE mode, const char* szTitle, uint32 ulButtonID, uint32 ulButtonParam, BOOL bIsBlocking = TRUE, uint32 ulCol = 0xD0404040, int iconNum = 0 );
	
	virtual UIXRECT		OnRender( InterfaceInstance* pInstance, UIXRECT pDisplayRect );

private:
	static int		ButtonEditLabelKeyboardMessageHandlerStatic( int nResponseCode, const char* szInputText, void* pUserObj );
	int		OnKeyboardMessage( int nResponseCode, const char* szInputText );

	std::string			mTitle;
	std::string			mPreEditTitle;
	uint32				mulButtonID = 0;
	uint32				mulButtonParam = 0;
	uint32				mulRightPressButtonID = 0;
	uint32				mulRightPressButtonParam = 0;
	eUIXBUTTON_MODE		mMode = UIXBUTTON_NORMAL;
	BOOL				mbIsBlocking = TRUE;
	BOOL				mbIsLabelEditable = FALSE;
	fnLabelEditCallback		mfnLabelEditCallback = NULL;
	uint32				mulCol = 0xd0404040;
	int					mIconNum = 0;
	int					mhImageTexture = NOTFOUND;
};






#endif
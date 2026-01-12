#ifndef UIX_CHECKBOX_H
#define UIX_CHECKBOX_H

#include <string>
#include "UIX.h"

class UIXCheckbox : public UIXObject
{
friend class UIX;
public:
	BOOL	IsChecked() { return( mbIsChecked); }

	void	SetToggleIcons( int offIcon, int onIcon ) { mnOffIcon = offIcon; mnOnIcon = onIcon; }
	void	SetBackgroundCol( uint32 ulBgCol ) { mulBgCol = ulBgCol; }
	void	SetText( const char* szText ) { mText = szText; }
protected:
	UIXCheckbox( uint32 uID, UIXRECT rect ) : UIXObject( uID, rect ) {}

	void	Initialise( UIX_CHECKBOX_MODE mode, BOOL bIsChecked, const char* szText, fnSelectedCallback selectedFunc );
	
	virtual UIXRECT		OnRender( InterfaceInstance* pInstance, UIXRECT pDisplayRect );

	void	OnPressed( void );
private:
	BOOL				mbIsChecked = FALSE;
	fnSelectedCallback	mfnSelectedCallback = NULL;
	UIX_CHECKBOX_MODE	mMode = STANDARD_CHECKBOX;
	std::string			mText;
	uint32				mulBgCol = 0xD0202020;
	int					mnOffIcon = NOTFOUND;
	int					mnOnIcon = NOTFOUND;
};






#endif
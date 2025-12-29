#ifndef UIX_SLIDER_H
#define UIX_SLIDER_H

#include "UIX.h"

class UIXSlider : public UIXObject
{
friend class UIX;
public:

	void	RegisterValueUpdateHandler( fnValueUpdateCallback func ) { mValueUpdateFunc = func; }
	void	SetRangeLimit( float min, float max ) { mfInitialMinVal = min; mfInitialMaxVal = max; }
protected:
	UIXSlider( uint32 uID, UIXRECT rect ) : UIXObject( uID, rect ) {}

	void	Initialise( UIX_SLIDER_MODE mode,uint32 ulUserParam, float fMin, float fMax, float fInitialVal, float fMinStep );
	
	void	OnHeldUpdate( BOOL bIsHeld, BOOL bFirstPress );
	void	OnMinRangeHeldUpdate( BOOL bIsHeld, BOOL bFirstPress );
	void	OnMaxRangeHeldUpdate( BOOL bIsHeld, BOOL bFirstPress );

	virtual UIXRECT		OnRender( InterfaceInstance* pInstance, UIXRECT pDisplayRect );
	virtual void		OnUpdate( float fDelta );

private:
	float				GetCursorValue();
	UIXRECT				mRenderRect;
	UIX_SLIDER_MODE		mMode;
	float				mfMinVal;
	float				mfMaxVal;
	float				mfInitialMinVal;
	float				mfInitialMaxVal;
	float				mfInitialVal;
	float				mfCurrentVal;
	float				mfMinStep;
	uint32				mulUserParam;
	BOOL				mbIsHeld;

	fnValueUpdateCallback	mValueUpdateFunc = NULL;
};






#endif
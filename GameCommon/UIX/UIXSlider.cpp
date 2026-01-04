
#include "StandardDef.h"
#include "InterfaceEx.h"

#include "../UI/UI.h"
#include "UIXSlider.h"


void	UIXSlider::Initialise( UIX_SLIDER_MODE mode, uint32 ulUserParam, float fMin, float fMax, float fInitialVal, float fMinStep )
{
	mfMinVal = fMin;
	mfMaxVal = fMax;
	mfInitialMinVal = fMin;
	mfInitialMaxVal = fMax;
	mfInitialVal = fInitialVal;
	mfCurrentVal = fInitialVal;
	mfMinStep = fMinStep;
	mMode = mode;
	mulUserParam = ulUserParam;
}


float	UIXSlider::GetCursorValue()
{
int		cursX = 0;
int		cursY = 0;
float	fRange = (float)mRenderRect.w;
float		fMinVal = mfInitialMinVal;
float		fMaxVal = mfInitialMaxVal;

	UIGetCurrentCursorPosition( &cursX, &cursY );
	
	if ( fRange <= 0.0f ) fRange = 1.0f;
	float	fVal = ((cursX - mRenderRect.x) * (fMaxVal-fMinVal)) / fRange;
	fVal += fMinVal;
	if ( fVal < fMinVal ) fVal = fMinVal;
	if ( fVal > fMaxVal ) fVal = fMaxVal;
	return( fVal );
}

void	UIXSlider::OnHeldUpdate( BOOL bIsHeld, BOOL bFirstPress )
{
	if ( bIsHeld )
	{
		mfCurrentVal = GetCursorValue();
	}
	mbIsHeld = bIsHeld;
}

void	UIXSlider::OnMinRangeHeldUpdate( BOOL bIsHeld, BOOL bFirstPress )
{
	if ( bIsHeld )
	{
		mfMinVal = GetCursorValue();
		if ( mfMinVal > mfMaxVal )
		{
			mfMinVal = mfMaxVal;
		}
	}
	mbIsHeld = bIsHeld;
}

void	UIXSlider::OnMaxRangeHeldUpdate( BOOL bIsHeld, BOOL bFirstPress )
{
	if ( bIsHeld )
	{
		mfMaxVal = GetCursorValue();
		if ( mfMaxVal < mfMinVal )
		{
			mfMaxVal = mfMinVal;
		}
	}
	mbIsHeld = bIsHeld;
}

UIXRECT		UIXSlider::OnRender( InterfaceInstance* pInterface, UIXRECT displayRect )
{
UIXRECT		renderRect = GetActualRenderRect( displayRect );
uint32		ulCol = 0xf0505070;

	mRenderRect = renderRect;

	switch( mMode )
	{
	case VALUERANGE:
		{
		int		nBarMaxW = renderRect.w;
		int		nMinBarPos = (int)(((mfMinVal-mfInitialMinVal) * nBarMaxW) / (mfInitialMaxVal-mfInitialMinVal));
		int		nMaxBarPos = (int)(((mfMaxVal-mfInitialMinVal) * nBarMaxW) / (mfInitialMaxVal-mfInitialMinVal));

			// Background
			pInterface->Rect( 0, mRenderRect.x, mRenderRect.y, nBarMaxW, mRenderRect.h, 0xf0080808 );
			// INDICATOR (MIN)
			ulCol = 0xf0505070;
			if ( UIHoverItem( mRenderRect.x + nMinBarPos - 6, mRenderRect.y, 6, mRenderRect.h ) == TRUE )
			{
				UIHoverIDSet( UIX_SLIDER_BAR_MINRANGE, 0, GetID() );
				ulCol = 0xf0a08050;
			}
			pInterface->Triangle( 1, mRenderRect.x + nMinBarPos - 6, mRenderRect.y + 6, mRenderRect.x + nMinBarPos, mRenderRect.y + 6, mRenderRect.x + nMinBarPos, mRenderRect.y, ulCol, ulCol, ulCol );
			pInterface->Rect( 0, mRenderRect.x + nMinBarPos - 6, mRenderRect.y + 6, 6, mRenderRect.h - 6, ulCol );
			// INDICATOR (MAX)
			ulCol = 0xf0505070;
			if ( UIHoverItem( mRenderRect.x + nMaxBarPos, mRenderRect.y, 6, mRenderRect.h ) == TRUE )
			{
				UIHoverIDSet( UIX_SLIDER_BAR_MAXRANGE, 0, GetID() );
				ulCol = 0xf0a08050;
			}
			pInterface->Rect( 0, mRenderRect.x + nMaxBarPos, mRenderRect.y + 6, 6, mRenderRect.h - 6, ulCol );
			pInterface->Triangle( 1, mRenderRect.x + nMaxBarPos, mRenderRect.y + 6, mRenderRect.x + nMaxBarPos + 6, mRenderRect.y + 6, mRenderRect.x + nMaxBarPos, mRenderRect.y, ulCol, ulCol, ulCol );

			// between bar
			pInterface->Rect( 0, mRenderRect.x + nMinBarPos + 2, mRenderRect.y + ( mRenderRect.h / 2), (nMaxBarPos - nMinBarPos) - 2, (mRenderRect.h/2), 0xA0404040 );
			// Notches
			pInterface->Rect( 1, mRenderRect.x, mRenderRect.y + mRenderRect.h - 3, 1, 3, 0xa0909090 );
			pInterface->Rect( 1, mRenderRect.x + (nBarMaxW/2), mRenderRect.y + mRenderRect.h - 5, 1, 5, 0xa0909090 );
			pInterface->Rect( 1, mRenderRect.x + nBarMaxW, mRenderRect.y + mRenderRect.h - 3, 1, 3, 0xa0909090 );
		}
		break;
	case ANGLE:
		{
		int		nBarMaxW = mRenderRect.w;
		int		nBarPos;
		float	angleMod = (mfCurrentVal * 360.0f) / A360;
		
			if ( angleMod > 180.0f ) angleMod -= 360.0f;

			nBarPos = (int)( (nBarMaxW / 2) + ( (angleMod * (nBarMaxW / 2)) / 180.0f ) );

//			pInterface->Text( 1, X + 110, lineY + 4, 0xd0e0e0e0, 3, "%.1f", angleMod );
			// BACKGROUND
			pInterface->Rect( 0, mRenderRect.x, mRenderRect.y, nBarMaxW, mRenderRect.h, 0xf0010101 );
			if ( UIHoverItem( mRenderRect.x, mRenderRect.y, mRenderRect.w, mRenderRect.h ) == TRUE )
			{
				UIHoverIDSet( UIX_SLIDER_BAR, 0, GetID() );
			}
			// INDICATOR
			pInterface->Rect( 0, mRenderRect.x + nBarPos - 2, mRenderRect.y, 4, mRenderRect.h, 0xf0303040 );
			// Notches
			pInterface->Rect( 1, mRenderRect.x, mRenderRect.y + mRenderRect.h - 3, 1, 3, 0xa0909090 );
			pInterface->Rect( 1, mRenderRect.x + (nBarMaxW/2), mRenderRect.y + mRenderRect.h - 5, 1, 5, 0xa0909090 );
			pInterface->Rect( 1, mRenderRect.x + nBarMaxW, mRenderRect.y + mRenderRect.h - 3, 1, 3, 0xa0909090 );
		}
		break;
//	case SCALER10:
	case VALUE:
	default:
		{
		int		nBarMaxW = mRenderRect.w;
		int		nBarW = (int)(((mfCurrentVal-mfMinVal) * nBarMaxW) / (mfMaxVal-mfMinVal));
	
			if ( nBarW > nBarMaxW ) nBarW = nBarMaxW;
			if ( nBarW < 0 ) nBarW = 0;

			// TODO - This should be editable text box
//			pInterface->Text( 1, X + 110, lineY + 4, 0xd0e0e0e0, 3, "%d%%", (int)(pProperty->Value(0)*100.0f) );
			// Background
			pInterface->Rect( 0, mRenderRect.x, mRenderRect.y, nBarMaxW, mRenderRect.h, 0xf0080808 );
				
			if ( UIHoverItem( mRenderRect.x, mRenderRect.y, mRenderRect.w, mRenderRect.h ) == TRUE )
			{
				UIHoverIDSet( UIX_SLIDER_BAR, 0, GetID() );
			}
			// Notches
			pInterface->Rect( 1, mRenderRect.x, mRenderRect.y + mRenderRect.h - 5, 1, 5, 0xa0909090 );
			pInterface->Rect( 1, mRenderRect.x + (nBarMaxW/2), mRenderRect.y + mRenderRect.h - 3, 1, 3, 0xa0909090 );
			pInterface->Rect( 1, mRenderRect.x + nBarMaxW, mRenderRect.y + mRenderRect.h - 5, 1, 5, 0xa0909090 );
			// Bar
			pInterface->Rect( 0, mRenderRect.x, mRenderRect.y, nBarW, mRenderRect.h, 0xf0202020 );
			pInterface->Rect( 0, mRenderRect.x + nBarW - 2, mRenderRect.y, 4, mRenderRect.h, 0xf0303040 );
		}
		break;
	}
	
	displayRect.h = 0;
	displayRect.y = GetDisplayRect().y + GetDisplayRect().h + 1;		// displayRect.y returns the lowest point we drew to

	return( displayRect );
}


void		UIXSlider::OnUpdate( float fDelta )
{
	if ( mValueUpdateFunc )
	{
		switch( mMode )
		{
		case VALUE:
		case ANGLE:
		default:
			{	
			float	fNewVal = mValueUpdateFunc( GetID(), mfCurrentVal, 0.0f, mulUserParam, mbIsHeld );
				if ( fNewVal != mfCurrentVal )
				{
					mfCurrentVal = fNewVal;
				}
			}		
			break;
		case VALUERANGE:
			mValueUpdateFunc( GetID(), mfMinVal, mfMaxVal, mulUserParam, mbIsHeld );
			break;
		}
	}

}

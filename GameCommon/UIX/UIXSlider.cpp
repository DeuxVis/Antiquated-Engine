
#include "StandardDef.h"
#include "InterfaceEx.h"

#include "../UI/UI.h"
#include "UIXTextBox.h"
#include "UIXSlider.h"

float	UIXSlider::OnValueChange( UIXObject* pxSourceObj, float fNewValue )		// Triggered by (e.g) child text boxes when a new value is entered there directly
{
	if ( pxSourceObj == mpRangeMinTextBox)
	{
		mfMinVal = fNewValue;
		if (mfMinVal > mfMaxVal)
		{
			mfMinVal = mfMaxVal;
		}
		// Expand the handled range if our new value is higher
		if ( mfMinVal < mfInitialMinVal )
		{
			mfInitialMinVal = mfMinVal;
		}
		return mfMinVal;
	}
	else if ( pxSourceObj == mpRangeMaxTextBox )
	{
		mfMaxVal = fNewValue;
		if (mfMaxVal < mfMinVal)
		{
			mfMaxVal = mfMinVal;
		}
		// Expand the handled range if our new value is higher
		if ( mfMaxVal > mfInitialMaxVal )
		{
			mfInitialMaxVal = mfMaxVal;
		}
		return mfMaxVal;	
	}
	else if ( pxSourceObj == mpValueTextBox )
	{
		mfCurrentVal = fNewValue;
	}
	return( fNewValue );
}

void	UIXSlider::Initialise( UIX_SLIDER_MODE mode, uint32 ulUserParam, float fMin, float fMax, float fInitialVal, float fMinStep, const char* szText )
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
	if ( szText )
	{
		mText = szText;
	}

	char	acVal[128];

	switch( mMode )
	{
	case VALUERANGE:
		{
		int		nTextSectionW = 120;
		int		nTextBoxW = (nTextSectionW / 2) - 8;
		UIXRECT		textboxRect = UIXRECT(0, 0, nTextBoxW, GetDisplayRect().h - 2);
			
			sprintf( acVal, "%.3f", mfMinVal );
			mpRangeMinTextBox = UIX::AddTextBox( this, textboxRect, 0, acVal );

			textboxRect.x += nTextBoxW + 14;
			sprintf( acVal, "%.3f", mfMaxVal );
			mpRangeMaxTextBox = UIX::AddTextBox( this, textboxRect, 0, acVal );
		} 
		break;
	case SLIDER_PLUSMINUS_VALUE:
		{
			UIXRECT		textboxRect = UIXRECT(0, 0, 60, GetDisplayRect().h - 2);

			sprintf(acVal, "%.2f", mfCurrentVal);
			mpValueTextBox = UIX::AddTextBox(this, textboxRect, 0, acVal);
		}
		break;
	}
}


float	UIXSlider::GetCursorValue()
{
int		cursX = 0;
int		cursY = 0;
float	fRange = (float)mRenderRect.w;
float		fMinVal = mfInitialMinVal;
float		fMaxVal = mfInitialMaxVal;
float		fVal;

	UIGetCurrentCursorPosition( &cursX, &cursY );
	
	if ( mMode == VERTICAL_VALUE )
	{
		fRange = (float)( mRenderRect.h );
		if ( fRange <= 0.0f ) fRange = 1.0f;
		fVal = (((mRenderRect.y + mRenderRect.h) - cursY) * (fMaxVal-fMinVal)) / fRange;
	}
	else
	{
		if ( fRange <= 0.0f ) fRange = 1.0f;
		fVal = ((cursX - mRenderRect.x) * (fMaxVal-fMinVal)) / fRange;
	}
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
		int		nTextAreaW = 120;

			// TODO - Might want to make this optional or part of a different mode..?
			// Show the text values to the left
			UIXRECT		drawRect = renderRect;
			int			nTextBoxW = (nTextAreaW / 2) - 8;
			char		acVal[128];
			uint32		ulTextCol = 0xC0C0C0C0;

			drawRect.w = nTextAreaW;
			drawRect.y += 1;
			drawRect.h -= 2;

			sprintf( acVal, "%.3f", mfMinVal );
			mpRangeMinTextBox->SetText( acVal );
			mpRangeMinTextBox->OnRender( pInterface, drawRect );
//			drawRect.x += nTextBoxW + 2;
			pInterface->Text( 1, drawRect.x + nTextBoxW + 4, drawRect.y + 4, ulTextCol, 3, "-" );
//			drawRect.x += 14;
			sprintf( acVal, "%.3f", mfMaxVal );
			mpRangeMaxTextBox->SetText( acVal );
			mpRangeMaxTextBox->OnRender( pInterface, drawRect );

/*			
			pInterface->OutlineBox( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0x40404040 );
			pInterface->Text( 1, drawRect.x + 2, drawRect.y + 4, ulTextCol, 3, acVal );
			drawRect.x += drawRect.w + 2;
			pInterface->Text( 1, drawRect.x + 2, drawRect.y + 4, ulTextCol, 3, "-" );

			drawRect.x += 14;
			sprintf( acVal, "%.3f", mfMaxVal );
			pInterface->OutlineBox( 0, drawRect.x, drawRect.y, drawRect.w, drawRect.h, 0x40404040 );
			pInterface->Text( 1, drawRect.x + 2, drawRect.y + 4, ulTextCol, 3, acVal );
			*/

			int		nBarMaxW = renderRect.w - nTextAreaW;
			int		nMinBarPos = (int)(((mfMinVal-mfInitialMinVal) * nBarMaxW) / (mfInitialMaxVal-mfInitialMinVal));
			int		nMaxBarPos = (int)(((mfMaxVal-mfInitialMinVal) * nBarMaxW) / (mfInitialMaxVal-mfInitialMinVal));
			int		nCurrentBarPos = (int)(((mfCurrentVal-mfInitialMinVal) * nBarMaxW) / (mfInitialMaxVal-mfInitialMinVal));

			mRenderRect.x += nTextAreaW;
			mRenderRect.w -= nTextAreaW;
			// Background
			pInterface->Rect( 0, mRenderRect.x, mRenderRect.y, nBarMaxW, mRenderRect.h, 0xf0080808 );
			// INDICATOR (MIN)
			ulCol = 0xf0505070;
			if ( UIHoverItem( mRenderRect.x + nMinBarPos - 7, mRenderRect.y, 8, mRenderRect.h ) == TRUE )
			{
				UIHoverIDSet( UIX_SLIDER_BAR_MINRANGE, 0, GetID() );
				ulCol = 0xf0a08050;
			}
			pInterface->Triangle( 1, mRenderRect.x + nMinBarPos - 6, mRenderRect.y + 6, mRenderRect.x + nMinBarPos, mRenderRect.y + 6, mRenderRect.x + nMinBarPos, mRenderRect.y, ulCol, ulCol, ulCol );
			pInterface->Rect( 0, mRenderRect.x + nMinBarPos - 6, mRenderRect.y + 6, 6, mRenderRect.h - 6, ulCol );
			// INDICATOR (MAX)
			ulCol = 0xf0505070;
			if ( UIHoverItem( mRenderRect.x + nMaxBarPos - 1, mRenderRect.y, 8, mRenderRect.h ) == TRUE )
			{
				UIHoverIDSet( UIX_SLIDER_BAR_MAXRANGE, 0, GetID() );
				ulCol = 0xf0a08050;
			}
			pInterface->Rect( 0, mRenderRect.x + nMaxBarPos, mRenderRect.y + 6, 6, mRenderRect.h - 6, ulCol );
			pInterface->Triangle( 1, mRenderRect.x + nMaxBarPos, mRenderRect.y + 6, mRenderRect.x + nMaxBarPos + 6, mRenderRect.y + 6, mRenderRect.x + nMaxBarPos, mRenderRect.y, ulCol, ulCol, ulCol );

			// Current value indicator
			ulCol = 0xc0b0b0b0;
			pInterface->Rect( 0, mRenderRect.x + nCurrentBarPos - 1, mRenderRect.y + 4, 2, mRenderRect.h - 6, ulCol );
			

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
	case VERTICAL_VALUE:
		{
		int		nKnobH = mRenderRect.w;
		int		nBarMaxH = mRenderRect.h - nKnobH;
		int		nBarH = (int)(((mfCurrentVal-mfMinVal) * nBarMaxH) / (mfMaxVal-mfMinVal));
		uint32	ulKnobCol = 0xf0303040;
			if ( nBarH > nBarMaxH ) nBarH = nBarMaxH;
			if ( nBarH < 0 ) nBarH = 0;

			// TODO - This should be editable text box
//			pInterface->Text( 1, X + 110, lineY + 4, 0xd0e0e0e0, 3, "%d%%", (int)(pProperty->Value(0)*100.0f) );
			// Background
			pInterface->Rect( 0, mRenderRect.x, mRenderRect.y, mRenderRect.w, mRenderRect.h, 0xf0080808 );
				
			if ( UIHoverItem( mRenderRect.x, mRenderRect.y, mRenderRect.w, mRenderRect.h ) == TRUE )
			{
				UIHoverIDSet( UIX_SLIDER_BAR, 0, GetID() );
				ulKnobCol = 0xf0505070;
			}
			// Notches
			pInterface->Rect( 1, mRenderRect.x, mRenderRect.y, 4, 1, 0xa0909090 );
			pInterface->Rect( 1, mRenderRect.x, mRenderRect.y + (mRenderRect.h/2), 3, 1, 0xa0909090 );
			pInterface->Rect( 1, mRenderRect.x, mRenderRect.y + mRenderRect.h, 4, 1, 0xa0909090 );
			// Bar
			pInterface->Rect( 0, mRenderRect.x, mRenderRect.y + nBarMaxH + nKnobH - nBarH, mRenderRect.w, nBarH, 0xf0202020 );

			int		nKnobTop =  mRenderRect.y + mRenderRect.h - (nBarH + nKnobH);
			pInterface->Rect( 0, mRenderRect.x, nKnobTop, mRenderRect.w, nKnobH, ulKnobCol );

			int			textY = nKnobTop + (((nKnobH - pInterface->GetStringHeight(mText.c_str(), 3))-1)/2);

			pInterface->TextCentre( 1, mRenderRect.x + (mRenderRect.w / 2), textY, 0xD0F0F0F0, 3, mText.c_str() );
		}
		break;
	case SLIDER_PLUSMINUS_VALUE:
		{
			int		nBarMaxW = mRenderRect.w;
			int		nBarPos;
			float	angleMod = (mfCurrentVal * 360.0f) / A360;
			int		nBarHalfW = (nBarMaxW / 2);
			UIXRECT		drawRect = renderRect;
			char		acVal[128];
			if (angleMod > 180.0f) angleMod -= 360.0f;

			drawRect.w = 60;
			drawRect.y += 1;
			drawRect.h -= 2;

			sprintf(acVal, "%.3f", mfCurrentVal );
			mpValueTextBox->SetText(acVal);
			mpValueTextBox->OnRender(pInterface, drawRect);
			
			mRenderRect.x += 60;
			mRenderRect.w -= 60;

			// TODO - MinVal might not be -MaxVal 
			// (This currently assumes middle is at 0 and its always, like, -10 -> 10 or something
			nBarPos = (int)(nBarHalfW + ((mfCurrentVal * nBarHalfW) / mfMaxVal));

			//			pInterface->Text( 1, X + 110, lineY + 4, 0xd0e0e0e0, 3, "%.1f", angleMod );
						// BACKGROUND
			pInterface->Rect(0, mRenderRect.x, mRenderRect.y, nBarMaxW, mRenderRect.h, 0xf0010101);
			if (UIHoverItem(mRenderRect.x, mRenderRect.y, mRenderRect.w, mRenderRect.h) == TRUE)
			{
				UIHoverIDSet(UIX_SLIDER_BAR, 0, GetID());
			}
			if (nBarPos < nBarHalfW)
			{
				pInterface->Rect(0, mRenderRect.x + nBarPos, mRenderRect.y, nBarHalfW - nBarPos, mRenderRect.h, 0xf0202020);
			}
			else
			{
				pInterface->Rect(0, mRenderRect.x + nBarHalfW, mRenderRect.y, nBarPos - nBarHalfW, mRenderRect.h, 0xf0202020);
			}
			// INDICATOR
			pInterface->Rect(0, mRenderRect.x + nBarPos - 2, mRenderRect.y, 4, mRenderRect.h, 0xf0303040);
			// Notches
			pInterface->Rect(1, mRenderRect.x, mRenderRect.y + mRenderRect.h - 3, 1, 3, 0xa0909090);
			pInterface->Rect(1, mRenderRect.x + (nBarMaxW / 2), mRenderRect.y + mRenderRect.h - 5, 1, 5, 0xa0909090);
			pInterface->Rect(1, mRenderRect.x + nBarMaxW, mRenderRect.y + mRenderRect.h - 3, 1, 3, 0xa0909090);
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
			// Value/Grab bar
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
		case VERTICAL_VALUE:
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
			mfCurrentVal = mValueUpdateFunc( GetID(), mfMinVal, mfMaxVal, mulUserParam, mbIsHeld );
			break;
		}
	}

}

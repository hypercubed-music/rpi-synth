#include "hx8357.h"

#define MADCTL_MY 0x80  ///< Bottom to top
#define MADCTL_MX 0x40  ///< Right to left
#define MADCTL_MV 0x20  ///< Reverse Mode
#define MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define MADCTL_MH 0x04  ///< LCD refresh right to left

void _swap_int(int x, int y) {
	x = y ^ x;
	y = x ^ y;
	x = y ^ x;
}

void HX8357::sendCommand(u8 commandByte, const u8* dataBytes, u8 numDataBytes) {
    m_DCPin.Write(LOW);
    m_pSPIMaster->Write(0, &commandByte, 1);
    m_DCPin.Write(HIGH);
    m_pSPIMaster->Write(0, dataBytes, 1);
}

void HX8357::sendCommand(u8 commandByte) {
    m_DCPin.Write(LOW);
    m_pSPIMaster->Write(0, &commandByte, 1);
    m_DCPin.Write(HIGH);
}

void HX8357::SendData (const void *pData, size_t nLength)
{
	assert (pData != 0);
	assert (nLength > 0);
	assert (m_pSPIMaster != 0);

	m_DCPin.Write(HIGH);

	m_pSPIMaster->SetClock (m_nClockSpeed);
	m_pSPIMaster->SetMode (m_CPOL, m_CPHA);

#ifndef NDEBUG
	int nResult =
#endif
		m_pSPIMaster->Write (m_nChipSelect, pData, nLength);
		//m_pSPIMaster->WriteReadSync(m_nChipSelect, pData, nullptr, nLength);
	assert (nResult == (int) nLength);
}

HX8357 *HX8357::s_pThis = 0;

HX8357::HX8357 (CSPIMaster *pSPIMaster,
				unsigned nDCPin, unsigned nResetPin, unsigned nBackLightPin,
				unsigned nWidth, unsigned nHeight,
				unsigned CPOL, unsigned CPHA, unsigned nClockSpeed,
				unsigned nChipSelect)
:	m_pSPIMaster (pSPIMaster),
	m_nResetPin (nResetPin),
	m_nBackLightPin (nBackLightPin),
	m_nWidth (nWidth),
	m_nHeight (nHeight),
	m_CPOL (CPOL),
	m_CPHA (CPHA),
	m_nClockSpeed (nClockSpeed),
	m_nChipSelect (nChipSelect),
	m_DCPin (nDCPin, GPIOModeOutput),
	m_pTimer (CTimer::Get ())
{
	assert (nDCPin != None);

	if (m_nBackLightPin != None)
	{
		m_BackLightPin.AssignPin (m_nBackLightPin);
		m_BackLightPin.SetMode (GPIOModeOutput, FALSE);
	}

	if (m_nResetPin != None)
	{
		m_ResetPin.AssignPin (m_nResetPin);
		m_ResetPin.SetMode (GPIOModeOutput, FALSE);
	}
		
	m_nRotation = 0;

	assert (s_pThis == 0);
	s_pThis = this;
}

bool HX8357::Initialize()
{
    assert (m_pSPIMaster != 0);
	assert (m_pTimer != 0);

	if (m_nBackLightPin != None)
	{
		m_BackLightPin.Write (LOW);
		m_pTimer->MsDelay (100);
		m_BackLightPin.Write (HIGH);
	}

	if (m_nResetPin != None)
	{
		m_ResetPin.Write (HIGH);
		m_pTimer->MsDelay (50);
		m_ResetPin.Write (LOW);
		m_pTimer->MsDelay (50);
		m_ResetPin.Write (HIGH);
		m_pTimer->MsDelay (50);
	}

	Command (HX8357_SWRESET);	// Software reset
	m_pTimer->MsDelay (10);

	Command (HX8357D_SETC);
	Data (0xFF);
    Data (0x83);
    Data (0x57);
    m_pTimer->MsDelay (300);

	Command(HX8357_SETRGB);
    Data(0x80);
    Data(0x00);
    Data(0x06);
    Data(0x06);

    Command(HX8357D_SETCOM);
    Data(0x25);

    Command(HX8357_SETOSC);
    Data(0x68);

    Command(HX8357_SETPANEL);
    Data(0x05);

    Command(HX8357_SETPWR1);
    Data(0x00);
    Data(0x15);
    Data(0x1C);
    Data(0x1C);
    Data(0x83);
    Data(0xAA);

    Command(HX8357D_SETSTBA);
    Data(0x50);
    Data(0x50);
    Data(0x01);
    Data(0x3C);
    Data(0x1E);
    Data(0x08);

    Command(HX8357D_SETCYC);
    Data(0x02);
    Data(0x40);
    Data(0x00);
    Data(0x2A);
    Data(0x2A);
    Data(0x0D);
    Data(0x78);

    Command(HX8357D_SETGAMMA);
    Data(0x02);
    Data(0x0A);
    Data(0x11);
    Data(0x1D);
    Data(0x23);
    Data(0x35);
    Data(0x41);
    Data(0x4B);

    Data(0x4B);
    Data(0x42);
    Data(0x3A);
    Data(0x27);
    Data(0x1B);
    Data(0x08);
    Data(0x09);
    Data(0x03);

    Data(0x02);
    Data(0x0A);
    Data(0x11);
    Data(0x1D);
    Data(0x23);
    Data(0x35);
    Data(0x41);
    Data(0x4B);

    Data(0x4B);
    Data(0x42);
    Data(0x3A);
    Data(0x27);
    Data(0x1B);
    Data(0x08);
    Data(0x09);
    Data(0x03);

    Data(0x00);
    Data(0x01);

    Command(HX8357_COLMOD);
    Data(0x55);

    Command(HX8357_MADCTL);
    Data(0xC0);

    Command(HX8357_TEON);
    Data(0x00);

    Command(HX8357_TEARLINE);
    Data(0x00);
    Data(0x02);

	Command (HX8357_SLPOUT);

    m_pTimer->SimpleMsDelay(150);

	On ();

	Clear ();

	if (UG_Init (&m_GUI, UG_SetPixel, m_nWidth, m_nHeight) < 0)
	{
		return FALSE;
	}

	//UG_DriverRegister(DRIVER_FILL_FRAME, (void*)&_HW_FillFrame);

	if (UG_SelectGUI (&m_GUI) < 0)
	{
		return FALSE;
	}

	return TRUE;
}

void HX8357::SetRotation (unsigned nRot)
{
	if (nRot == 0 || nRot == 90 || nRot == 180 || nRot == 270)
	{
		m_nRotation = nRot;
	}
}

void HX8357::On (void)
{
	assert (m_pTimer != 0);

	Command (HX8357_DISPON);
	m_pTimer->MsDelay (50);
}

void HX8357::Off (void)
{
	Command (HX8357_DISPOFF);
}

void HX8357::Clear (HX8357Color Color)
{
	assert (m_nWidth > 0);
	assert (m_nHeight > 0);

	SetWindow (0, 0, m_nWidth-1, m_nHeight-1);

	HX8357Color Buffer[m_nWidth];
	for (unsigned x = 0; x < m_nWidth; x++)
	{
		Buffer[x] = Color;
	}

	for (unsigned y = 0; y < m_nHeight; y++)
	{
		SendData (Buffer, sizeof Buffer);
	}
}

unsigned HX8357::RotX (unsigned x, unsigned y)
{
	//   0 -> [x,y]
	//  90 -> [y, MAX_Y-x]
	// 180 -> [MAX_X-x, MAX_Y-y]
	// 270 -> [MAX_X-y, x]
	switch (m_nRotation)
	{
		case 90:
			return y;
			break;

		case 180:
			return m_nWidth - 1 - x;
			break;

		case 270:
			return m_nWidth - 1 - y;
			break;

		default: // 0 or other
			return x;
			break;
	}
}

unsigned HX8357::RotY (unsigned x, unsigned y)
{
	switch (m_nRotation)
	{
		case 90:
			return m_nHeight - 1 - x;
			break;

		case 180:
			return m_nHeight - 1 - y;
			break;

		case 270:
			return x;
			break;

		default: // 0 or other
			return y;
			break;
	}
}

void HX8357::SetPixel (unsigned nPosX, unsigned nPosY, HX8357Color Color)
{
	unsigned xc = RotX(nPosX, nPosY);
	unsigned yc = RotY(nPosX, nPosY);
	SetWindow (xc, yc, xc, yc);

	SendData (&Color, sizeof Color);
}

void HX8357::UG_SetPixel (UG_S16 nPosX, UG_S16 nPosY, UG_COLOR Color)
{
	// TODO: convert UG colors to screen colors

	assert(s_pThis != 0);
	s_pThis->SetPixel(nPosX, nPosY, Color);
}


void HX8357::DrawText (unsigned nPosX, unsigned nPosY, const char *pString,
			       HX8357Color Color, HX8357Color BgColor, bool bDoubleWidth, bool bDoubleHeight)
{
	assert (pString != 0);

	unsigned nWidthScaler = (bDoubleWidth ? 2 : 1);
	unsigned nHeightScaler = (bDoubleHeight ? 2 : 1);
	unsigned nCharWidth = m_CharGen.GetCharWidth () * nWidthScaler;
	unsigned nCharHeight = m_CharGen.GetCharHeight () * nHeightScaler;

	HX8357Color Buffer[nCharHeight * nCharWidth];

	char chChar;
	while ((chChar = *pString++) != '\0')
	{
		for (unsigned y = 0; y < nCharHeight; y++)
		{
			for (unsigned x = 0; x < nCharWidth; x++)
			{
				HX8357Color pix = m_CharGen.GetPixel (chChar, x/nWidthScaler, y/nHeightScaler)
											? Color : BgColor;
				// Rotation determines order of bytes in the buffer
				switch (m_nRotation)
				{
					case 90:
						// (Last C - C)*rows + R
						Buffer[(nCharWidth-1-x)*nCharHeight + y] = pix;
						break;
					case 180:
						// (Last R - R)*cols + (Last C - C)
						Buffer[(nCharHeight-1-y)*nCharWidth + (nCharWidth-1-x)] = pix;
						break;
					case 270:
						// C*rows + (Last R - R)
						Buffer[x*nCharHeight + nCharHeight - 1 - y] = pix;
						break;
					default:
						// R*cols + C
						Buffer[y*nCharWidth + x] = pix;
						break;
				}
			}
		}
		
		// Need to set different corners of the window depending on
		// the rotation to avoid have negative/inside out windows.
		unsigned x1=nPosX;
		unsigned x2=nPosX+nCharWidth-1;
		unsigned y1=nPosY;
		unsigned y2=nPosY+nCharHeight-1;
		switch (m_nRotation)
		{
			case 90:
				SetWindow (RotX(x2,y1), RotY(x2,y1), RotX(x1,y2), RotY(x1,y2));
				break;
			case 180:
				SetWindow (RotX(x2,y2), RotY(x2,y2), RotX(x1,y1), RotY(x1,y1));
				break;
			case 270:
				SetWindow (RotX(x1,y2), RotY(x1,y2), RotX(x2,y1), RotY(x2,y1));
				break;
			default:
				SetWindow (x1, y1, x2, y2);
				break;
		}

		SendData (Buffer, sizeof Buffer);

		nPosX += nCharWidth;
	}
}

void HX8357::SetWindow (unsigned x0, unsigned y0, unsigned x1, unsigned y1)
{
	assert (x0 <= x1);
	assert (y0 <= y1);
	assert (x1 < m_nWidth);
	assert (y1 < m_nHeight);

	Command (HX8357_CASET);
	Data (x0 >> 8);
	Data (x0 & 0xFF);
	Data (x1 >> 8);
	Data (x1 & 0xFF);

	Command (HX8357_PASET);
	Data (y0 >> 8);
	Data (y0 & 0xFF);
	Data (y1 >> 8);
	Data (y1 & 0xFF);

	Command (HX8357_RAMWR);
}

void HX8357::SendByte (u8 uchByte, boolean bIsData)
{
	assert (m_pSPIMaster != 0);

	m_DCPin.Write (bIsData ? HIGH : LOW);

	m_pSPIMaster->SetClock (m_nClockSpeed);
	m_pSPIMaster->SetMode (m_CPOL, m_CPHA);

#ifndef NDEBUG
	int nResult =
#endif
		m_pSPIMaster->Write (m_nChipSelect, &uchByte, sizeof uchByte);
		//m_pSPIMaster->WriteReadSync(m_nChipSelect, &uchByte, nullptr, sizeof uchByte);
	assert (nResult == (int) sizeof uchByte);
}


// hw acceleration for ugui
UG_RESULT HX8357::_HW_FillFrame( UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c )
{
	s_pThis->SetWindow(x1, y1, x2, y2);
	
	UG_COLOR Buffer[(x2-x1)];

	//SetWindow (0, 0, m_nWidth-1, m_nHeight-1);
	for (int i = 0; i < (x2-x1); i++) {
		Buffer[i] = c;
	}
	//m_pSPIMaster->StartWriteRead(m_nChipSelect, _OutBuffer, nullptr, (x2-x1)*(y2-y1));

	for (int i = 0; i < (y1 - y2); i++) {
		s_pThis->SendData(Buffer, sizeof Buffer);
	}
	return UG_RESULT_OK;
}
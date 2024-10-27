#ifndef _HX8357D
#define _HX8357D

#include <circle/spimaster.h>
//#include <circle/spimasterdma.h>
#include <circle/gpiopin.h>
#include <circle/timer.h>
#include <circle/2dgraphics.h>
#include <circle/logger.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif
	#include "ugui.h"
#ifdef __cplusplus
}
#endif

#define HX8357D 0xD ///< Our internal const for D type
#define HX8357B 0xB ///< Our internal const for B type

#define HX8357_TFTWIDTH 320  ///< 320 pixels wide
#define HX8357_TFTHEIGHT 480 ///< 480 pixels tall

#define HX8357_NOP 0x00     ///< No op
#define HX8357_SWRESET 0x01 ///< software reset
#define HX8357_RDDID 0x04   ///< Read ID
#define HX8357_RDDST 0x09   ///< (unknown)

#define HX8357_RDPOWMODE 0x0A ///< Read power mode Read power mode
#define HX8357_RDMADCTL 0x0B  ///< Read MADCTL
#define HX8357_RDCOLMOD 0x0C  ///< Column entry mode
#define HX8357_RDDIM 0x0D     ///< Read display image mode
#define HX8357_RDDSDR 0x0F    ///< Read dosplay signal mode

#define HX8357_SLPIN 0x10  ///< Enter sleep mode
#define HX8357_SLPOUT 0x11 ///< Exit sleep mode
#define HX8357B_PTLON 0x12 ///< Partial mode on
#define HX8357B_NORON 0x13 ///< Normal mode

#define HX8357_INVOFF 0x20  ///< Turn off invert
#define HX8357_INVON 0x21   ///< Turn on invert
#define HX8357_DISPOFF 0x28 ///< Display on
#define HX8357_DISPON 0x29  ///< Display off

#define HX8357_CASET 0x2A ///< Column addr set
#define HX8357_PASET 0x2B ///< Page addr set
#define HX8357_RAMWR 0x2C ///< Write VRAM
#define HX8357_RAMRD 0x2E ///< Read VRAm

#define HX8357B_PTLAR 0x30   ///< (unknown)
#define HX8357_TEON 0x35     ///< Tear enable on
#define HX8357_TEARLINE 0x44 ///< (unknown)
#define HX8357_MADCTL 0x36   ///< Memory access control
#define HX8357_COLMOD 0x3A   ///< Color mode

#define HX8357_SETOSC 0xB0      ///< Set oscillator
#define HX8357_SETPWR1 0xB1     ///< Set power control
#define HX8357B_SETDISPLAY 0xB2 ///< Set display mode
#define HX8357_SETRGB 0xB3      ///< Set RGB interface
#define HX8357D_SETCOM 0xB6     ///< Set VCOM voltage

#define HX8357B_SETDISPMODE 0xB4 ///< Set display mode
#define HX8357D_SETCYC 0xB4      ///< Set display cycle reg
#define HX8357B_SETOTP 0xB7      ///< Set OTP memory
#define HX8357D_SETC 0xB9        ///< Enable extension command

#define HX8357B_SET_PANEL_DRIVING 0xC0 ///< Set panel drive mode
#define HX8357D_SETSTBA 0xC0           ///< Set source option
#define HX8357B_SETDGC 0xC1            ///< Set DGC settings
#define HX8357B_SETID 0xC3             ///< Set ID
#define HX8357B_SETDDB 0xC4            ///< Set DDB
#define HX8357B_SETDISPLAYFRAME 0xC5   ///< Set display frame
#define HX8357B_GAMMASET 0xC8          ///< Set Gamma correction
#define HX8357B_SETCABC 0xC9           ///< Set CABC
#define HX8357_SETPANEL 0xCC           ///< Set Panel

#define HX8357B_SETPOWER 0xD0     ///< Set power control
#define HX8357B_SETVCOM 0xD1      ///< Set VCOM
#define HX8357B_SETPWRNORMAL 0xD2 ///< Set power normal

#define HX8357B_RDID1 0xDA ///< Read ID #1
#define HX8357B_RDID2 0xDB ///< Read ID #2
#define HX8357B_RDID3 0xDC ///< Read ID #3
#define HX8357B_RDID4 0xDD ///< Read ID #4

#define HX8357D_SETGAMMA 0xE0 ///< Set Gamma

#define HX8357B_SETGAMMA 0xC8        ///< Set Gamma
#define HX8357B_SETPANELRELATED 0xE9 ///< Set panel related

// Plan is to move this to GFX header (with different prefix), though
// defines will be kept here for existing code that might be referencing
// them. Some additional ones are in the ILI9341 lib -- add all in GFX!
// Color definitions
#define HX8357_BLACK 0x0000   ///< BLACK color for drawing graphics
#define HX8357_BLUE 0x001F    ///< BLUE color for drawing graphics
#define HX8357_RED 0xF800     ///< RED color for drawing graphics
#define HX8357_GREEN 0x07E0   ///< GREEN color for drawing graphics
#define HX8357_CYAN 0x07FF    ///< CYAN color for drawing graphics
#define HX8357_MAGENTA 0xF81F ///< MAGENTA color for drawing graphics
#define HX8357_YELLOW 0xFFE0  ///< YELLOW color for drawing graphics
#define HX8357_WHITE 0xFFFF   ///< WHITE color for drawing graphics

class HX8357 {
    public:
        static const unsigned None = GPIO_PINS;
        typedef u16 HX8357Color;

        HX8357 (CSPIMaster *pSPIMaster,
			unsigned nDCPin, unsigned nResetPin = None, unsigned nBackLightPin = None,
			unsigned nWidth = 240, unsigned nHeight = 240,
			unsigned CPOL = 0, unsigned CPHA = 0, unsigned nClockSpeed = 15000000,
			unsigned nChipSelect = 0);

        /// \return Display width in number of pixels
        unsigned GetWidth (void) const		{ return m_nWidth; }
        /// \return Display height in number of pixels
        unsigned GetHeight (void) const		{ return m_nHeight; }

        /// \return Operation successful?
        boolean Initialize (void);

        /// \brief Set the global rotation of the display
        /// \param nRot (0, 90, 180, 270)
        void SetRotation (unsigned nRot);
        /// \return Rotation in degrees (0,90,180,270)
        unsigned GetRotation (void) const	{ return m_nRotation; }

        /// \brief Set display on
        void On (void);
        /// \brief Set display off
        void Off (void);

        /// \brief Clear entire display with color
        /// \param Color RGB565 color with swapped bytes (see: ST7789_COLOR())
        void Clear (HX8357Color Color = HX8357_BLACK);

        /// \brief Set a single pixel to color
        /// \param nPosX X-position (0..width-1)
        /// \param nPosY Y-postion (0..height-1)
        /// \param Color RGB565 color with swapped bytes (see: ST7789_COLOR())
        void SetPixel (unsigned nPosX, unsigned nPosY, HX8357Color Color);

        static void UG_SetPixel (UG_S16 nPosX, UG_S16 nPosY, UG_COLOR Color);

        /// \brief Draw an ISO8859-1 string at a specific pixel position
        /// \param nPosX X-position (0..width-1)
        /// \param nPosY Y-postion (0..height-1)
        /// \param pString 0-terminated string of printable characters
        /// \param Color RGB565 foreground color with swapped bytes (see: ST7789_COLOR())
        /// \param BgColor RGB565 background color with swapped bytes (see: ST7789_COLOR())
        /// \param bDoubleWidth default TRUE for thicker characters on screen
        /// \param bDoubleHeight default TRUE for higher characters on screen
        void DrawText (unsigned nPosX, unsigned nPosY, const char *pString,
                    HX8357Color Color, HX8357Color BgColor = HX8357_BLACK,
                bool bDoubleWidth = TRUE, bool bDoubleHeight = TRUE);

        static UG_RESULT _HW_FillFrame( UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c );
        static UG_RESULT _HW_DrawLine( UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c );

        //void DrawBuffer (TScreenColor * pBuffer, size_t nLength);
    private:
        CGPIOPin m_DCPin;
        CGPIOPin m_ResetPin;
        CGPIOPin m_BackLightPin;
        //CSPIMaster * m_pSPIMaster;
        CSPIMaster * m_pSPIMaster;
        
        CLogger * m_Logger;
        CCharGenerator m_CharGen;
	    CTimer *m_pTimer;
        UG_GUI m_GUI;
        
        unsigned m_nResetPin;
        unsigned m_nBackLightPin;
        unsigned m_nWidth;
        unsigned m_nHeight;
        unsigned m_CPOL;
        unsigned m_CPHA;
        unsigned m_nClockSpeed;
        unsigned m_nChipSelect;
        unsigned m_nRotation;

        void sendCommand(u8 commandByte, const u8* dataBytes, u8 numDataBytes);
        void sendCommand(u8 commandByte);
        void SetWindow (unsigned x0, unsigned y0, unsigned x1, unsigned y1);

        void SendByte (u8 uchByte, boolean bIsData);

        void Command (u8 uchByte)	{ SendByte (uchByte, FALSE); }
        void Data (u8 uchByte)		{ SendByte (uchByte, TRUE); }

        void SendData (const void *pData, size_t nLength);
        
        unsigned RotX (unsigned x, unsigned y);
        unsigned RotY (unsigned x, unsigned y);

        static HX8357 *s_pThis;

};

#endif
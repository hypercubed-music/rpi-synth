#ifndef _kernel_h
#define _kernel_h

#include <circle/actled.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/screen.h>
#include <circle/serial.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/types.h>
#include <circle/bcmframebuffer.h>
#include <circle/2dgraphics.h>
//#include <circle/spimaster.h>
#include <circle/spimasterdma.h>
#include <circle/sound/i2ssoundbasedevice.h>
#include <circle/sound/soundbasedevice.h>
#include "hx8357.h"
#include "synth.h"
#include <math.h>
#include <circle/i2cmaster.h>
#include <circle/usb/usbcontroller.h>

enum TShutdownMode
{
        ShutdownNone,
        ShutdownHalt,
        ShutdownReboot
};

class CKernel
{
public:
        CKernel (void);
        ~CKernel (void);

        boolean Initialize (void);

        TShutdownMode Run (void);

private:
        CActLED                 m_ActLED;
        CKernelOptions          m_Options;
        CDeviceNameService      m_DeviceNameService;
        CScreenDevice           m_Screen;
        CExceptionHandler       m_ExceptionHandler;
        CInterruptSystem        m_Interrupt;
        CTimer                  m_Timer;
        CLogger                 m_Logger;
        CSPIMaster              m_SPI;
        HX8357                  m_HX8357;
        UG_GUI                  m_GUI;
        CI2CMaster		m_I2CMaster;
	CUSBController		*m_pUSB;
        Csynth                  *m_Synth;

        u64 screenUpdateTime = 0;
};

#endif
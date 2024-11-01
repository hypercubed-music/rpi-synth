#include "kernel.h"
#include <circle/usb/usbhcidevice.h>
#include <circle/usb/gadget/usbmidigadget.h>
#include <circle/machineinfo.h>
#include <assert.h>

static const char FromKernel[] = "kernel";

#define SPI_MASTER_DEVICE 0 // 0, 4, 5, 6 on Raspberry Pi 4; 0 otherwise
// #define SPI_CLOCK_SPEED		15000000	// Hz
#define SPI_CLOCK_SPEED 24000000
#define SPI_CPOL 0        // try 0, if it does not work
#define SPI_CPHA 0        // try 1, if it does not work
#define SPI_CHIP_SELECT 0 // 0 or 1; don't care, if not connected

#define WIDTH 320  // display width in pixels
#define HEIGHT 480 // display height in pixels
#define DC_PIN 23
#define RESET_PIN HX8357::None
#define BACKLIGHT_PIN HX8357::None

//#define USB_GADGET_MODE

CKernel::CKernel(void)
    : m_Screen(m_Options.GetWidth(), m_Options.GetHeight()),
      m_Timer(&m_Interrupt),
      m_Logger(m_Options.GetLogLevel(), &m_Timer),
      m_HX8357(&m_SPI, DC_PIN, RESET_PIN, BACKLIGHT_PIN, WIDTH, HEIGHT,
               SPI_CPOL, SPI_CPHA, SPI_CLOCK_SPEED, SPI_CHIP_SELECT),
      m_I2CMaster(CMachineInfo::Get()->GetDevice(DeviceI2CMaster), TRUE),
#ifndef USB_GADGET_MODE
      m_pUSB(new CUSBHCIDevice(&m_Interrupt, &m_Timer, TRUE)), // TRUE: enable plug-and-play
#else
      m_pUSB(new CUSBMIDIGadget(&m_Interrupt)),
#endif
      m_Synth(0)
{
        m_ActLED.Blink(3);
}

CKernel::~CKernel(void)
{
}

boolean CKernel::Initialize(void)
{
        boolean bOK = TRUE;

        if (bOK)
        {
                bOK = m_Screen.Initialize();
        }

        if (bOK)
        {
                bOK = m_Logger.Initialize(&m_Screen);
        }

        if (bOK)
        {
                bOK = m_Interrupt.Initialize();
        }

        if (bOK)
        {
                bOK = m_Timer.Initialize();
        }

        if (bOK)
        {
                m_Logger.Write(FromKernel, LogNotice, "Initializing SPI");
                bOK = m_SPI.Initialize();
        }

        if (bOK)
        {
                m_Logger.Write(FromKernel, LogNotice, "Initializing display");
                bOK = m_HX8357.Initialize();
        }

        if (bOK)
        {
                m_Logger.Write(FromKernel, LogNotice, "Initializing I2C");
                bOK = m_I2CMaster.Initialize();
        }


        if (bOK)
        {
                m_Logger.Write(FromKernel, LogNotice, "Initializing USB");
                assert(m_pUSB);
                bOK = m_pUSB->Initialize();
        }

        if (bOK)
        {
                m_Logger.Write(FromKernel, LogNotice, "Initializing Synth");
                m_Synth = new Csynth(&m_Interrupt, &m_I2CMaster);

                bOK = m_Synth->Initialize();
        }
        m_Logger.Write(FromKernel, LogNotice, "Done initializing");

        m_Throttle.SetSpeed(CPUSpeedMaximum, true);

        return bOK;
}

TShutdownMode CKernel::Run(void)
{
        m_Logger.Write(FromKernel, LogNotice, "Initializing success!");

        //m_ActLED.Blink(2);
        //m_HX8357.DrawText(10, 10, "Hello world!", HX8357_WHITE);

        UG_FontSelect(&FONT_12X16);
        UG_ConsoleSetBackcolor(C_BLACK);
        UG_ConsoleSetForecolor(C_WHITE);
        UG_ConsolePutString("Hello World!\n");

        UG_DrawCircle(200, 100, 50, HX8357_RED);
        UG_DrawCircle(200, 200, 50, HX8357_GREEN);
        UG_DrawCircle(100, 100, 50, HX8357_BLUE);
        UG_DrawCircle(100, 100, 50, HX8357_WHITE);

        m_Logger.Write(FromKernel, LogNotice, "Starting synth");
        //m_HX8357.DrawBuffer(m_2DGraphics.GetBuffer(), 320 * 480 * 2);

        m_Synth->Start();

        for (unsigned nCount = 0; m_Synth->IsActive(); nCount++)
        {
                // This must be called from TASK_LEVEL to update the tree of connected USB devices.
                assert(m_pUSB);
                boolean bUpdated = m_pUSB->UpdatePlugAndPlay();

                m_Synth->Process(bUpdated);

                // 10 frames per second; this is just to test if the display can draw stuff while audio is going
                if (CTimer::GetClockTicks64() - screenUpdateTime > 100000) {
                        UG_FillFrame(100, 100, 200, 200, nCount);
                        screenUpdateTime = CTimer::GetClockTicks64();
                }
        }

        return ShutdownHalt;
}
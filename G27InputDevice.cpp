#include "G27InputDevice.h"
int test51 = 0;
uint8_t DeviceReport[] = { // Reflects what a G29 expects.
    0x01, //Report ID (0x01)
    0x80, 0x80, 0x80, 0x80, //X, Y, Z, Rz (unused) (0x04)
    0x08, //4 MSB = 4 buttons, 4 LSB = hat switch (0x05)
    0x00, 0x00, //10 buttons (0x06, 0x07)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x80, //wheel (0x43, 0x44)
    0xff, 0xff, //gas pedal (0x45, 0x46)
    0xff, 0xff, //break pedal (0x47, 0x48)
    0xff, 0xff, //Clutch (0x49, 0x50)
    0x00, // Shifter (0x51)
    0xff, 0xff, //unknown (0x52, 0x53)
    0x00, // MFD Controls (0x54)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00
};

int wheel = 0;

extern volatile unsigned char sendReport;

void G27InputDevice::ParseHIDData(USBHID* hid __attribute__((unused)), bool is_rpt_id __attribute__((unused)), uint8_t len, uint8_t* buf)
{
    if (HIDUniversal::VID == Device_VID && (HIDUniversal::PID == Device_PID_Compat1 || HIDUniversal::PID == Device_PID_Compat2 || HIDUniversal::PID == Device_PID_Compat3) && len > 2 && buf) // Test for all possible PIDS
    {
        uint8_t buf[7]; // Do not revert identity
        buf[0] = 0xF8;
        buf[1] = 0x0A;
        buf[2] = 0x00;
        buf[3] = 0x00;
        buf[4] = 0x00;
        buf[5] = 0x00;
        buf[6] = 0x00;
        G27InputDevice_Command(buf, sizeof(buf));

        buf[0] = 0xF8;
        buf[1] = 0x09; // Change device mode 
        buf[2] = 0x04; // Logitech G27
        buf[3] = 0x01; // 1=Switch identity after detaching from USB
        buf[4] = 0x00;
        buf[5] = 0x00;
        buf[6] = 0x00;
        G27InputDevice_Command(buf, sizeof(buf));
    };


    if (HIDUniversal::VID == Device_VID && (HIDUniversal::PID == Device_PID) && len > 2 && buf)
    {        
        // Parsing G27 to G29 data
        DeviceReport[5] = (buf[0] & 0xCF) + ((buf[0] & 0x10) << 1) + ((buf[0] & 0x20) >> 1); // Change X and square


        // Buttons remapping - Swapped Paddles, and Start/Share. R3 is fed from buf[2] and shifted.
        
        DeviceReport[6] = 
                  ((buf[1] & 0x01) << 1) + \ // LPaddle
                  ((buf[1] & 0x02) >> 1) + \ // RPaddle
                  ((buf[1] & 0x04) << 0) + \ 
                  ((buf[1] & 0x08) >> 0) + \
                  ((buf[1] & 0x10) << 1) + \ // Start
                  ((buf[1] & 0x20) >> 1) + \ // Share
                  ((buf[1] & 0x40) << 0) + \ // L3?
                  ((buf[2] & 0x40) << 1); // R3

        DeviceReport[7] = ((buf[1] & 0x80) >> 7); // PS button

        DeviceReport[43] = buf[3]; // Wheel
        DeviceReport[44] = buf[4]; // Wheel
        
        DeviceReport[45] = buf[5]; // GAS
        DeviceReport[46] = buf[5]; // GAS

        DeviceReport[47] = buf[6]; // Brake
        DeviceReport[48] = buf[6]; // Brake
        
        DeviceReport[49] = buf[7]; // Clutch
        DeviceReport[50] = buf[7]; // Clutch

        DeviceReport[51] = buf[2]; // Shifter
        
        DeviceReport[52] = buf[8]; // Try shifter axes?
        DeviceReport[53] = buf[9];

        // All buttons I can test work - I have a aftermarket wheel and therefore I am missing buttons. 
        // I have my E-brake tied into what is registered by the game as R3, confirmed working.
        // If somebody wants to send me masks from each button, I can map them.
        // TODO: ALL buttons work

        sendReport = 1;
    }
};

    uint8_t G27InputDevice::OnInitSuccessful()
    {
        if (HIDUniversal::VID == Device_VID && (HIDUniversal::PID == Device_PID_Compat1 || HIDUniversal::PID == Device_PID_Compat2 || HIDUniversal::PID == Device_PID_Compat3 || HIDUniversal::PID == Device_PID))
        {
            Serial.println("Init succesful - G27");
            if (pFuncOnInit)
            {
                pFuncOnInit(); // Call the user function
            }
        }
        return 0;
    };

    void G27InputDevice::SendFFB(unsigned char* buffer)
    {
        G27InputDevice_Command(buffer + 1, 7);
    };

    void G27InputDevice::G27InputDevice_Command(uint8_t * data, uint16_t nbytes)
    {
        pUsb->outTransfer(bAddress, 0x01, nbytes, data);
    };

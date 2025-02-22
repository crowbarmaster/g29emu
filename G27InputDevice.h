#ifndef _G27InputDevice_h_
#define _G27InputDevice_h_

#include "hiduniversal.h"

#define Device_VID 0x046D
#define Device_PID_Compat1 0xC294
#define Device_PID_Compat2 0xC298
#define Device_PID_Compat3 0xC299
#define Device_PID 0xC29B

class G27InputDevice : public HIDUniversal {
public:
    /**
     * Constructor for the G27InputDevice class.
     * @param  p   Pointer to the USB class instance.
     */
    G27InputDevice(USB* p) :
        HIDUniversal(p)
    {

    };
    /**
     * Used to check if a PS Buzz controller is connected.
     * @return Returns true if it is connected.
     */
    bool connected() {
        return HIDUniversal::isReady() && HIDUniversal::VID == Device_VID && (HIDUniversal::PID == Device_PID_Compat1 || HIDUniversal::PID == Device_PID_Compat2 || HIDUniversal::PID == Device_PID_Compat3 || HIDUniversal::PID == Device_PID);
    };
    
    void SendFFB(unsigned char* buffer);
    /**
     * Used to call your own function when the device is successfully initialized.
     * @param funcOnInit Function to call.
     */
    void attachOnInit(void (*funcOnInit)(void)) {
        pFuncOnInit = funcOnInit;
    };
protected:
    /** @name HIDUniversal implementation */
    /**
     * Used to parse USB HID data.
     * @param hid       Pointer to the HID class.
     * @param is_rpt_id Only used for Hubs.
     * @param len       The length of the incoming data.
     * @param buf       Pointer to the data buffer.
     */
    void ParseHIDData(USBHID* hid, bool is_rpt_id, uint8_t len, uint8_t* buf);

    /**
     * Called when a device is successfully initialized.
     * Use attachOnInit(void (*funcOnInit)(void)) to call your own function.
     * This is useful for instance if you want to set the LEDs in a specific way.
     */
    uint8_t OnInitSuccessful();
    /**@}*/

/** @name USBDeviceConfig implementation */
/**
 * Used by the USB core to check what this driver support.
 * @param  vid The device's VID.
 * @param  pid The device's PID.
 * @return     Returns true if the device's VID and PID matches this driver.
 */
    virtual bool VIDPIDOK(uint16_t vid, uint16_t pid) {
        return (vid == Device_VID && pid == (Device_PID || Device_PID_Compat1 || Device_PID_Compat2 || Device_PID_Compat3));
    };
    /**@}*/

private:
    void (*pFuncOnInit)(void); // Pointer to function called in onInit()
    void G27InputDevice_Command(uint8_t* data, uint16_t nbytes);

};
#endif

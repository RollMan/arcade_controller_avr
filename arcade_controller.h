// F:\Documents\vusb-20121206\examples\hid-mouse\firmware\arcade_controller.h


char ReportDescriptor[54] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x39,                    //   USAGE (Hat switch)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x07,                    //   LOGICAL_MAXIMUM (7)
    0x35, 0x00,                    //   PHYSICAL_MINIMUM (0)
    0x46, 0x3b, 0x01,              //   PHYSICAL_MAXIMUM (315)
    0x65, 0x14,                    //   UNIT (Eng Rot:Angular Pos)
    0x75, 0x03,                    //   REPORT_SIZE (3)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x65, 0x00,                    //   UNIT (None)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
    0x29, 0x0a,                    //   USAGE_MAXIMUM (Button 10)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x95, 0x0a,                    //   REPORT_COUNT (10)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0xc0                           // END_COLLECTION
};


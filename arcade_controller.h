// F:\Documents\vusb-20121206\examples\hid-mouse\firmware\arcade_controller.h


char ReportDescriptor[57] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x42, 0x90, 0x00,              // 0x42(0x90)
    0x42, 0x91, 0x00,              // 0x42(0x91)
    0x42, 0x92, 0x00,              // 0x42(0x92)
    0x42, 0x93, 0x00,              // 0x42(0x93)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x04,                    //   REPORT_SIZE (4)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x65, 0x00,                    //   UNIT (None)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x04,                    //   REPORT_COUNT (4)
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


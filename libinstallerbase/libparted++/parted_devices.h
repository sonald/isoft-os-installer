#ifndef _PARTED_DEVICES_H_PARTED_PLUS_
#define _PARTED_DEVICES_H_PARTED_PLUS_ 

#include "devices.h" 

class PartedDevices : public Devices 
{
 public:
    PartedDevices();
 private:
    void remove_zerolength();
    void remove_readonly();
    void remove_blacklist();
    bool in_blacklist(const char* path);
};

#endif // _PARTED_DEVICES_H_PARTED_PLUS_

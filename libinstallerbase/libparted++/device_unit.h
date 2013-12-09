#ifndef _DEVICE_UNIT_H_PARTED_PLUS_
#define _DEVICE_UNIT_H_PARTED_PLUS_

#include <parted/parted.h>

class Device;

class DeviceUnit {
public:
    DeviceUnit(Device* dev);
    Device* owner_dev();
    const char* format(PedSector num);
    const char* format(PedSector num, const char* unit_name);
    const char* format(PedSector num, PedUnit unit);

    static PedUnit get_by_name(const char* name);
    static const char* get_name(PedUnit unit);

    long long unit_size(PedUnit unit);
    long long unit_size(const char* unit_name);

    PedUnit get_default();
    void set_default(PedUnit);
    void set_default(const char* unit_name);
private:
    Device*     dev_;
    PedDevice*	pdev_;
    PedUnit	def_unit_;
};
#endif // _DEVICE_UNIT_H_PARTED_PLUS_

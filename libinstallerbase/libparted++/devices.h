#ifndef _DEVICES_H_PARTED_PLUS_
#define _DEVICES_H_PARTED_PLUS_

#include <list>
#include <parted/parted.h>

class DeviceUnit;
class Devices;
class PartitionTable;
using namespace std;

/*
 * Device is a physical disk.
 * A device can be no partition table.
 */
class Device 
{
 public:
    PedDevice* pdev()                           { return dev_; }
    
    // PedDevice Property
    bool  is_busy() const 			{ return ped_device_is_busy( dev_ ) ? true: false; }
    char* model() const 			{ return dev_->model; }
    char* path() const 				{ return dev_->path; }
    PedSector length() const 			{ return dev_->length; }
    long long sector_size() const 		{ return dev_->sector_size; }
    long long physical_sector_size() const 	{ return dev_->phys_sector_size; }
    
    int open_count() const                      { return dev_->open_count; }
    bool is_read_only() const 			{ return dev_->read_only ? true : false; }
    bool in_external_mode() const 		{ return dev_->external_mode ? true: false; }
    
    // dirty and boot_dirty still not impl. study in the future.

    // external access
    int begin_external_access()			{ return ped_device_begin_external_access( dev_ ); }
    int end_external_access()			{ return ped_device_end_external_access( dev_ ); }

    PartitionTable* parttable()			{ return part_table_; }
    DeviceUnit* unit()				{ return unit_; }

    // length_str is ugly, remove it in the future.
    const char* length_str()
    {
	return ped_unit_format_custom( dev_, dev_->length, PED_UNIT_COMPACT );
    }

 private:
    // Device only created and deleted by class Devices.
    Device(PedDevice* dev);
    ~Device();

    PedDevice* dev_;

    PartitionTable* part_table_;
    DeviceUnit* unit_;
    friend class Devices;
};

/*
 * Devices List.
 * Device can be enumerated or specified with path.
 */
class Devices
{
 public:
    Devices();
    ~Devices();

    // whether list is empty.
    bool is_empty();

    // count of devices in devices list.
    int count();

    // index from zero to count() - 1.
    Device* device(int index);

    // get the device with specified path.
    Device* device(const char* path);

    // print the devices info.
    void print();

 protected:					// derived class can access the device list.
    list<Device*> list_dev_;			// list of device object maintained by class Devices and derived class.
    void del(Device*);				// derived class can del item of list by del(), but no way to new.
};
#endif // _DEVICES_H_PARTED_PLUS_

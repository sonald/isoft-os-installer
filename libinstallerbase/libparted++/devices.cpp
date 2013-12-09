#include <cassert>
#include "devices.h"
#include "parttable.h"
#include "device_unit.h"

/***********************************************************
 * class Device Implementation
 ***********************************************************/
Device::Device(PedDevice* dev)
{
    assert( dev != NULL );
    
    dev_ = dev;
    part_table_ = new PartitionTable(this);
    unit_ = new DeviceUnit(this);
}

Device::~Device()
{
    ped_device_destroy( dev_ );
    delete part_table_;
    delete unit_;
}

/***********************************************************
 * class Devices Implementation
 ***********************************************************/
Devices::Devices()
{
    ped_device_probe_all();
    PedDevice* ped_dev = NULL;

    while( ped_dev = ped_device_get_next(ped_dev) )
    {
	Device* dev = new Device(ped_dev);
	list_dev_.push_back(dev);
    }
}

Devices::~Devices()
{
    list<Device*>::iterator iter = list_dev_.begin();
    while( iter != list_dev_.end() )
    {
	delete *iter;
	iter++;
    }
    list_dev_.clear();
    ped_device_free_all();
}

bool Devices::is_empty()
{
    return list_dev_.empty();
}

void Devices::del(Device* dev)
{
    assert( dev != NULL );
    delete dev;
}

void Devices::print()
{
    list<Device*>::const_iterator iter;
    for( iter = list_dev_.begin() ; iter != list_dev_.end(); iter ++ )
    {
	fprintf(stderr, "model: %s\n", (*iter)->model() );
	fprintf(stderr, "path: %s\n", (*iter)->path() );
	fprintf(stderr, "length: %lld\n", (*iter)->length() );
	fprintf(stderr, "sector size: %lld\n", (*iter)->sector_size() );
	fprintf(stderr, "physical sector size: %lld\n", (*iter)->physical_sector_size() );
	fprintf(stderr, "read_only: %s\n", (*iter)->is_read_only() ? "yes" : "no" );
	/*
	fprintf(stderr, "external mode: %s\n", (*iter)->in_external_mode() ? "yes" : "no" );
	fprintf(stderr, "busy?: %s\n", (*iter)->is_busy() ? "yes" : "no" );
	*/
	fprintf(stderr, "\n\n" );
    }
}

/* count of devices in.
 */
int Devices::count()
{
    return list_dev_.size();
}

/* get the device by index.
 * index begin from 0.
 */
Device* Devices::device(int index)
{
    list<Device*>::iterator iter = list_dev_.begin();
    while( index != 0 && iter != list_dev_.end() )
    {
	index--;
	iter++;
    }
    if ( iter == list_dev_.end() )
	return NULL;
    else
	return (*iter);
}

/* get the device by path.
 */
Device* Devices::device(const char* path)
{
    list<Device*>::iterator iter = list_dev_.begin();
    while( iter != list_dev_.end() )
    {
	if( !strcmp( path, (*iter)->path() ) )
	    return *iter;
	iter++;
    }
    return NULL;
}

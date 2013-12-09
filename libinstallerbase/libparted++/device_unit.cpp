#include "devices.h"
#include "device_unit.h"

DeviceUnit::DeviceUnit(Device* dev)
{
    dev_ = dev;
    pdev_ = dev->pdev();
    ped_unit_set_default(PED_UNIT_COMPACT);
}

Device* DeviceUnit::owner_dev()
{
    return dev_;
}

PedUnit DeviceUnit::get_default()
{
    return ped_unit_get_default();
}

void DeviceUnit::set_default(PedUnit unit)
{
    ped_unit_set_default(unit);
}

void DeviceUnit::set_default(const char * unit_name)
{
    PedUnit unit = ped_unit_get_by_name(unit_name);
    if ( unit == -1 ) { // wrong name
	fprintf(stderr, "DeviceUnit::set_default with %s, wrong unit name.\n", 
		unit_name);
	return;
    }
    ped_unit_set_default(unit);
}

const char* DeviceUnit::format(PedSector num)
{
    return ped_unit_format( pdev_, num );
}

const char* DeviceUnit::format(PedSector num, PedUnit unit)
{
    return ped_unit_format_custom( pdev_, num, unit );
}

const char* DeviceUnit::format(PedSector num, const char* unit_name)
{
    PedUnit unit = ped_unit_get_by_name( unit_name );
    if ( unit == -1 ) { // wrong name
	fprintf(stderr, "DeviceUnit::set_default with %s, wrong unit name.\n", 
		unit_name);
	return NULL;
    }
    return ped_unit_format_custom( pdev_, num, unit );
}

PedUnit DeviceUnit::get_by_name(const char* name)
{
    return ped_unit_get_by_name( name );
}

const char* DeviceUnit::get_name(PedUnit unit)
{
    return ped_unit_get_name(unit);
}

long long DeviceUnit::unit_size(PedUnit unit)
{
    return ped_unit_get_size( pdev_, unit );
}

long long DeviceUnit::unit_size(const char * unit_name)
{
    PedUnit unit = ped_unit_get_by_name( unit_name );
    if ( unit == -1 ) { // wrong name
	fprintf(stderr, "DeviceUnit::set_default with %s, wrong unit name.\n", 
		unit_name);
	return -1;
    }
    return ped_unit_get_size( pdev_, unit );
}

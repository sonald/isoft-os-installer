#include "partition.h"
#include "partlist.h"
#include "parttable.h"
#include "devices.h"
#include <cassert>

Partition::Partition(PartitionList* plist, PedPartition* part)
{
    plist_ = plist;
    part_ = part;
}

Partition::~Partition()
{
}

PartitionList* Partition::owner_plist()
{
    return plist_;
}

/* create a partition.
 */
/*
bool Partition::created(PedDisk* disk, PedPartitionType type, PedFileSystemType* fs_type, 
	PedSector start, PedSector end)
{
    assert( part_ == NULL );

    PedPartition* part_ = ped_partition_new(disk, type, fs_type, start, end);
    disk_ = disk;
    return ( part_ == NULL ) ? false : true ;
}
*/

const char* Partition::fs_type_name()
{
    if ( part_->fs_type == NULL )
	if ( (part_->type & PED_PARTITION_EXTENDED) || (part_->type & PED_PARTITION_FREESPACE) )
	    return "";
	else
	    return "Unknown";
    else
	return part_->fs_type->name;
}

/* this name is not the class data member, name.
 * Only can setName() when partition_table support this feature.
 */
bool Partition::set_name(const char* name)
{
    return ped_partition_set_name(part_, name) ? true : false;
}

void Partition::print()
{
    printf ("%-10s %02d %-10s %-10s %02d  %s %-16lld = (%lld->%lld) Addr:%p \n", 
	    path(), type(), type_name(), fs_type_name(), num(), length_str(), length(),  start(), end(), part_ );
}

const char* Partition::str_sector(PedSector sec)
{
    Device* dev = owner_plist()->owner_ptable()->owner_dev();
    long long sector_size = dev->sector_size();
    long long byte = sec * sector_size;
    PedUnit unit;
    if ( byte > PED_TERABYTE_SIZE )
	unit = PED_UNIT_TERABYTE;
    else if ( byte > PED_GIGABYTE_SIZE )
	unit = PED_UNIT_GIGABYTE;
    else if ( byte > PED_MEGABYTE_SIZE )
	unit = PED_UNIT_MEGABYTE;
    else if ( byte > PED_KILOBYTE_SIZE )
	unit = PED_UNIT_KILOBYTE;
    else
	unit = PED_UNIT_BYTE;

    return ped_unit_format_custom( dev->pdev(), sec, unit);
}

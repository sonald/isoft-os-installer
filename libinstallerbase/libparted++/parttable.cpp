#include <cstdio>
#include <cassert>
#include "devices.h"
#include "parttable.h"
#include "partlist.h"

PartitionTable::PartitionTable(Device* dev)
{
    dev_        = dev;
    pdev_ 	= dev->pdev();
    disk_ 	= NULL;
    disk_type_ 	= NULL;
    part_list_ 	= NULL;
}

PartitionTable::~PartitionTable()
{
    if ( disk_ )
        ped_disk_destroy( disk_ );
    if ( part_list_ )
        delete part_list_;
}

PedDisk* PartitionTable::pdisk()
{
    return disk_;
}

Device* PartitionTable::owner_dev()
{
    return dev_;
}

/******************************************************
 * table functions
 * ***************************************************/
bool PartitionTable::check()
{
    return ped_disk_check( disk_ ) ? true : false ;
}

bool PartitionTable::commit_to_dev()
{
    return ped_disk_commit_to_dev( disk_ ) ? true : false ;
}

bool PartitionTable::commit_to_os()
{
    return ped_disk_commit_to_os( disk_ ) ? true : false ;
}

// commit to dev and os.
bool PartitionTable::commit()
{
    return ped_disk_commit( disk_ ) ? true : false ;
}

bool PartitionTable::isExist()
{
    PedDiskType* disk_type = ped_disk_probe( pdev_ );
    if ( disk_type == NULL )
	return false;
    else
	return true;
}

/* read the parttable.
 * if no parttable exist in device, disk_ and disk_type_ == NULL. 
 */
bool PartitionTable::read()
{
    if (disk_type_ && disk_ && part_list_) { // already loaded
        return true;
    }

    if ( disk_ )
        ped_disk_destroy( disk_ );
    if ( part_list_ )
        delete part_list_;

    disk_type_ = ped_disk_probe( pdev_ );
    if ( disk_type_ == NULL )
        disk_ = NULL;
    else
        disk_ = ped_disk_new( pdev_ );

    if ( disk_ != NULL ) {
        part_list_ = new PartitionList( this );
        return true;
    } else
        return false;
}

/* create the new fresh parttable with disk_type.
 * if failure, disk_ and disk_type_ will be NULL.
 */
bool PartitionTable::create(const char* disk_type)
{
    if ( disk_ ) 
	ped_disk_destroy( disk_ );
    if ( part_list_ )
	delete part_list_;

    disk_type_ = ped_disk_type_get(disk_type);
    if ( disk_type_ == NULL ) 	
	disk_ = NULL;
    else
	disk_ = ped_disk_new_fresh( pdev_, disk_type_ );

    if ( disk_ != NULL ) {
	part_list_ = new PartitionList( this );
	return true;
    } else
	return false;
}

/***************************************
 * feature functions
 **************************************/
bool PartitionTable::is_support_feature(PedDiskTypeFeature feature)
{
    return ped_disk_type_check_feature( disk_type_, feature) ? true : false ;
}

bool PartitionTable::is_support_extended_partition()
{
    return ped_disk_type_check_feature( disk_type_, PED_DISK_TYPE_EXTENDED) ? true : false ; 
}

bool PartitionTable::is_support_name_partition()
{
    return ped_disk_type_check_feature( disk_type_, PED_DISK_TYPE_PARTITION_NAME) ? true : false ;
}

int PartitionTable::max_primary_partition_count()
{
    return ped_disk_get_max_primary_partition_count( disk_ );
}

void PartitionTable::print_disk()
{
    fprintf(stderr, "PartitionTable::print_disk:\n");
    ped_disk_print( disk_ );
}

// This function is different to PartitionList.print().
// print the PedPartition info directly.
void PartitionTable::print_part_list()
{
    fprintf(stderr, "PartitionTable::print_part_list:\n");
    PedPartition* temp_part = NULL;
    while( (temp_part = ped_disk_next_partition( disk_ , temp_part )) != NULL ) {
	printf ("%02d %02d Addr:%p\n", temp_part->num, temp_part->type, temp_part);
    }
}


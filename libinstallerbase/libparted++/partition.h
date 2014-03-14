#ifndef _PARTITION_H_PARTED_PLUS_
#define _PARTITION_H_PARTED_PLUS_

#include <parted/parted.h>

class PartitionList;

class Partition 
{
public:
    Partition(PartitionList* plist, PedPartition* part =NULL);
    ~Partition();
    
    PartitionList* owner_plist();

    const char* name()				{ return ped_partition_get_name( part_ ); }
    char* path()				{ return ped_partition_get_path( part_ ); }
    bool is_busy()				{ return ped_partition_is_busy( part_ ) ? true : false; }
    bool is_active()				{ return ped_partition_is_active( part_ ) ? true : false ; }

    int num()					{ return part_->num; }
    PedPartitionType type()			{ return part_->type; }
    // isPrimary is true when partition is primary and primary free.
    bool isPrimary()                            { return ((part_->type & 0x01) == 0) ? true : false ; } 
    const char * type_name()			{ return ped_partition_type_get_name( part_->type ); }
    const char * fs_type_name();		
    bool has_flag(PedPartitionFlag flag) const;

    // return the string form of start, end and length 
    const char* start_str()                     { return str_sector(part_->geom.start); }
    const char* end_str()                       { return str_sector(part_->geom.end); }
    const char* length_str()                    { return str_sector(part_->geom.length); }
    PedSector start()				{ return part_->geom.start; }
    PedSector end()				{ return part_->geom.end; }
    PedSector length()				{ return part_->geom.length; }
    /*
    bool created(PedDisk* disk, 
	    PedPartitionType type, 
	    PedFileSystemType* fs_type, 
	    PedSector start, PedSector end);
    */
    
    bool set_name(const char* name);
    void print();

private:
    const char* str_sector(PedSector sector);

    PartitionList*      plist_;
    PedPartition* 	part_;
    friend class PartitionList;
};
#endif // _PARTITION_H_PARTED_PLUS_

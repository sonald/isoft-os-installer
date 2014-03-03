#ifndef _PART_TABLE_H_PARTED_PLUS_
#define _PART_TABLE_H_PARTED_PLUS_

#include <parted/parted.h>

class Device;
class PartitionList;

class PartitionTable 
{
public:
    friend class Device;

    PartitionTable(Device* dev);
    ~PartitionTable();

    Device*  owner_dev();
    PedDisk* pdisk();

    bool isExist();
    bool create(const char* disk_type);

    const char* type_name()		{ if (disk_type_) return disk_type_->name; else return NULL; }

    // feature support
    bool is_support_feature(PedDiskTypeFeature feature);
    bool is_support_extended_partition();
    bool is_support_name_partition();
    int  max_primary_partition_count();
    // table operation
    bool check();
    bool commit();
    bool commit_to_dev();
    bool commit_to_os();
    
    PartitionList* partlist()		{ return part_list_; }

    void print_disk();
    void print_part_list();

 private:
    Device*     dev_;
    PedDevice* 	pdev_;
    PedDisk* 	disk_;
    PedDiskType* disk_type_;
    
    PartitionList* part_list_;	// list of Partition, sync the list and internal part_list in PedDisk.
    				// Operations, etc add remove and edit, works on PartitionList.
                    
    bool read();
};
#endif //_PART_TABLE_H_PARTED_PLUS_

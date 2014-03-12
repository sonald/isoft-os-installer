#ifndef LIBPARTED_PLUS_PLUS_PARTLIST_H_
#define LIBPARTED_PLUS_PLUS_PARTLIST_H_

#include <parted/parted.h>

#include <list>

using namespace std;
class Partition;
class PartitionTable;

/* 
 * definition: 
 * num is 4 of /dev/sda4. 
 * index is the internal order of part-list. 
 * more info in docs.
 */
class PartitionList
{ 
 public:
    PartitionList(PartitionTable* ptable);
    ~PartitionList(); 
    
    PartitionTable* owner_ptable();

    // count of partitions
    const int count() const;

    // last num of partition
    const int last_part_num() const;

    // count of primary partitions, only the extended and primary had filesystem.
    const int count_primary() const;
    
    // count of primary partitions include freespace will be primary.
    const int count_primary_freeincluded();

    // the pointer of partition with num
    Partition* part_num(int num);

    // the pointer of partition with index
    Partition* part_index(int index);
    
    // the pointer of extended partition.
    Partition* part_extended();
    
    // whether extended partition exist.
    bool is_extended_exist();

    // the index of partition
    int find_index_of(Partition* part);

    // the index of partition with num
    int find_index_of(int num);
      
    // set the unit used by partition operations.
    void set_unit(PedUnit unit);
    void set_unit(const char* unit);
    PedUnit unit() 			{ return unit_; }

    // print the list of partitions.
    void print();
    
    // partition operations.
    // delete operation.
    bool delete_all();
    int  delete_num(int num);
    int  delete_index(int index);

    // add a new partition with length in freespace(index)
    // return the num of created part, 0 when failed.
    int add_by_length(int index,
		      const char* part_type_name,
		      const char* fs_type_name,
		      const char* length,
		      const char* unit_name = "compact",
              PedPartitionFlag flag = PED_PARTITION_LBA);
    /*
      bool add_by_length(int index,
      PedPartitionType part_type,
      PedFileSystemType* fs_type,
      PedSector length);
    */

    // add the whole free partition with index.
    // part_type_name: primary, logical, extended, free.
    // return the num of created partition, 0 when failed.
    int add_by_whole(int index,
		     const char* part_type_name,
		     const char* fs_type_name,
              PedPartitionFlag flag = PED_PARTITION_LBA);
   
    // misc
    bool set_part_fstype(int index,
			 const char* fs_type_name); /* useless */
    bool is_extended_empty();
    bool minimize_extended();

 private:
    int add_by_whole(int index,
		     PedPartitionType part_type,
		     PedFileSystemType* fs_type,
             PedPartitionFlag flag = PED_PARTITION_LBA);

    int add_by_length(int index,
		      const char* part_type_name,
		      const char* fs_type_name,
		      const char* length,
		      PedUnit unit,
              PedPartitionFlag flag = PED_PARTITION_LBA);
    int add_by_length(int index,
		      PedPartitionType part_type,
		      PedFileSystemType* fs_type,
		      const char* length,
		      PedUnit unit,
              PedPartitionFlag flag = PED_PARTITION_LBA);

    void build_list();
    void clear_list();
    void update_list();
 private:
    PartitionTable* ptable_;

    PedDevice* dev_;
    PedDisk* disk_;
    PedUnit unit_;				// suggest unit used by partition operations.

    list<Partition*> list_part_;
};
#endif // LIBPARTED_PLUS_PLUS_PARTLIST_H_

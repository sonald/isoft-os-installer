#include <stdio.h>
#include <parted++/parted++.h>
#include <parted/parted.h>

int main(int argc, char* argv[])
{
    PartedDevices devs;
    PartitionTable* ptable;
    Device* pdev;
    DeviceUnit* unit;
    
    pdev = devs.device( "/dev/sda" );
    fprintf(stderr, "sector size:%lld\n", pdev->sector_size() );
    ptable = pdev->parttable();

    if( !ptable->read() )
	exit(0);

    PartitionList* plist = ptable->partlist();
    plist->print();
    plist->delete_num(8);
    plist->print();
    int num = plist->add_by_length(6, "logical", "ext2", "1300M");
    plist->print();
    fprintf(stderr, "num:%d\n", num);
    Partition* part = plist->part_num(num);
    fprintf(stderr, "length:%s\n", part->start_str() );
}

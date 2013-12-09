#include <stdio.h>
#include <parted++/parted++.h>
#include <parted/parted.h>

int main(int argc, char* argv[])
{
    PartedDevices devs;
    PartitionTable* ptable;
    Device* pdev;
    
    pdev = devs.device( "/dev/sda" );
    ptable = pdev->parttable();

    if ( !ptable->read() ) {
	fprintf( stderr, "no partition table in /dev/sda ." );
	exit(0);
    }

    ptable->print_disk();
    ptable->print_part_list();

    PartitionList* plist = ptable->partlist();
    plist->print();
}

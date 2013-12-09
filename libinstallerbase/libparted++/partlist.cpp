#include "partition.h"
#include "partlist.h"
#include "parttable.h"
#include "devices.h"
#include <cassert>
#include <parted/debug.h>

/////////////////////////////////////////////////////////////////////////////
// static declarations. 
typedef int EMoves;
const EMoves MOVE_NO 	=0;
const EMoves MOVE_STILL	=1;
const EMoves MOVE_UP	=2;
const EMoves MOVE_DOWN	=4;
const int SECT_START	=0;
const int SECT_END	=-1;

static void snap_to_boundaries (PedGeometry* new_geom, PedGeometry* old_geom,
                    PedDisk* disk,
                    PedGeometry* start_range, PedGeometry* end_range);
static EMoves
prefer_snap (PedSector s, int what, PedGeometry* range, EMoves* allow,
             PedPartition* part, PedSector* dist);
static int
snap (PedSector* sector, PedSector new_sector, PedGeometry* range);

static int matched_type(const char* part_type_name);

static void print_geometry(PedGeometry* ); 
static void print_constraint(PedConstraint* );
static void print_alignment(PedAlignment* );

//////////////////////////////////////////////////////////////////////////////////////
PartitionList::PartitionList(PartitionTable* ptable)
{
    ptable_ = ptable;
    
    disk_ = ptable->pdisk();
    dev_ = ptable->owner_dev()->pdev();
    // unit_ = PED_UNIT_COMPACT;
    unit_ = PED_UNIT_MEGABYTE;
    build_list();
}

PartitionList::~PartitionList()
{
    clear_list();
}

PartitionTable* PartitionList::owner_ptable()
{
    return ptable_;
}

void PartitionList::build_list()
{
    long long length_littlefree = 10 * PED_MEGABYTE_SIZE; //10MB
    long long sector_size = dev_->sector_size;

    PedPartition* walk = NULL;
    while ( ( walk = ped_disk_next_partition(disk_, walk) ) != NULL ) {
	if ( walk->type & PED_PARTITION_METADATA )
	    continue;

	if ( walk->type & PED_PARTITION_FREESPACE ) {
	    if ( walk->geom.length * sector_size < length_littlefree )
		continue;
	}

	Partition* part = new Partition(this, walk);
	list_part_.push_back(part);
    }
}

void PartitionList::clear_list()
{
    list<Partition*>::iterator iter = list_part_.begin();
    while( iter != list_part_.end() )
    {
	delete *iter;
	iter++;
    }
    list_part_.clear();
}

/* call update when every time part list modified.
 */
void PartitionList::update_list()
{
    clear_list();
    build_list();
}

void PartitionList::set_unit(PedUnit unit)
{
    unit_ = unit;
}

void PartitionList::set_unit(const char* unit_name)
{
    PedUnit unit = ped_unit_get_by_name(unit_name);
    if ( unit != -1 )
	unit_ = unit;
}

void PartitionList::print()
{
    printf("part list index print.\n");
    printf("idx path       type          fs type    num size   sector size     start->end   addr of partition node\n");
    for ( int i =0 ; i< list_part_.size(); i++ )
    {
	Partition* temp_part = part_index(i);
	printf("%2d: ", i);
	temp_part->print();
    }
}

const int PartitionList::count() const
{
    list_part_.size();
}

const int PartitionList::last_part_num() const
{
    return ped_disk_get_last_partition_num( disk_ );
}

const int PartitionList::count_primary() const
{
    return ped_disk_get_primary_partition_count( disk_ );
}

const int PartitionList::count_primary_freeincluded()
{
    int num=0;
    for( int index=0; index < count() ; index++ ) {
	Partition* part = part_index( index );
	PedPartitionType type = part->type();
	if ( ( type == PED_PARTITION_NORMAL ) ||
	     ( type == PED_PARTITION_EXTENDED ) ||
	     ( type == PED_PARTITION_FREESPACE ) )
	    num++;
    }
    return num;
}


/* get the part with num.
 * num begin the count from 1.
 * returns: pointer to Partition Object; NULL, if no part with num.
 */
Partition* PartitionList::part_num(int num)
{
    assert( num > 0 );

    list<Partition*>::iterator iter = list_part_.begin();
    while( iter != list_part_.end() )
    {
	if ( (*iter)->num() == num )
	    return (*iter);
	iter++;
    }
    return NULL;
}

/* get the part with index.
 * index begin the count from 0.
 * returns: pointer to Partition Object; NULL, if index is out of range. 
 */
Partition* PartitionList::part_index(int index)
{
    assert( index >= 0 );
    list<Partition*>::iterator iter = list_part_.begin();
    while( index!=0 && iter != list_part_.end() ) {
	index--;
	iter++;
    }
    if ( iter == list_part_.end() )
	return NULL;
    else
	return *iter;
}

/*
 * return the pointer to extended partition.
 * return NULL if no extended partition.
 */
Partition* PartitionList::part_extended()
{
    list<Partition*>::iterator iter = list_part_.begin();
    while( iter != list_part_.end() ) {
	if ( (*iter)->type() & PED_PARTITION_EXTENDED ) {
	    return *iter;
	}
	iter++;
    }
    return NULL;
}

// whether extended partition is existed.
bool PartitionList::is_extended_exist()
{
    bool found = false;
    list<Partition*>::iterator iter = list_part_.begin();
    while( iter != list_part_.end() && !found ) {
	if ( (*iter)->type() & PED_PARTITION_EXTENDED ) {
	    found = true;
	}
	iter++;
    }
    return found;
}

// retval: -1 when fail, index when part found.
int PartitionList::find_index_of(Partition* part)
{
    assert( part != NULL );
    list<Partition*>::iterator iter = list_part_.begin();
    int index =0;
    while( iter != list_part_.end() ) {
	if( *iter == part )
	    return index;
	else {
	    index++;
	    iter++;
	}
    }
    if ( iter == list_part_.end() )
	return -1;
}

// retval: -1 when fail, index when num found.
int PartitionList::find_index_of(int num)
{
    assert( num > 0 );

    int index =0;
    list<Partition*>::iterator iter = list_part_.begin();
    while( iter != list_part_.end() ) {
	if ( (*iter)->num() == num ) {
	    return index;
	} else {
	    index++;
	    iter++;
	}
    }
    if ( iter == list_part_.end() )
	return -1;
}

/////////////////////////////////////////////////////////////////////////////////////
// delete function.
/* delete all partitions.
 */
bool PartitionList::delete_all()
{
    int ret = ped_disk_delete_all(disk_);
    if ( ret )
	update_list();
    return ret ? true : false;
}

/* delete the partition with num.
 * returns: 1 success, 0 not found or del failed, -1 extended part not empty.
 */
int PartitionList::delete_num(int num)
{
    assert( num > 0 );

    Partition* part = part_num(num);
    if ( part == NULL )
	return 0;
    // dont worry about PED_PARTITION_PROTECTED.
    // Not to del the non-empty extended partition.
    if ( ( part->type() & PED_PARTITION_EXTENDED ) && !is_extended_empty() )
	return -1;
    int ret = ped_disk_delete_partition( disk_, part->part_ );
    if ( ret ) {
	update_list();
	return 1;
    } else {
	return 0;
    }
}

/* delete the partition with index.
 * return: 1 success, 0 not found or del failed, -1 extended part not empty or freespace.
 */
int PartitionList::delete_index(int index)
{
    // find the part.
    list<Partition*>::iterator iter = list_part_.begin();
    while ( index != 0 && iter != list_part_.end() )
    {
	index--;
	iter++;
    }
    if ( iter == list_part_.end() )
	return 0;
    // not to del extended part.
    if ( ( (*iter)->type() & PED_PARTITION_EXTENDED ) && !is_extended_empty() )
	return -1;
    // not to del free part.
    if ( (*iter)->type() & PED_PARTITION_FREESPACE )
	return -1;
    int ret = ped_disk_delete_partition( disk_, (*iter)->part_ );
    if ( ret ) {
	update_list();
	return 1;
    } else { 
	return 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////
// add function.
//
/* add a new Partition(length) in freespace part(index).
 */
int PartitionList::add_by_length(int index,
				 const char* part_type_name,
				 const char* fs_type_name,
				 const char* length,
				 const char* unit_name)
{
    PedUnit unit = ped_unit_get_by_name( unit_name );
    if ( unit == -1 )
	return 0;
    return add_by_length( index, part_type_name, fs_type_name, length, unit );
}

int PartitionList::add_by_length(int index,
				 const char* part_type_name,
				 const char* fs_type_name,
				 const char* length,
				 PedUnit unit)
{
    assert( part_type_name != NULL );
    
    int part_type = ::matched_type(part_type_name);
    if ( part_type == -1 )
	return 0;

    PedFileSystemType* fs_type = NULL;
    if ( fs_type_name )
	fs_type = ped_file_system_type_get(fs_type_name);
    
    if ( part_type & PED_PARTITION_EXTENDED )
	fs_type = NULL;

    return add_by_length( index, (PedPartitionType)part_type, fs_type, length, unit);
}

int PartitionList::add_by_length(int index,
				 PedPartitionType part_type,
				 PedFileSystemType* fs_type,
				 const char* length,
				 PedUnit unit)
{
    Partition* part = part_index(index);

    // make sure part is free and the create operation is suitable for this part.
    if ( part == NULL )
	return 0;
    if ( ! ( part->type() & PED_PARTITION_FREESPACE ) )
	return 0;
    // NOT permit create non-logical part in extended freespace.
    if ( ( part->type() & PED_PARTITION_LOGICAL ) && !( part_type & PED_PARTITION_LOGICAL ) )
	return 0;
    // NOT permit create logical part in primary freespace.
    if ( !( part->type() & PED_PARTITION_LOGICAL ) && ( part_type & PED_PARTITION_LOGICAL ) )
	return 0;
    // not check the number of primary partition. left it to caller.

    char* str_start_sector = NULL;
    char* str_end_sector = NULL;

    // get the string of start location.
    str_start_sector = ped_unit_format_custom( dev_, part->start(), unit );

    // transfer length from string to sector.
    PedSector sector_length;
    PedGeometry* range_length;
    ped_unit_parse_custom( length, dev_, unit, &sector_length, &range_length);
    ped_geometry_destroy( range_length ); 

    // can't to create the partition bigger than free partition.
    if ( sector_length > part->length() )
	sector_length = part->length();

    // get the string of end location.
    str_end_sector = ped_unit_format_custom( dev_, part->start() + sector_length -1 , unit );

    PedSector start_sector =0, end_sector =0;
    PedGeometry *range_start = NULL, *range_end =NULL;

    // get the sector and range of start and end locations.
    ped_unit_parse_custom( str_start_sector, dev_, unit, &start_sector, &range_start);
    ped_unit_parse_custom( str_end_sector, dev_, unit, &end_sector, &range_end);

    /*** print the detail info of add operation. ***/
    fprintf(stderr, "add_by_length:\n");
    fprintf(stderr, "free:\t start(%lld), end(%lld), length(%lld).\n",
	    part->start(), part->end(), part->length() );
    fprintf(stderr, "create:\t length(%s, %lld), end(%lld).\n", 
	    length, sector_length, part->start()+sector_length-1);
    fprintf(stderr, "string:\t start(%s, %lld), end(%s, %lld)\n", 
	    str_start_sector, start_sector, str_end_sector, end_sector);    
    fprintf(stderr, "rstart:\t");
    print_geometry( range_start );
    fprintf(stderr, "rend:\t");
    print_geometry( range_end );
    if ( part->start() != start_sector )
	fprintf(stderr, "WARNING: start location is moved.\n");
    if ( (part->start()+sector_length-1) != end_sector )
	fprintf(stderr, "WARNING: end location is moved.\n");
    /*** end of print detail ***/
    
    free(str_start_sector);
    free(str_end_sector);

    // temp solution.
    if ( start_sector < part->start() )
	start_sector = part->start();
    if ( end_sector > part->end() )
	end_sector = part->end();

    // new partition.
    PedPartition* part_new = ped_partition_new( disk_, part_type, fs_type, start_sector, end_sector);
    if ( !part_new )
	goto error_part_new;

    /*** print the info of created partition. ***/
    fprintf(stderr, "created part info:\n");
    fprintf(stderr, "Type(%-2d), num(%-2d)\n", part_new->type, part_new->num );
    print_geometry( &(part_new->geom) );

    // snap part to boundary.
    ::snap_to_boundaries( &part_new->geom, NULL, disk_, range_start, range_end );
    /*** print the info after snap. ***/
    fprintf(stderr, "after snap:\n");
    fprintf(stderr, "Type(%-2d), num(%-2d)\n", part_new->type, part_new->num );
    print_geometry( &(part_new->geom) );

    // calculate the constraint.
    PedConstraint* user_constraint;
    user_constraint = ped_constraint_new( ped_alignment_any, ped_alignment_any,
					  range_start, range_end, 1, dev_->length );
    PedConstraint* dev_constraint;
    dev_constraint = ped_device_get_constraint( dev_ );
    PedConstraint* final_constraint;
    final_constraint = ped_constraint_intersect( user_constraint, dev_constraint );
    if( !final_constraint )
	goto error_final_constraint;

    int ret;
    ret = ped_disk_add_partition( disk_, part_new, final_constraint );
    if ( !ret ) {
	fprintf(stderr, "ADD PART AGAIN.\n");
	ret = ped_disk_add_partition( disk_, part_new, ped_constraint_any( dev_ ) );
    }
    if ( !ret ) {
	fprintf(stderr, "ADD PART FAILED.\n");
	goto error_add_partition;
    }
    /*** print the info after add. ***/
    fprintf(stderr, "ADD PART SUCCESSFUL.\n");
    fprintf(stderr, "Type(%-2d), num(%-2d)\n", part_new->type, part_new->num );
    print_geometry( &(part_new->geom) );
    
    ped_disk_print( disk_ );

    ped_partition_set_system( part_new, fs_type );
    if( ped_partition_is_flag_available( part_new, PED_PARTITION_LBA ) )
	ped_partition_set_flag( part_new, PED_PARTITION_LBA, 1);
    update_list();
    
    // cleanup
    ped_constraint_destroy(final_constraint);
    ped_constraint_destroy(user_constraint);
    ped_constraint_destroy(dev_constraint);
    if ( range_start )
	ped_geometry_destroy( range_start );
    if ( range_end )
	ped_geometry_destroy( range_end );
    return part_new->num;

 error_add_partition:
    ped_constraint_destroy(final_constraint);
 error_final_constraint:
    ped_constraint_destroy(user_constraint);
    ped_constraint_destroy(dev_constraint);
    ped_partition_destroy(part_new);
 error_part_new:
    if ( range_start )
	ped_geometry_destroy( range_start );
    if ( range_end )
	ped_geometry_destroy( range_end );
    return 0;
}

/* add the partition with exact length.
 * hide it because no one use it.
bool PartitionList::add_by_length(int index, 
				PedPartitionType part_type, 
				PedFileSystemType* fs_type,
				PedSector length)
{
    Partition* part = part_index(index);

    if ( part == NULL )
	return false;
    if ( ! ( part->type() & PED_PARTITION_FREESPACE ) )
	return false;
    // NOT permit create non-logical part in extended freespace.
    if ( ( part->type() & PED_PARTITION_LOGICAL ) && !( part_type & PED_PARTITION_LOGICAL ) )
	return false;
    if ( !( part->type() & PED_PARTITION_LOGICAL ) && ( part_type & PED_PARTITION_LOGICAL ) )
	return false;

    if ( length > part->length() )
	return false;

    PedDevice* dev = disk_->dev;
    PedSector start_sector = part->start();
    PedSector end_sector = start_sector + length - 1;
    PedGeometry* range_start = ped_geometry_new( dev, start_sector, 1 );
    PedGeometry* range_end = ped_geometry_new( dev, end_sector, 1 );

    PedPartition* part_new = ped_partition_new( disk_, part_type, fs_type, start_sector, end_sector );
    if ( !part_new )
	goto error_part_new;

    ::snap_to_boundaries( &part_new->geom, NULL, disk_, range_start, range_end );
    
    PedConstraint* user_constraint;
    user_constraint = ped_constraint_new( ped_alignment_any, ped_alignment_any,
	    						range_start, range_end, 1, dev->length );
    PedConstraint* dev_constraint;
    dev_constraint = ped_device_get_constraint( dev );
    PedConstraint* final_constraint;
    final_constraint = ped_constraint_intersect( user_constraint, dev_constraint );
    if( !final_constraint )
	goto error_final_constraint;

    int ret;
    ret = ped_disk_add_partition( disk_, part_new, final_constraint );
    if ( !ret )
	ret = ped_disk_add_partition( disk_, part_new, ped_constraint_any( dev ) );
    if ( !ret )
	goto error_add_partition;

    ped_partition_set_system( part_new, fs_type );
    if( ped_partition_is_flag_available( part_new, PED_PARTITION_LBA ) )
	    ped_partition_set_flag( part_new, PED_PARTITION_LBA, 1);
    update_list();
    // cleanup
    ped_constraint_destroy(final_constraint);
    ped_constraint_destroy(user_constraint);
    ped_constraint_destroy(dev_constraint);
    if ( range_start )
	ped_geometry_destroy( range_start );
    if ( range_end )
	ped_geometry_destroy( range_end );
    return true;

error_add_partition:
    ped_constraint_destroy(final_constraint);
error_final_constraint:
    ped_constraint_destroy(user_constraint);
    ped_constraint_destroy(dev_constraint);
    ped_partition_destroy(part_new);
error_part_new:
    if ( range_start )
	ped_geometry_destroy( range_start );
    if ( range_end )
	ped_geometry_destroy( range_end );
    return false;
}
*/

 /* convenient function for add_by_whole
  */
int PartitionList::add_by_whole(int index,
				const char * part_type_name,
				const char * fs_type_name)
{
    assert( part_type_name != NULL );

    int part_type = ::matched_type(part_type_name);
    if ( part_type == -1 )
	return 0;

    PedFileSystemType* fs_type = NULL;
    if ( fs_type_name )
	fs_type = ped_file_system_type_get(fs_type_name);
    
    if ( part_type & PED_PARTITION_EXTENDED )
	fs_type = NULL;

    return add_by_whole( index, (PedPartitionType)part_type, fs_type );
}

/* add the new Partition() with whole space in freespace part(index).
 */
int PartitionList::add_by_whole(int index, 
				PedPartitionType part_type, 
				PedFileSystemType* fs_type)
{
    Partition* part = part_index(index);
 
    if ( part == NULL )
	return 0;
    if ( ! ( part->type() & PED_PARTITION_FREESPACE ) )
	return 0;
    // NOT permit create non-logical part in extended freespace.
    if ( ( part->type() & PED_PARTITION_LOGICAL ) && !( part_type & PED_PARTITION_LOGICAL ) )
	return 0;
    if ( !( part->type() & PED_PARTITION_LOGICAL ) && ( part_type & PED_PARTITION_LOGICAL ) )
	return 0;

    // get the start and end sector.
    char* str_start_sector =NULL;
    char* str_end_sector =NULL;

    str_start_sector = ped_unit_format_custom( dev_, part->start(), unit_ );
    str_end_sector = ped_unit_format_custom( dev_, part->end(), unit_ );

    PedSector start_sector =0, end_sector =0;
    PedGeometry *range_start =NULL, *range_end =NULL;

    ped_unit_parse_custom( str_start_sector, dev_, unit_, &start_sector, &range_start);
    ped_unit_parse_custom( str_end_sector, dev_, unit_, &end_sector, &range_end);

    /*** print the detail info of add operation. ***/
    fprintf(stderr, "add_by_whole:\n");
    fprintf(stderr, "free:\t start(%lld), end(%lld).\n",
	    part->start(), part->end() );
    fprintf(stderr, "string:\t start(%s, %lld), end(%s, %lld)\n", 
	    str_start_sector, start_sector, str_end_sector, end_sector);    
    fprintf(stderr, "rstart:\t");
    print_geometry( range_start );
    fprintf(stderr, "rend:\t");
    print_geometry( range_end );
    if ( part->start() != start_sector )
	fprintf(stderr, "WARNING: start location is moved.\n");
    if ( part->end() != end_sector )
	fprintf(stderr, "WARNING: end location is moved.\n");
    /*** end of print detail ***/

    // temp solution.
    if ( start_sector < part->start() )
	start_sector = part->start();
    if ( end_sector > part->end() )
	end_sector = part->end();

    free(str_start_sector);
    free(str_end_sector);

    // new partition.
    PedPartition* part_new = ped_partition_new( disk_, part_type, fs_type, start_sector, end_sector);
    if ( !part_new )
	goto error_part_new;

    /*** print the info of created partition. ***/
    fprintf(stderr, "created part info:\n");
    fprintf(stderr, "Type(%-2d), num(%-2d)\n", part_new->type, part_new->num );
    print_geometry( &(part_new->geom) );

    // snap part to boundary.
    ::snap_to_boundaries( &part_new->geom, NULL, disk_, range_start, range_end );

    /*** print the info after snap. ***/
    fprintf(stderr, "after snap:\n");
    fprintf(stderr, "Type(%-2d), num(%-2d)\n", part_new->type, part_new->num );
    print_geometry( &(part_new->geom) );

    // calculate the constraint.
    PedConstraint* user_constraint;
    user_constraint = ped_constraint_new( ped_alignment_any, ped_alignment_any, 
					  range_start, range_end, 1, dev_->length );

    PedConstraint* dev_constraint;
    dev_constraint = ped_device_get_constraint( dev_ );
    PedConstraint* final_constraint;
    final_constraint = ped_constraint_intersect( user_constraint, dev_constraint );
    if ( ! final_constraint )
	goto error_final_constraint;

    // add the partition into partition table.
    int ret;
    ret = ped_disk_add_partition( disk_, part_new, final_constraint );
    if ( !ret ) {
	fprintf(stderr, "ADD PART AGAIN.\n");
	ret = ped_disk_add_partition( disk_, part_new, ped_constraint_any( dev_ ) );
    }
    if ( !ret ) {
	fprintf(stderr, "ADD PART FAILED.\n");
	goto error_add_partition;
    }
    /*** print the info after add. ***/
    fprintf(stderr, "ADD PART SUCCESSFUL.\n");
    fprintf(stderr, "Type(%-2d), num(%-2d)\n", part_new->type, part_new->num );
    print_geometry( &(part_new->geom) );

    /*** print the partition table. ***/
    ped_disk_print( disk_ );

    ped_partition_set_system( part_new, fs_type );
    if( ped_partition_is_flag_available( part_new, PED_PARTITION_LBA ) )
	ped_partition_set_flag( part_new, PED_PARTITION_LBA, 1);
    update_list();
    // cleanup 
    ped_constraint_destroy(final_constraint);
    ped_constraint_destroy(user_constraint);
    ped_constraint_destroy(dev_constraint);
    if ( range_start )
	ped_geometry_destroy( range_start );
    if ( range_end )
	ped_geometry_destroy( range_end );
    return part_new->num;

 error_add_partition:
    ped_constraint_destroy(final_constraint);
 error_final_constraint:
    ped_constraint_destroy(user_constraint);
    ped_constraint_destroy(dev_constraint);
    ped_partition_destroy(part_new);
 error_part_new:
    if ( range_start )
	ped_geometry_destroy( range_start );
    if ( range_end )
	ped_geometry_destroy( range_end );
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// extended partition function.
//
// set_part_fstyp useless
bool PartitionList::set_part_fstype(int index, const char* fs_type_name)
{
    Partition* part = part_index( index );
    if ( !part )
	return false;
    PedFileSystemType* fs_type = ped_file_system_type_get(fs_type_name);
    if ( !fs_type )
	return false;
    part->part_->fs_type = fs_type;
}

bool PartitionList::is_extended_empty()
{
    int num = last_part_num();
    int max = ped_disk_get_max_primary_partition_count( disk_ );
    return ( num > max ) ? false : true ;
    /*
    list<Partition*>::iterator iter = list_part_.begin();
    while ( iter != list_part_.end() )
    {
	if ( ( (*iter)->type() & PED_PARTITION_LOGICAL ) && ( ! ((*iter)->type() & PED_PARTITION_FREESPACE ) ) )
	    return false;
	iter++;
    }
    return true;
    */
}

bool PartitionList::minimize_extended()
{
    int ret = ped_disk_minimize_extended_partition( disk_ );
    if ( ret )
	update_list();
    return ret ? true : false;
}

///////////////////////////////////////////////////////////////////////////////////
// for debug: print ......
void print_constraint(PedConstraint* cont)
{
    fprintf(stderr, "start align: ");
    print_alignment(cont->start_align);
    fprintf(stderr, "end align:   ");
    print_alignment(cont->end_align);
    fprintf(stderr, "start range: ");
    print_geometry(cont->start_range);
    fprintf(stderr, "end range:   ");
    print_geometry(cont->end_range);
    fprintf(stderr, "min: %lld\t", cont->min_size);
    fprintf(stderr, "max: %lld\n", cont->max_size);
    printf("\n");
}

void print_alignment(PedAlignment* align)
{
    fprintf(stderr, "offset(%lld), grain_size(%lld)\n", align->offset, align->grain_size);
}

void print_geometry(PedGeometry* geom)
{
    fprintf(stderr, "%-16lld (%lld->%lld)\n", geom->length, geom->start, geom->end);
}

//////////////////////////////////////////////////////////////////////////////////////////
// local functions.
static int
matched_type(const char* part_type_name)
{
    const char * type_name;
    const PedPartitionType type_enum[] = { PED_PARTITION_NORMAL,
					PED_PARTITION_LOGICAL,
					PED_PARTITION_EXTENDED,
					PED_PARTITION_FREESPACE,
					PED_PARTITION_METADATA,
					PED_PARTITION_PROTECTED };
    for ( int i =0 ; i < sizeof(type_enum)/sizeof(PedPartitionType) ; i++ )
    {
	type_name = ped_partition_type_get_name( type_enum[i] );
	if ( ! strcmp ( type_name , part_type_name ) )
	    return type_enum[i];
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////
// snap functions cut from parted.c
/* This function changes "sector" to "new_sector" if the new value lies
 * within the required range.
 */
static int
snap (PedSector* sector, PedSector new_sector, PedGeometry* range)
{
        PED_ASSERT (ped_geometry_test_sector_inside (range, *sector), return 0);
        if (!ped_geometry_test_sector_inside (range, new_sector))
                return 0;
        *sector = new_sector;
        return 1;
}

/* Find the prefered way to adjust the sector s inside range.
 * If a move isn't allowed or is out of range it can't be selected.
 * what contains SECT_START if the sector to adjust is a start sector
 * or SECT_END if it's an end one.
 * The prefered move is to the nearest allowed boundary of the part
 * partition (if at equal distance: to start if SECT_START or to end
 * if SECT_END).
 * The distance is returned in dist.
 */
static EMoves
prefer_snap (PedSector s, int what, PedGeometry* range, EMoves* allow,
             PedPartition* part, PedSector* dist)
{
        PedSector up_dist = -1, down_dist = -1;
        PedSector new_sect;
        EMoves move;

        PED_ASSERT (what == SECT_START || what == SECT_END, return 0);

        if (!(*allow & (MOVE_UP | MOVE_DOWN))) {
                *dist = 0;
                return MOVE_STILL;
        }

        if (*allow & MOVE_UP) {
                new_sect = part->geom.end + 1 + what;
                if (ped_geometry_test_sector_inside (range, new_sect))
                        up_dist = new_sect - s;
                else
                        *allow &= ~MOVE_UP;
        }

        if (*allow & MOVE_DOWN) {
                new_sect = part->geom.start + what;
                if (ped_geometry_test_sector_inside (range, new_sect))
                        down_dist = s - new_sect;
                else
                        *allow &= ~MOVE_DOWN;
        }

        move = MOVE_STILL;
        if ((*allow & MOVE_UP) && (*allow & MOVE_DOWN)) {
                if (down_dist < up_dist || (down_dist == up_dist
                                            && what == SECT_START) )
                        move = MOVE_DOWN;
                else if (up_dist < down_dist || (down_dist == up_dist
                                                 && what == SECT_END) )
                        move = MOVE_UP;
                else
                        PED_ASSERT (0, return 0);
        } else if (*allow & MOVE_UP)
                move = MOVE_UP;
        else if (*allow & MOVE_DOWN)
                move = MOVE_DOWN;

        *dist = ( move == MOVE_DOWN ? down_dist :
                ( move == MOVE_UP   ? up_dist   :
                  0 ) );
        return move;
}

/* Snaps a partition to nearby partition boundaries.  This is useful for
 * gobbling up small amounts of free space, and also for reinterpreting small
 * changes to a partition as non-changes (eg: perhaps the user only wanted to
 * resize the end of a partition).
 *      Note that this isn't the end of the story... this function is
 * always called before the constraint solver kicks in.  So you don't need to
 * worry too much about inadvertantly creating overlapping partitions, etc.
 */
static void
snap_to_boundaries (PedGeometry* new_geom, PedGeometry* old_geom,
                    PedDisk* disk,
                    PedGeometry* start_range, PedGeometry* end_range)
{
        PedPartition*   start_part;
        PedPartition*   end_part;
        PedSector       start = new_geom->start;
        PedSector       end = new_geom->end;
        PedSector       start_dist = -1, end_dist = -1;
        EMoves          start_allow, end_allow, start_want, end_want;
        int             adjacent;

        start_want = end_want = MOVE_NO;
        start_allow = end_allow = MOVE_STILL | MOVE_UP | MOVE_DOWN;

        start_part = ped_disk_get_partition_by_sector (disk, start);
        end_part = ped_disk_get_partition_by_sector (disk, end);
        adjacent = (start_part->geom.end + 1 == end_part->geom.start);

        /* If we can snap to old_geom, then we will... */
        /* and this will enforce the snapped positions  */
        if (old_geom) {
                if (snap (&start, old_geom->start, start_range))
                        start_allow = MOVE_STILL;
                if (snap (&end, old_geom->end, end_range))
                        end_allow = MOVE_STILL;
        }

        /* If start and end are on the same partition, we */
        /* don't allow them to cross. */
        if (start_part == end_part) {
                start_allow &= ~MOVE_UP;
                end_allow &= ~MOVE_DOWN;
        }

        /* Let's find our way */
        start_want = prefer_snap (start, SECT_START, start_range, &start_allow,
                                  start_part, &start_dist );
        end_want = prefer_snap (end, SECT_END, end_range, &end_allow,
                                end_part, &end_dist );

        PED_ASSERT (start_dist >= 0 && end_dist >= 0, return);

        /* If start and end are on adjacent partitions,    */
        /* and if they would prefer crossing, then refrain */
        /* the farthest to do so. */
        if (adjacent && start_want == MOVE_UP && end_want == MOVE_DOWN) {
                if (end_dist < start_dist) {
                        start_allow &= ~MOVE_UP;
                        start_want = prefer_snap (start, SECT_START,
                                                  start_range, &start_allow,
                                                  start_part, &start_dist );
                        PED_ASSERT (start_dist >= 0, return);
                } else {
                        end_allow &= ~MOVE_DOWN;
                        end_want = prefer_snap (end, SECT_END,
                                                end_range, &end_allow,
                                                end_part, &end_dist );
                        PED_ASSERT (end_dist >= 0, return);
                }
        }

        /* New positions */
        start = ( start_want == MOVE_DOWN ? start_part->geom.start :
                ( start_want == MOVE_UP ? start_part->geom.end + 1 :
                  start ) );
        end = ( end_want == MOVE_DOWN ? end_part->geom.start - 1 :
              ( end_want == MOVE_UP ? end_part->geom.end :
                end ) );
        PED_ASSERT (ped_geometry_test_sector_inside(start_range,start), return);
        PED_ASSERT (ped_geometry_test_sector_inside (end_range, end), return);
        PED_ASSERT (start <= end,
                    PED_DEBUG (0, "start = %d, end = %d\n", start, end));
        ped_geometry_set (new_geom, start, end - start + 1);
}

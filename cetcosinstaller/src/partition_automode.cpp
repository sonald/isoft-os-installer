#include <QDebug>
#include <cassert>
#include "installer_global.h"
#include "partition_automode.h"

/* 
 * partition the disk with auto mode.
 * Strategy:
 *     1. List all the disks. Use the first disk if satisified.
 *     2. If disk space is enough, go to next step, else check the next disk.
 *     3. If disk is no partition table, create the partition table for 
 *        this disk. now get a disk with free parts.
 *     4. Make sure at least one free part is bigger than min-space needed by
 *        requirement. min size of swap partition is 512M, min size of / is 4G.
 *     5. If no extended partition, create one, and prepare to use it.
 *     6. If extended partition existed, find one free partition suit to part.
 *        The most suitable free part is 10G. use the free partition in extended
 *        partition first.
 *     7. Part the free partition. 
 *     7a free is Primary, if primary count is equal to 4 and no swap
 *        , use the swap file.
 *        // smart step, not in impl plan.
 *        if there is one partition size is perfect 512M or 1024M, use it as swap.
 *     7b free is Primary, if primary count is less than 4, part like logical.
 *     7c fire is Logical, if has swap, part one, else part into two. swap is the
 *        first one.
 *        if part space is bigger than 10G, set the size of swap part to 1024M, 
 *        else set to 512M.
 *        If part size is bigger than 10G, partition it.
 */

const long long AutoRootSize      = PED_GIGABYTE_SIZE * 4;
const long long AutoSwapMinSize   = PED_MEGABYTE_SIZE * 512;
const long long AutoSwapMaxSize   = PED_MEGABYTE_SIZE * 1024;
const long long AutoRootSwapSize  = AutoRootSize + AutoSwapMinSize;
const long long AutoPerfectSize   = PED_GIGABYTE_SIZE * 10;

/*
 * part the disk. if the current one can't be used, try the next.
 */
bool PartitionAutoMode::autoPartition()
{
    PartedDevices allDevs;
    bool found =false;
    int  i =0;
    while( i < allDevs.count() && !found ) {
	recordClean();
	m_rootpath = QString();
	m_partinfo = QString();
	
	m_dev = allDevs.device(i);
	qDebug() << "Disk" << i << ":" << m_dev->path();
	
	PedSector sizeDisk = m_dev->length();
	sizeDisk = sizeDisk * m_dev->sector_size();

	if ( sizeDisk > AutoRootSwapSize )
	    found = autoPartDisk();
	i++;
    }
    return found;
}

/*
 * part the disk. if no partition table, create it.
 */	
bool PartitionAutoMode::autoPartDisk()
{
    qDebug() << "Part the disk:" << m_dev->path();

    PartitionTable* ptable = m_dev->parttable();
    
    bool ret = ptable->read();
    if ( !ret ) {
	qDebug() << "Create the partition table.";
	ret = ptable->create("msdos");
	if ( !ret ) 
	    return ret;
	recordLabel( m_dev->path(), "msdos" );
	m_partinfo += QString( tr("Create the partition table on disk %1.\n") ).arg( m_dev->path() );
    }

    PartitionList* plist = ptable->partlist();

    // The issue of partition number supported by kernel.
    int partlimit = numSwapPart( plist ) ? 15 : 14 ;
    if ( plist->last_part_num() >= partlimit )
	return false;

    return autoPartFreePartitions(plist);
}

/*
 * part on disk which have some free partitions.
 */
bool PartitionAutoMode::autoPartFreePartitions(PartitionList* plist)
{
    int numSwap = numSwapPart( plist );
    long long sizeExpect;
    if ( numSwap ) 
	sizeExpect = AutoRootSize;
    else 
	sizeExpect = AutoRootSwapSize;

    if ( !hasSuitFree( plist, sizeExpect ) ) {
	qDebug() << "Not free partition is bigger than" << sizeExpect;
	return false;
    }

    int index;
    bool hasExtended = plist->is_extended_exist();
    bool canExtended;
    int countPrimary = plist->count_primary();
    
    // when countPrimary == 0, total disk is free. Do not use all space 
    // to create extended partition.
    canExtended = ( ( countPrimary < 4 ) && ( countPrimary > 0 ) );
    if ( !hasExtended && canExtended ) {
	int indexExt;
	indexExt = findMaxFree(plist);
	index = partExtended(plist, indexExt);
	if( index == -1 ) {
	    qDebug() << "create the extended partition failed.";
	    return false;
	}
    } else {
	index = findExpectFree(plist, numSwap);
    }

    //XXX: output the debug info.
    plist->print();
    qDebug() << "index:" << index;
    
    // hasSuitFree make sure a free part can be found. 
    assert( index != -1 );

    bool ret = false;
    ret = partExpectFree(plist, index, numSwap);
    return ret;
}

/*
 * Whether any free partition has required size.
 */
bool PartitionAutoMode::hasSuitFree(PartitionList* plist, long long size )
{
    bool suit = false;
    int  index = 0;
    int  count = plist->count();
    long long sizeSector = m_dev->sector_size();
    
    bool logicalOnly = ( plist->owner_ptable()->max_primary_partition_count()
			 == plist->count_primary() );

    while( index < count && !suit ) {
	Partition* part = plist->part_index( index );
	if ( ( part->type() & PED_PARTITION_FREESPACE ) // is free part
	     && !( part->isPrimary() && logicalOnly ) ) { // not isPrimary when logicalOnly
	    long long sizePart;
	    sizePart = part->length() * sizeSector;
	    
	    if ( sizePart > size )
		suit = true;
	}
	index++;
    }
    return suit;
}

/*
 * find the maximum partition for extended partition created.
 * return the index of the partition.
 */
int PartitionAutoMode::findMaxFree(PartitionList* plist)
{
    long long max = -1;
    int indexMax = -1;
    long long sizeSector = m_dev->sector_size();

    for( int index =0; index < plist->count(); index++ ) {
	Partition* part = plist->part_index( index );
	if ( !(part->type() & PED_PARTITION_FREESPACE) )
	    continue;

	long long sizePart;
	sizePart = part->length() * sizeSector;
	if ( sizePart > max ) {
	    max = sizePart;
	    indexMax = index;
	}
    }
    assert( indexMax != -1 );
    return indexMax;
}

/* 
 * create the extended partition, and return the index of free part in extended.
 * return -1 when failed.
 */
int PartitionAutoMode::partExtended(PartitionList* plist, int index)
{
    int ret;
    ret = createExtended(plist, index);
    
    if ( !ret )
	return -1;

    // get the index of free part in extended.
    int i =0;
    int indexExt = -1;
    int count = plist->count();
    while( i < count ) {
	Partition* part = plist->part_index( i );

	if ( part->type() & PED_PARTITION_EXTENDED ) {
	    indexExt = i;
	}
	
	if ( ( part->type() & PED_PARTITION_FREESPACE ) &&
	     ( part->type() & PED_PARTITION_LOGICAL ) ) {
	    assert( indexExt != -1 );
	    assert( i == indexExt + 1 );
	    return i;
	}
	i++;
    }
    // never here.
    return -1;
}

/*
 * findExpectFree is called when extended partiton is existed.
 *
 * return -1 if not found, else return the index.
 * find strategy:
 *   partBig: the index of min one which is bigger than 10G.
 *   partSmall: the index of max one which is smaller than 10G.
 *   1. if part > 10G and part < partBig, partBig = part.
 *   2. if part < 10G and part > partSmall, partSmall = part.
 *   3. if partBig != -1, partSuitMost = partBig, else = partSmall.
 *
 *   consider about the logical first situation:
 *   ( count_primary +1 == max_count_primary ).
 */
int PartitionAutoMode::findExpectFree(PartitionList* plist, int numSwap)
{
    long long sizeExpect;
    if ( numSwap ) 
	sizeExpect = AutoRootSize;
    else 
	sizeExpect = AutoRootSwapSize;

    int max_primary = plist->owner_ptable()->max_primary_partition_count();
    long long sizeSector = m_dev->sector_size();

    // use the logical partition when count of primary part equal to max count.
    // avoid using the large primary partition.
    bool logicalFirst = false;
    bool logicalOnly = false;
    if ( ( plist->count_primary() + 1 ) == max_primary ) 
	logicalFirst = true;
    if ( plist->count_primary() == max_primary )
	logicalOnly = true;
    
    int indexPartBig = -1;
    int indexPartSmall = -1;
    long long sizePartBig =0;
    long long sizePartSmall =0;
    bool logicalBig = false;
    bool logicalSmall = false;

    for( int index =0; index < plist->count(); index++ ) {
	Partition* part = plist->part_index( index );
	// not freespace, pass
	if ( !(part->type() & PED_PARTITION_FREESPACE) )
	    continue;
	
	// if part is primary free part and logical only, ignore it.
	if ( logicalOnly && ( part->type() == PED_PARTITION_FREESPACE ) )
	    continue;

	// space not enough, pass
	long long sizePart;
	sizePart = part->length() * sizeSector;
	if ( sizePart < sizeExpect )
	    continue;
	
	if ( sizePart > AutoPerfectSize ) {
	    // if logical first, then
	    // when logical big existed, ignore all primary partition.
	    // when encounter the first logical, use it.
	    // when else, compare the size as the not logical first.
	    if ( logicalFirst ) {
		// primary free
		if ( ( part->type() == PED_PARTITION_FREESPACE ) && logicalBig )
		    continue;

		// logical free
		if ( ( part->type() & PED_PARTITION_LOGICAL ) && !logicalBig ) {
		    sizePartBig = sizePart;
		    indexPartBig = index;
		    logicalBig = true;
		    continue;
		}
	    }
		
	    if ( ( sizePartBig > sizePart ) || ( sizePartBig == 0 ) ) {
		sizePartBig = sizePart;
		indexPartBig = index;
	    }
 
	} else {
	    // same as big.
	    if ( logicalFirst ) {
		if ( ( part->type() == PED_PARTITION_FREESPACE ) && logicalSmall )
		    continue;
		
		if ( ( part->type() & PED_PARTITION_LOGICAL ) && !logicalSmall ) {
		    sizePartSmall = sizePart;
		    indexPartSmall = index;
		    logicalSmall = true;
		    continue;
		}
	    }

	    if ( sizePartSmall < sizePart ) {
		sizePartSmall = sizePart;
		indexPartSmall = index;
	    }
	}
    }
    
    if ( logicalFirst ) {
	if ( logicalBig )
	    return indexPartBig;
	else if ( logicalSmall )
	    return indexPartSmall;
    }

    return ( indexPartBig != -1 ) ? indexPartBig : indexPartSmall ;
}

bool PartitionAutoMode::partExpectFree(PartitionList* plist, int index, int numSwap)
{
    Partition* part = plist->part_index(index);

    const char* parttype;
    if ( part->type() & PED_PARTITION_LOGICAL ) {
	parttype = "logical";
    } else {
	parttype = "primary";
    }
 
    bool ret = false;
    int max_primary = plist->owner_ptable()->max_primary_partition_count();

    if ( numSwap ) {
	// set the mount point for existed swap partition.
	const char* path = plist->part_num( numSwap )->path();
	qDebug() << "Swap partition path:" << path;
	recordMountPoint( path, "linux-swap", "linux-swap" );	
    } else if ( part->isPrimary() 
		&& ( max_primary == (plist->count_primary()+1) ) ) {
	// Do nothing. use the swap file.
    } else {
	// create the swap partition.
	ret = createSwap( plist, index, parttype );
	if ( ret ) {
	    index++;
	}	
    }

    // create the root partition.
    ret = createRoot( plist, index, parttype );

    return ret;
}

bool PartitionAutoMode::createSwap(PartitionList* plist, int index, const char* parttype)
{
    Partition* part = plist->part_index(index);
    long long sizePart = part->length();
    sizePart = sizePart * m_dev->sector_size();
    const char* devpath = m_dev->path();

    const char* swapLength;
    swapLength = swapSizeHint( sizePart );

    int num;
    num = plist->add_by_length( index, parttype, "linux-swap", swapLength);
    if ( num ) {
	part = plist->part_num( num );
	const char* path = part->path();

	recordLength( devpath, index, parttype, "linux-swap", swapLength);
	recordMountPoint( path, "linux-swap", "linux-swap" );
	m_partinfo += QString( tr("create the swap partition: %1.\n") ).arg( path );
    }
	
    return num;
}

bool PartitionAutoMode::createRoot(PartitionList* plist, int index, const char* parttype)
{
    int num=0;
    Partition* part = plist->part_index(index);
    long long sizePart = part->length();
    sizePart = sizePart * m_dev->sector_size();
    const char* devpath = m_dev->path();

    // dont divide into multi-partition when up to max number of primary partition.
    // if divide, the rest of free will not can be used.
    int max_primary = plist->owner_ptable()->max_primary_partition_count();
    if ( part->isPrimary() && ( max_primary == (plist->count_primary()+1) ) ) {
	num = plist->add_by_whole( index, parttype, "ext3");
	if ( num ) {
	    recordWhole( devpath, index, parttype, "ext3" );
	}
    } else if ( sizePart > 2 * AutoPerfectSize ) { // too big, so part to 10G.
	num = plist->add_by_length( index, parttype, "ext3", "10G");
	if ( num ) 
	    recordLength( devpath, index, parttype, "ext3", "10G");
    } else {
	num = plist->add_by_whole( index, parttype, "ext3");
	if ( num )
	    recordWhole( devpath, index, parttype, "ext3");
    }

    if ( num ) {
	const char* path = plist->part_num(num)->path();
	recordMountPoint( path, "/", "ext3");
	m_rootpath = path;
	m_partinfo += QString( tr("create the root partition: %1.\n") ).arg( path );
    }
    return num;
}

bool PartitionAutoMode::createExtended(PartitionList* plist, int index)
{
    int num;
    num = plist->add_by_whole( index, "extended", 0);
    const char* devpath = m_dev->path();
    if ( num ) {
	recordWhole( devpath, index, "extended", "" );
	const char* path = plist->part_num(num)->path();
	m_partinfo += QString( tr("create the extended partition: %1.\n") ).arg( path );
    }
    return num;
}

/*
 * return the number of swap partition, 0 when no swap part.
 */
int PartitionAutoMode::numSwapPart(PartitionList* plist)
{
    int index =0;
    while( index < plist->count() ) {
	Partition* part = plist->part_index( index );
	if ( !strcmp( part->fs_type_name(), "linux-swap") )
	    return part->num();
	index++;
    }
    // not found.
    return 0;	    
} 

const char* PartitionAutoMode::swapSizeHint(long long sizeDisk)
{
    if ( sizeDisk > AutoPerfectSize )
	return "1024M";
    else
	return "512M";
}

void PartitionAutoMode::recordClean()
{
    m_result.clear();
}

void PartitionAutoMode::recordLabel(const QString& devpath, const QString& labeltype)
{
    QStringList strListRecord;
    QString strRecord;
    
    strListRecord << "MakeLabel" << devpath << labeltype;
    strRecord = strListRecord.join(",");
    m_result << strRecord;
}
			      
void PartitionAutoMode::recordLength(const QString& devpath, int index, 
				     const QString& parttype, const QString& fstype, const QString& length)
{
    QStringList strListRecord;
    QString strRecord;
    QString strIndex;

    strIndex.setNum(index);
    strListRecord << "MakePartLength" << devpath << strIndex << parttype << fstype << length;
    strRecord = strListRecord.join(",");
    m_result << strRecord ;
}

void PartitionAutoMode::recordWhole(const QString& devpath, int index, 
				    const QString& parttype, const QString& fstype)
{
    QStringList strListRecord;
    QString strRecord;
    QString strIndex;

    strIndex.setNum(index);
    strListRecord << "MakePartWhole" << devpath << strIndex << parttype << fstype ;
    strRecord = strListRecord.join(",");
    m_result << strRecord ;
}

void PartitionAutoMode::recordMountPoint(const QString& partpath, const QString& mountpoint, const QString& fstype)
{
    QStringList strListRecord;
    QString strRecord;
    
    strListRecord << "SetMountPoint" << partpath << mountpoint << fstype ;
    strRecord = strListRecord.join(",");
    m_result << strRecord ;
}

void PartitionAutoMode::writeXML()
{
    QStringList::const_iterator itor;
    for (itor = m_result.constBegin(); itor != m_result.constEnd(); ++itor) {
	qDebug() << (*itor);
	QStringList params = itor->split(",");
	QString cmd = params.at(0);
	if ( cmd == "MakeLabel" ) {
	    assert( params.size() == 3 );
	    g_engine->cmdMakeLabel( params.at(1).toAscii(), params.at(2).toAscii() );
	} else if ( cmd == "MakePartWhole" ) {
	    assert( params.size() == 5 );
	    g_engine->cmdMakePartWhole( params.at(1).toAscii(), params.at(2).toAscii(),
					params.at(3).toAscii(), params.at(4).toAscii() );
	} else if ( cmd == "MakePartLength" ) {
	    assert( params.size() == 6 );
	    g_engine->cmdMakePartLength( params.at(1).toAscii(), params.at(2).toAscii(),
					 params.at(3).toAscii(), params.at(4).toAscii(),
					 params.at(5).toAscii() );

	} else if ( cmd == "SetMountPoint" ) {
	    assert( params.size() == 4 );
	    g_engine->cmdSetMountPoint( params.at(1).toAscii(), params.at(2).toAscii(),
					params.at(3).toAscii() );
	} else {
	    assert( false );
	}
    }

    qDebug() << m_rootpath;
    qDebug() << m_partinfo;
}

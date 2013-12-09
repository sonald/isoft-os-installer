#include "parted_devices.h"
#include <cassert>
#include <limits.h>

PartedDevices::PartedDevices()
{
    remove_readonly();
    remove_zerolength();
    remove_blacklist();
}

void PartedDevices::remove_readonly()
{
    list<Device*>::iterator iter = list_dev_.begin();
    while( iter != list_dev_.end() )
    {
	if ( (*iter)->is_read_only() ) {
	    del(*iter);
	    iter = list_dev_.erase( iter );
	} else
	    iter++;
    }
}

void PartedDevices::remove_zerolength()
{
    list<Device*>::iterator iter = list_dev_.begin();
    while( iter != list_dev_.end() )
    {
	if ( (*iter)->length() == 0 ) {
	    del(*iter);
	    iter = list_dev_.erase( iter );
	} else
	    iter++;
    }
}

void PartedDevices::remove_blacklist()
{
    list<Device*>::iterator iter = list_dev_.begin();
    while( iter != list_dev_.end() )
    {
	char path[PATH_MAX];
	strcpy( path, (*iter)->path() );
	assert( ((*iter)->path()) [0] == '/' );
	assert( ((*iter)->path()) [1] == 'd' );
	assert( ((*iter)->path()) [2] == 'e' );
	assert( ((*iter)->path()) [3] == 'v' );
	assert( ((*iter)->path()) [4] == '/' );

	char* p = strrchr( (*iter)->path(), '/' );
	int len = p - ( (*iter)->path() + 4 ) -1;
	if ( len < 0 )
	    len = strlen( (*iter)->path() ) - 5 ;

	strncpy( path, (*iter)->path() + 5, len );
	path[len+1] = '\0';

	if ( in_blacklist( path ) ) {
	    del(*iter);
	    iter = list_dev_.erase( iter );
	} else {
	    iter ++;
	}
    }
}

// match the path with blacklist
// if blacklist string is prefix of path, is matched.
bool PartedDevices::in_blacklist(const char* path)
{
    const char* blacklist[] = {
	"mapper",
	"sr"
    };

    int num_black = sizeof(blacklist)/sizeof(char*);
    for( int i =0; i < num_black; i++ )
    {
	int len = strlen( blacklist[i] );
	int j =0;
	while( j < len ) {
	    if ( path[j] != blacklist[i][j] )
		break;
	    else
		j++;
	}
	if ( j == len )
	    return true;
    }
    return false;
}

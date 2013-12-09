#include <stdio.h>
#include <parted++/parted++.h>
#include <parted/parted.h>

void print_devices();
void print_parted_devices();

int main(int argc, char* argv[])
{
    print_devices();
}

void print_devices()
{
    Devices devs;
    devs.print();
}

void print_parted_devices()
{
    PartedDevices devs;
    devs.print();
}


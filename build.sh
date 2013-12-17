#!/usr/bin/env bash

function check_status () {
    if [ $? -ne 0 ]; then
        echo -e "\033[31m${1} build error\033[0m"
        exit $?
    fi
}

PROJECT_ROOT=`pwd`
function do_build() {
    BUILD_ROOT=$PROJECT_ROOT/build

    mkdir -p $BUILD_ROOT

    for sub in libdiskswidget libinstallerbase cetcosinstaller; do
        echo build $sub
        mkdir -p $BUILD_ROOT/$sub
        pushd $BUILD_ROOT/$sub
        if [ "$sub" == libinstallerbase ]; then
            # cmake build
            cmake ../../$sub
        else
            if which qtchooser > /dev/null; then
                qmake -qt=qt4 ../../$sub
            else
                qmake-qt4 ../../$sub
            fi
        fi
        make -j2 > /dev/null
        check_status $sub
        sudo make install > /dev/null
        popd
    done
} 

function do_makedist() {
    DIST_ROOT=$PROJECT_ROOT/dist
    rm -rf $DIST_ROOT
    mkdir -p $DIST_ROOT
    pushd $DIST_ROOT

    for sub in libinstallerbase libdiskswidget cetcosinstaller; do
        echo makedist $sub
        cp -r ../$sub $sub-0.1
        tar jcf ${sub}-0.1.tar.bz2 $sub-0.1
        cp $sub-0.1/${sub}.spec .
        rm -rf $sub-0.1
    done
    popd
}

case $1 in 
    dist*)
        do_makedist
        ;;

    *)
        do_build
        ;;
esac


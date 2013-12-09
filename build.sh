#!/usr/bin/env bash

PROJECT_ROOT=`pwd`
BUILD_ROOT=$PROJECT_ROOT/build

mkdir -p $BUILD_ROOT

for sub in libdiskswidget libinstallerbase installer; do
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
    make -j2
    popd
done


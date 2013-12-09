#!/bin/sh

for i in translations/*.ts
do 
lupdate-qt4 -verbose games src -ts $i
done

#!/bin/bash
mkdir -p xkozak15
cp -r IMP xkozak15/
cp dokumentace.pdf xkozak15/dokumentace.pdf
cp dokumentace.tex xkozak15/dokumentace.tex
tar -czvf xkozak15.tar.gz xkozak15/ --exclude-vcs
rm -rf xkozak15/

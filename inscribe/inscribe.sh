#! /bin/bash
IPELET_NAME="inscribe"
DIR_PATH=$(dirname ${BASH_SOURCE})
BUILD_PATH=${DIR_PATH}"/../../../build/ipelets"

cd  ${DIR_PATH}
make clean IPEPREFIX=/usr/local
make IPEPREFIX=/usr/local
cd ~
cp ${BUILD_PATH}/${IPELET_NAME}.so ~
cp ${DIR_PATH}/${IPELET_NAME}.lua ~
mv ${IPELET_NAME}.* ~/.ipe/ipelets
cd ${DIR_PATH}
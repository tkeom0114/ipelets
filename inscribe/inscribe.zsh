#! /bin/zsh
IPELET_NAME="inscribe"
DIR_PATH=$(dirname $(realpath %N) )
BUILD_PATH=${DIR_PATH}"/../../../build/ipelets"

echo ${BASH_SOURCE}
cd  ${DIR_PATH}
make clean IPEPREFIX=/usr/local
make IPEPREFIX=/usr/local
cd ~
cp ${BUILD_PATH}/${IPELET_NAME}.so ~
cp ${DIR_PATH}/${IPELET_NAME}.lua ~
mv ${IPELET_NAME}.* ~/.ipe/ipelets
cd ${DIR_PATH}
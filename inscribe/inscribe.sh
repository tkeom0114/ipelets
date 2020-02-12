#! /bin/bash
IPELET_NAME="inscribe"

cd  ~/ipe-7.2.13/src/ipelets/${IPELET_NAME}
make IPEPREFIX=/usr/local
cd ~
cp ~/ipe-7.2.13/build/ipelets/${IPELET_NAME}.so ~
cp ~/ipe-7.2.13/src/ipelets/${IPELET_NAME}/${IPELET_NAME}.lua ~
mv ${IPELET_NAME}.* ~/.ipe/ipelets
cd ~/ipe-7.2.13/src/ipelets/${IPELET_NAME}
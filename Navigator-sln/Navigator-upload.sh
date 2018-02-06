#!/bin/bash
rsync -ratlzp --progress --update --rsh="sshpass -p temppwd ssh -o StrictHostKeyChecking=no -l debian" \
../libamex-bus-dev \
../libamex-cstrutil-dev \
../libamex-entry-dev \
../libamex-gpio-dev \
../libamex-license-dev \
../libamex-mmcid-dev \
../libamex-module-adis16405-dev \
../libamex-module-bmp280-dev \
../libamex-module-neom8ngps-dev \
../libamex-module-serialout-dev \
../libamex-sucmder-dev \
../libNavout \
../libqmodules \
../libSimpleCrypt \
../Navigator-app  \
../Navigator-sln  \
192.168.1.139:~

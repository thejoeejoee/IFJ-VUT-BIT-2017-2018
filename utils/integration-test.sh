#!/usr/bin/env bash

git clone https://github.com/thejoeejoee/VUT-FIT-IFJ-2017-toolkit.git toolkit;
LOGINS=$'xkobel02\nxtravi99'
echo "$LOGINS" | python3 toolkit/test.py --command-timeout 5 -v -e $2 $1
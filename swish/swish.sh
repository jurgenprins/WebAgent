#!/bin/sh
/usr/local/bin/swish -S fs -c /usr/local/wn/swish/swish.conf
/usr/local/bin/swish -D /usr/local/wn/swish/swish.index > /usr/local/wn/swish/swish.dump
/usr/local/wn/bin/wnindex /usr/local/wn/swish/swish.dump 25 > /usr/local/wn/swish/swish.words
/bin/rm -f /usr/local/wn/swish/swish.dump

#! /bin/sh

cc simplefs.c `pkg-config fuse --cflags --libs` -o simplefs

#!/usr/bin/env bash

find $1 -L -type l -mtime +7 -exec echo "{}" \;

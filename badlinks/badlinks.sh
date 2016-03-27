#!/usr/bin/env bash

find $0 -L -type l -mtime +7 -exec echo "{}" \;

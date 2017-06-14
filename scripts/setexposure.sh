#!/bin/bash
v4l2-ctl --set-ctrl exposure_auto=1 -d 2
v4l2-ctl --set-ctrl exposure_absolute=$1 -d 2

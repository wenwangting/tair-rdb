#!/bin/bash
/opt/tair_rdb_bin/tair.sh stop_cs
/opt/tair_rdb_bin/tair.sh stop_ds
sleep 2 
rm /home/caster/tair_rdb_bin/etc -rf
cp -rf /home/caster/tair_rdb_bin/* /opt/tair_rdb_bin
rm -f /opt/tair_rdb_bin/logs/*
/opt/tair_rdb_bin/tair.sh start_ds
/opt/tair_rdb_bin/tair.sh start_cs
sleep 3

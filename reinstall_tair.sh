#!/bin/bash
/opt/tair_rdb_bin/tair.sh stop_cs
/opt/tair_rdb_bin/tair.sh stop_ds
sleep 2 
cp -rf /home/caster/tair_rdb_bin/* /opt/tair_rdb_bin
rm -f /opt/tair_rdb_bin/logs/*
rm -f /opt/tair_rdb_bin/data/data/*
/opt/tair_rdb_bin/tair.sh start_ds
/opt/tair_rdb_bin/tair.sh start_cs
sleep 10 

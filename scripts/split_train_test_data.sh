#!/usr/bin/env bash

# Figure out the paths.
script_path=`readlink -f $0`
script_dir=`dirname $script_path`
app_dir=`dirname $script_dir`
progname=split_train_test_data
prog_path=${app_dir}/build/tools/${progname}

dataset_path="${app_dir}/data/nyt/"
docs_file="nytimes.dat.bin.shuffled"
percent=10

echo "Split train/test data"

GLOG_logtostderr=1 \
   $prog_path \
   --path ${dataset_path} \
   --docs ${docs_file} \
   --percent ${percent}

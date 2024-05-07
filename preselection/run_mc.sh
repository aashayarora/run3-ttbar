#!/bin/bash

./bin/runAnalysis --input input/dy_ceph.json --output dy.root
./bin/runAnalysis --input input/tt_ceph.json --output tt.root
cp ~/nvme/output/dy.root ~/nvme/output/tt.root ~/ceph/output/

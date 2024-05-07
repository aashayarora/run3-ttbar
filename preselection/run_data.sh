#!/bin/bash

./bin/runAnalysis --input input/egamma_ceph.json --output egamma.root --data
./bin/runAnalysis --input input/muon_ceph.json --output muon.root --data
cp ~/nvme/output/egamma.root ~/nvme/output/muon.root ~/ceph/output/


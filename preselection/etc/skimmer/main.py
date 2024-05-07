import ROOT
import pathlib
from dask_jobqueue.htcondor import HTCondorCluster
from dask.distributed import Client, LocalCluster
import argparse
from glob import glob

def get_cluster(condor=False):
    if condor:
        print("Using HTCondorCluster")
        _CLUSTER = HTCondorCluster(cores=2, 
                    memory="1GB", 
                    disk="1GB",
                    death_timeout='60',
                    nanny=False,
                    job_extra_directives={
                        "+DESIRED_Sites": "T2_US_UCSD",
                    },
                    log_directory="./log")
        _CLUSTER.scale(jobs=64)
    else:  
        print("Using LocalCluster")
        _CLUSTER = LocalCluster(n_workers=128,
                    processes=True,
                    threads_per_worker=1,
                    nanny=False,
                    memory_limit='auto')
    return _CLUSTER

def run_dataframe(sample, output, use_condor=False):
    with get_cluster(use_condor) as cluster, Client(cluster) as client:
        df = ROOT.RDF.Experimental.Distributed.Dask.RDataFrame("Events", sample, daskclient=client)
        # selections
        all_cols = df.GetColumnNames()
        selected_cols = [col for col in all_cols if "Electron" in str(col) or "Muon" in str(col) or "Jet" in str(col) or "Tau" in str(col) or "GenPart" in str(col) or "Generator" in str(col) or "FatJet" in str(col) or "MET" in str(col) or "event" in str(col) or "run" in str(col) or "luminosityBlock" in str(col) or "LHE" in str(col) or "Weight" in str(col) or "Flag" in str(col) or "SubJet" in str(col) or ("HLT" in str(col) and ("Ele" in str(col) or "Mu" in str(col))) or "PV" in str(col) or "Pileup" in str(col)]
        df = df.Define("goodElectrons", "Electron_pt > 10 && abs(Electron_eta) < 2.5") \
               .Define("goodMuons", "Muon_pt > 10 && abs(Muon_eta) < 2.4") \
               .Define("nGoodElectrons", "nElectron == 0 ? 0 : Sum(goodElectrons)") \
               .Define("nGoodMuons", "nMuon == 0 ? 0 : Sum(goodMuons)")
          
        df = df.Filter("nGoodElectrons >= 1 || nGoodMuons >= 1")

        df.Snapshot("Events", output, selected_cols)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--use_condor", action="store_true")
    args = parser.parse_args()

    sample_list_data = lambda sample : glob(f"/ceph/cms/store/data/Run2022*/{sample}/NANOAOD/22Sep2023-v*/*")
    samples = sample_list_data("EGamma")
    
    for i, sample in enumerate(samples):
        print(sample)
        tmp = sample.split("/ceph/cms/store/data/")[1].split("/")
        pathlib.Path(f"/data/userdata/aaarora/run3skims/{tmp[0]}/{tmp[1]}/{(i+1)*1000}").mkdir(parents=True, exist_ok=True)

        output = f"/ceph/cms/store/user/aaarora/output/"

        run_dataframe(sample + "/*.root", output, args.use_condor)
import ROOT
import pathlib
import argparse
from glob import glob
import multiprocessing as mp

def run_dataframe(sample, output):
    print(f"running on {sample}")
    df = ROOT.RDataFrame("Events", sample)
    # selections
    all_cols = df.GetColumnNames()
    selected_cols = [col for col in all_cols if "Electron" in str(col) or "Muon" in str(col) or "Jet" in str(col) or "Tau" in str(col) or "GenPart" in str(col) or "Generator" in str(col) or "FatJet" in str(col) or "MET" in str(col) or "event" in str(col) or "run" in str(col) or "luminosityBlock" in str(col) or "LHE" in str(col) or "Weight" in str(col) or "Flag" in str(col) or "SubJet" in str(col) or ("HLT" in str(col) and ("Ele" in str(col) or "Mu" in str(col))) or "PV" in str(col) or "Pileup" in str(col)]
    df = df.Define("goodElectrons", "Electron_pt > 7 && abs(Electron_eta) < 2.5") \
            .Define("goodMuons", "Muon_pt > 5 && abs(Muon_eta) < 2.4") \
            .Define("nGoodElectrons", "nElectron == 0 ? 0 : Sum(goodElectrons)") \
            .Define("nGoodMuons", "nMuon == 0 ? 0 : Sum(goodMuons)")
        
    df = df.Filter("nGoodElectrons >= 1 || nGoodMuons >= 1")

    df.Snapshot("Events", output, selected_cols)
    print(f"wrote {output}")

if __name__ == "__main__":
    sample_list_data = lambda sample, era : glob(f"/ceph/cms/store/data/{era}/{sample}/NANOAOD/22Sep2023-v2/*") or glob(f"/ceph/cms/store/data/{era}/{sample}/NANOAOD/22Sep2023-v1/*")
    samples = sample_list_data("EGamma", "Run2022E") + sample_list_data("Muon", "Run2022E") + sample_list_data("MuonEG", "Run2022E") + sample_list_data("EGamma", "Run2022F") + sample_list_data("Muon", "Run2022F") + sample_list_data("MuonEG", "Run2022F") + sample_list_data("EGamma", "Run2022G") + sample_list_data("Muon", "Run2022G") + sample_list_data("MuonEG", "Run2022G") + sample_list_data("EGamma", "Run2022H") + sample_list_data("Muon", "Run2022H") + sample_list_data("MuonEG", "Run2022H")
    
    for i, sample in enumerate(samples):
        print(sample)
        tmp = sample.split("/ceph/cms/store/data/")[1].split("/")
        pat = "/".join(tmp[:-1])
        pathlib.Path(f"/data/userdata/aaarora/run3skims/{pat}").mkdir(parents=True, exist_ok=True)

        # parallelize run_dataframe over files in the sample

        pool = mp.Pool(32)
        pool.starmap(run_dataframe, [(file, f"/data/userdata/aaarora/run3skims/{pat}/output_{idx}.root") for idx, file in enumerate(glob(sample + "/*.root"))])

import ROOT
import pathlib
import argparse
from glob import glob
import multiprocessing as mp

def run_dataframe(sample, output):
    print(f"running on {sample}")
    try:
        df = ROOT.RDataFrame("Events", sample)
        # selections
        all_cols = df.GetColumnNames()
        selected_cols = [col for col in all_cols if "Electron" in str(col) or "Muon" in str(col) or "Jet" in str(col) or "Tau" in str(col) or "GenPart" in str(col) or "Generator" in str(col) or "FatJet" in str(col) or "MET" in str(col) or "event" in str(col) or "run" in str(col) or "luminosityBlock" in str(col) or "LHE" in str(col) or "Weight" in str(col) or "Flag" in str(col) or "SubJet" in str(col) or ("HLT" in str(col) and ("Ele" in str(col) or "Mu" in str(col))) or "PV" in str(col) or "Pileup" in str(col)]
        df = df.Define("goodElectrons", "Electron_pt > 10 && abs(Electron_eta) < 2.5 && Electron_cutBased >= && Electron_convVeto == 1") \
                .Define("nGoodElectrons", "nElectron == 0 ? 0 : Sum(goodElectrons)") \
                .Define("goodMuons", "Muon_pt > 10 && abs(Muon_eta) < 2.4 && Muon_looseId == 1") \
                .Define("nGoodMuons", "nMuon == 0 ? 0 : Sum(goodMuons)") \
                .Filter("nGoodElectrons > 0 || nGoodMuons > 0")

        df.Snapshot("Events", output, selected_cols)
        print(f"wrote {output}")
    except Exception as e:
        print(e)
        print(f"FAILED ON FILE {sample}, {output}")
        pass

if __name__ == "__main__":
    sample_list_data_ = lambda sample, era : glob(f"/ceph/cms/store/data/{era}/{sample}/NANOAOD/22Sep2023-v2/*") or glob(f"/ceph/cms/store/data/{era}/{sample}/NANOAOD/22Sep2023-v1/*")
    sample_list_mc = glob(f"/ceph/cms/store/user/aaarora/Run3Summer22EENanoAODv12/DYto2L-2Jets_MLL-50_TuneCP5_13p6TeV_amcatnloFXFX-pythia8/NANOAODSIM/130X_mcRun3_2022_realistic_postEE_v6-v2/*")
    sample_list_data = sample_list_data_("EGamma", "Run2022E") + sample_list_data_("Muon", "Run2022E") + sample_list_data_("MuonEG", "Run2022E") + sample_list_data_("EGamma", "Run2022F") + sample_list_data_("Muon", "Run2022F") + sample_list_data_("MuonEG", "Run2022F") + sample_list_data_("EGamma", "Run2022G") + sample_list_data_("Muon", "Run2022G") + sample_list_data_("MuonEG", "Run2022G")
    
    for i, sample in enumerate(sample_list_data):
        print(sample)
        tmp = sample.split("/ceph/cms/store/data/")[1].split("/")
        pat = "/".join(tmp[:-1])
        pathlib.Path(f"/data/userdata/aaarora/run3skims/{pat}").mkdir(parents=True, exist_ok=True)
            
        # parallelize run_dataframe over files in the sample
        pool = mp.Pool(mp.cpu_count())
        pool.starmap(run_dataframe, [(file, f"/data/userdata/aaarora/run3skims/{pat}/output_{idx}.root") for idx, file in enumerate(glob(sample + "/*.root"))])

    # for i, sample in enumerate(sample_list_mc):
    #     print(sample)
    #     tmp = sample.split("/ceph/cms/store/user/aaarora/")[1].split("/")
    #     pat = "/".join(tmp[:-1])
    #     pathlib.Path(f"/data/userdata/aaarora/run3skims/{pat}").mkdir(parents=True, exist_ok=True)
            
    #     # parallelize run_dataframe over files in the sample
    #     pool = mp.Pool(32)
    #     pool.starmap(run_dataframe, [(file, f"/data/userdata/aaarora/run3skims/{pat}/output_{idx}.root") for idx, file in enumerate(glob(sample + "/*.root"))])


    print("DONE!")
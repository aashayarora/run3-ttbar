# %%
from glob import glob
import uproot

# %%
def get_total_weights(files):
    total = 0
    for file in glob(files):
        with uproot.open(file) as f:
            total += f.get("Runs")["genEventSumw"].array()[0]
    return total

# %%
DY = "/ceph/cms/store/mc/Run3Summer22EENanoAODv12/DYto2L-2Jets_MLL-50_TuneCP5_13p6TeV_amcatnloFXFX-pythia8/NANOAODSIM/130X_mcRun3_2022_realistic_postEE_v6-v2/*/*.root"
TT = "/ceph/cms/store/mc/Run3Summer22EENanoAODv12/TTto2L2Nu_TuneCP5_13p6TeV_powheg-pythia8/NANOAODSIM/130X_mcRun3_2022_realistic_postEE_v6-v2/*/*.root"
DY_E = "/ceph/cms/store/mc/Run3Summer22EENanoAODv12/DYto2L-2Jets_MLL-50_TuneCP5_13p6TeV_amcatnloFXFX-pythia8/NANOAODSIM/130X_mcRun3_2022_realistic_postEE_v6-v2/2520000/*.root"
# %%
print(get_total_weights(DY_E))
# 3804828879906.292

# %%
print(get_total_weights(TT))
# 6822815546.3937
# %%
int_lumi = 26671.7
int_lumi_E = 5807.0
int_lumi_F = 17781.9
int_lumi_G = 3082.8

DY_total = 3804828879906.292
DY_total_E = 354062582433.812
DY_xsec = 6688.0

TT_total = 6822815546.3937
TT_xsec = 98.096
# %%
xsec_weight = lambda xsec, int_lumi, total: xsec * int_lumi / total 
DY_weight = xsec_weight(DY_xsec, int_lumi, DY_total)
TT_weight = xsec_weight(TT_xsec, int_lumi, TT_total)
DY_weight_E = xsec_weight(DY_xsec, int_lumi_F, DY_total_E)

print('{:.20f}'.format(DY_weight_E))
print('{:.20f}'.format(DY_weight))
print('{:.20f}'.format(TT_weight))

# # %%
# import ROOT as r

# # %%
# df = r.RDataFrame("Events", "../output/mc.root")
# # %%
# df.Display(["weight", "pileup_weight", "electron_scale_factors", "muon_scale_factors", "xsec_weight"], 100).Print()
# # %%
# df2 = r.RDataFrame("Events", "/home/users/aaarora/ceph/Run3Summer22EENanoAODv12/DYto2L-2Jets_MLL-50_TuneCP5_13p6TeV_amcatnloFXFX-pythia8/NANOAODSIM/130X_mcRun3_2022_realistic_postEE_v6-v2/2520000/f7187457-ce24-4509-8f43-b196990bd532.root")
# # %%
# df2.Display(["genWeight"], 100).Print()
# %%

# import ROOT as r

# # %%
# df = r.RDataFrame("Events", "/ceph/cms/store/data/Run2022G/Muon/NANOAOD/22Sep2023-v1/30000/*.root")

# # %%
# df.Define("WeirdJetPhi", "Jet_phi[Jet_phi < -3.141602]").Filter("WeirdJetPhi.size() != 0").Display(["run", "event", "WeirdJetPhi"],10).Print()

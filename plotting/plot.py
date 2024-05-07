#!/usr/bin/env python

# %%
from dataclasses import dataclass
import multiprocessing as mp

import matplotlib.pyplot as plt
import mplhep as hep
hep.style.use(hep.style.CMS)

import ROOT as r
from uproot.pyroot import from_pyroot
import hist
# %%
@dataclass
class MyHist:
    var: str
    xlabel: str
    binning: tuple
    hist_data: hist.Hist = hist.Hist
    hist_bkg: hist.Stack = hist.Stack
    hist_ratio: hist.Hist = hist.Hist

# %%
hists = [
    MyHist("leading_lepton_pt", r"Leading Lepton $p_T$ (GeV)", (50, 0, 200)),
    MyHist("subleading_lepton_pt", r"Subleading Lepton $p_T$ (GeV)", (50, 0, 200)),
    MyHist("leading_lepton_eta", r"Leading Lepton $\eta$", (50, -2.5, 2.5)),
    MyHist("subleading_lepton_eta", r"Subleading Lepton $\eta$", (50, -2.5, 2.5)),
    MyHist("leading_lepton_phi", r"Leading Lepton $\phi$", (50, -3.14, 3.14)),
    MyHist("subleading_lepton_phi", r"Subleading Lepton $\phi$", (50, -3.14, 3.14)),
    MyHist("leading_jet_pt", r"Leading Jet $p_T$ (GeV)", (50, 0, 200)),
    MyHist("subleading_jet_pt", r"Subleading Jet $p_T$ (GeV)", (50, 0, 200)),
    MyHist("leading_jet_eta", r"Leading Jet $\eta$", (50, -2.5, 2.5)),
    MyHist("subleading_jet_eta", r"Subleading Jet $\eta$", (50, -2.5, 2.5)),
    MyHist("leading_jet_phi", r"Leading Jet $\phi$", (50, -3.14, 3.14)),
    MyHist("subleading_jet_phi", r"Subleading Jet $\phi$", (50, -3.14, 3.14)),
    MyHist("nGJet", r"nJets", (10, 0, 10)),
    MyHist("m_ll", r"$M_{ll}$ (GeV)", (40, 80, 120)),
    MyHist("m_mumu", r"$M_{\mu\mu}$ (GeV)", (40, 80, 120)),
    MyHist("m_ee", r"$M_{ee}$ (GeV)", (40, 80, 120)),
    MyHist("m_jj", r"$M_{jj}$ (GeV)", (50, 0, 500)),
    MyHist("pt_jj", r"${p_T}_{jj}$ (GeV)", (50, 0, 500)),
    MyHist("delta_eta_jj", r"$\Delta \eta_{jj}$", (50, 0, 5)),
    MyHist("n_pvs", "Num Primary Vertices", (50, 10, 60)),
]

# %%
data = ["/data/userdata/aaarora/output/egamma.root", "/data/userdata/aaarora/output/muon.root"]
mc = ["/data/userdata/aaarora/output/dy.root", "/data/userdata/aaarora/output/tt.root"]

df_data = r.RDataFrame("Events", data)
df_bkg = [r.RDataFrame("Events", output) for output in mc]

# %%
def plot(histogram):
    hist_data = df_data.Histo1D((histogram.var, histogram.var, *histogram.binning), histogram.var).GetValue()
    hist_bkgs = [df_bkg_i.Histo1D((histogram.var, histogram.var, *histogram.binning), histogram.var, "weight").GetValue() for df_bkg_i in df_bkg]

    hist_data.Sumw2()
    [hist.Sumw2() for hist in hist_bkgs]

    hist_ratio = hist_data.Clone()
    hist_bkg = hist_bkgs[0].Clone()
    [hist_bkg.Add(hist) for hist in hist_bkgs[1:]]
    hist_ratio.Divide(hist_bkg)

    # print(f"data: {hist_data.Integral()}")
    # print(f"bkg: {hist_bkg.Integral()}")

    histogram.hist_data = from_pyroot(hist_data).to_hist()
    # histogram.hist_bkg = from_pyroot(hist_bkgs[0]).to_hist()
    histogram.hist_bkg = hist.Stack.from_dict(
        {
            "DY": from_pyroot(hist_bkgs[0]).to_hist(), 
            "tt": from_pyroot(hist_bkgs[1]).to_hist()
        })
    histogram.hist_ratio = from_pyroot(hist_ratio).to_hist()

    fig, ax = plt.subplots(2, 1, gridspec_kw={"height_ratios": (4, 1)})
    hep.cms.label("Preliminary", data=True, lumi=26.67, year=2022, ax=ax[0])
    hep.histplot(histogram.hist_data, label="Data", ax=ax[0], histtype="errorbar", color="black")
    # hep.histplot(histogram.hist_bkg, label="MC", ax=ax[0])
    histogram.hist_bkg.plot(ax=ax[0], stack=True, histtype="fill")
    hep.histplot(histogram.hist_ratio, color="black", ax=ax[1], histtype="errorbar")
    ax[0].legend()
    ax[0].set_xlabel("")
    ax[1].set_xlabel(histogram.xlabel)
    ax[0].set_ylabel("Events")
    ax[1].set_ylabel("Data / MC")
    ax[1].set_ylim(0.8, 1.2)
    ax[1].axhline(1, color="black", linestyle="--")

    plt.savefig(f"/home/users/aaarora/phys/analysis/ttbar/plotting/plots/{histogram.var}.png")
    plt.close()
# %%
if __name__ == "__main__":
    mp.Pool(16).map(plot, hists)
# %%

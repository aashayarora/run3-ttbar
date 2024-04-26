# %%
import ROOT as r
import uproot
from dataclasses import dataclass

import matplotlib.pyplot as plt
import mplhep as hep
hep.style.use(hep.style.CMS)

import hist

# %%
@dataclass
class MyHist:
    var: str
    xlabel: str
    binning: tuple
    hist_data: hist.Hist = hist.Hist
    hist_sig: hist.Hist = hist.Hist
    hist_ratio: hist.Hist = hist.Hist

# %%
hists = [
    MyHist("VBSJet1_pt", r"Leading Jet $p_T$ (GeV)", (25, 0, 500)),
    MyHist("VBSJet2_pt", r"Subleading Jet $p_T$ (GeV)", (25, 0, 500)),
    MyHist("ST", r"$S_T$", (20, 1000, 3000))
]

# %%
df_data = r.RDataFrame("Events", "../preselection/output/data.root")
df_sig = r.RDataFrame("Events", "../preselection/output/data.root")

for histogram in hists:
    hist_data = df_data.Histo1D((histogram.var, histogram.var, *histogram.binning), histogram.var).GetValue()
    hist_sig = df_sig.Histo1D((histogram.var, histogram.var, *histogram.binning), histogram.var).GetValue()

    hist_data.Sumw2()
    hist_sig.Sumw2()

    hist_ratio = hist_data.Clone()
    hist_ratio.Divide(hist_sig)

    histogram.hist_data = uproot.pyroot.from_pyroot(hist_data).to_hist()
    histogram.hist_sig = uproot.pyroot.from_pyroot(hist_sig).to_hist()
    histogram.hist_ratio = uproot.pyroot.from_pyroot(hist_ratio).to_hist()

# %%
for histogram in hists:
    fig, ax = plt.subplots(2, 1, gridspec_kw={"height_ratios": (4, 1)})
    hep.cms.label("Preliminary", data=True, lumi=50, year=2017, ax=ax[0])
    hep.histplot(histogram.hist_data, label="Data", ax=ax[0], histtype="errorbar")
    hep.histplot(histogram.hist_sig, label="Signal", ax=ax[0])
    hep.histplot(histogram.hist_ratio, color="black", ax=ax[1], histtype="errorbar")
    ax[0].legend()
    ax[0].set_xlabel("")
    ax[1].set_xlabel(histogram.xlabel)
    ax[0].set_ylabel("Events")
    ax[1].set_ylabel("Data / MC")
    ax[1].set_ylim(0.5, 1.5)
    ax[1].axhline(1, color="black", linestyle="--")

    plt.savefig(f"plots/{histogram.var}.png")
# %%

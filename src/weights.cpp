#include "main.hpp"

namespace Weights {
    RNode xsecWeights(RNode df) {
        double dy_xsec = 762.1; //pb
        double int_lumi = 3082.75;
        double n_events = 604079041.227;
        double weight = dy_xsec * int_lumi / n_events;
        auto calcWeight = [weight](float genWeight){return genWeight * weight;};
        return df.Define("weights", calcWeight, {"genWeight"});
    }

    RNode goodRun(RNode df, json jf) {
        auto extract_from_gj = [jf](unsigned int run, unsigned int lumi) {
            std::string run_str = std::to_string(run);
            if (jf.contains(run_str)) { for (auto i : jf[run_str]) { if (lumi >= i[0]) {if (i[1] == -1 || lumi <= i[1]) { return 1.;}}} return 0.;}
            else {return 0.;}
        };
        return df.Define("weights", extract_from_gj, {"run", "luminosityBlock"});
    }

    RNode primaryVerticesNormalization(RNode df_mc) {
        auto th_mc = *(df_mc.Histo1D({"", "", 100, 0, 100}, "Pileup_nTrueInt", "weights"));

        TFile *f = new TFile("configs/pileup_2022G.root");
        TH1F *h_pileup = (TH1F*)f->Get("pileup");

        h_pileup->Scale(1. / h_pileup->Integral(0,-1));
        th_mc.Scale(1. / th_mc.Integral(0,-1));
        h_pileup->Divide(&th_mc);

        auto calcPVWeight = [h_pileup](float numPV, double weights) {return weights * h_pileup->GetBinContent((int)numPV+1);};
        return df_mc.Define("weights_after_pv", calcPVWeight, {"Pileup_nTrueInt", "weights"});
    }
}
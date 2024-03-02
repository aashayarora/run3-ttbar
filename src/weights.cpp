#include "common.hpp"

namespace Weights {
    RNode xsecWeights(RNode df) {
        double dy_xsec = 98.88; //pb
        double int_lumi = 21007; // pb^-1
        double n_events = 1417357887.633;
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

    RNode leptonScaleFactors(RNode df){
        auto electron_cset = CorrectionSet::from_file("configs/scalefactors/electron.json");
        auto electronscalefactors = electron_cset->at("Electron-ID-SF");

        auto electron_eval_correction = [electronscalefactors] (float eta, float pt) {
            return electronscalefactors->evaluate({"2022Re-recoE+PromptFG", "sf", "Loose", eta, pt});
        };

        auto muon_cset = CorrectionSet::from_file("configs/scalefactors/muon.json");
        auto muonscalefactors = muon_cset->at("NUM_IsoMu24_DEN_CutBasedIdMedium_and_PFIsoMedium");

        auto muon_eval_correction = [muonscalefactors] (float eta, float pt) {
            return muonscalefactors->evaluate({eta, pt, "nominal"});
        };

        return df.Define("electron_scale_factors", electron_eval_correction, {"electron_eta", "electron_pt"})
            .Define("muon_scale_factors", muon_eval_correction, {"muon_eta", "muon_pt"})
            .Redefine("weights", [] (double weights, double electron_scale_factors, double muon_scale_factors) {return weights * electron_scale_factors * muon_scale_factors;}, {"weights", "electron_scale_factors", "muon_scale_factors"});
    }

    RNode pileupCorrection(RNode df){
        auto cset = CorrectionSet::from_file("configs/pileup/puWeights.json");
        auto pileupweights = cset->at("Collisions2022_359022_362760_eraEFG_GoldenJson");

        auto eval_correction = [pileupweights] (float ntrueint, double weights) {
            return weights * pileupweights->evaluate({ntrueint, "nominal"});
        };

        return df.Redefine("weights", eval_correction, {"Pileup_nTrueInt", "weights"});
    }
}
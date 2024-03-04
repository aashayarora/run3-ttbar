#include "common.hpp"

namespace Weights {
    RNode xsecWeights(RNode df, double xsec, double int_lumi, double n_events) {
        double weight = xsec * int_lumi / n_events;
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
        auto electron_cset = CorrectionSet::from_file("configs/scalefactors/electron/electron.json");
        auto electronscalefactors = electron_cset->at("Electron-ID-SF");

        auto electron_eval_correction = [electronscalefactors] (const RVec<float>& eta, const RVec<float>& pt) {
            return electronscalefactors->evaluate({"2022Re-recoE+PromptFG", "sf", "Medium", eta[0], pt[0]});
        };

        auto muon_cset_ID = CorrectionSet::from_file("configs/scalefactors/muon/ScaleFactors_Muon_Z_ID_ISO_2022_EE_schemaV2.json");
        auto muonscalefactors_ID = muon_cset_ID->at("NUM_MediumID_DEN_TrackerMuons");

        auto muon_eval_correction_ID = [muonscalefactors_ID] (const RVec<float>& eta, const RVec<float>& pt) {
            return muonscalefactors_ID->evaluate({abs(eta[0]), pt[0], "nominal"});
        };

        auto muon_cset_ISO = CorrectionSet::from_file("configs/scalefactors/muon/ScaleFactors_Muon_Z_ID_ISO_2022_EE_schemaV2.json");
        auto muonscalefactors_ISO = muon_cset_ISO->at("NUM_MediumMiniIso_DEN_MediumID");

        auto muon_eval_correction_ISO = [muonscalefactors_ISO] (const RVec<float>& eta, const RVec<float>& pt) {
            return muonscalefactors_ISO->evaluate({abs(eta[0]), pt[0], "nominal"});
        };

        auto muon_cset_HLT = CorrectionSet::from_file("configs/scalefactors/muon/ScaleFactors_Muon_Z_HLT_2022_EE_abseta_pt_schemaV2.json");
        auto muonscalefactors_HLT = muon_cset_HLT->at("NUM_IsoMu24_DEN_CutBasedIdMedium_and_PFIsoMedium");

        auto muon_eval_correction_HLT = [muonscalefactors_HLT] (const RVec<float>& eta, const RVec<float>& pt) {
            return muonscalefactors_HLT->evaluate({abs(eta[0]), pt[0], "nominal"});
        };

        return df.Define("electron_scale_factors", electron_eval_correction, {"Electron_eta", "Electron_pt"})
            .Define("muon_scale_factors_ID", muon_eval_correction_ID, {"Muon_eta", "Muon_pt"})
            .Define("muon_scale_factors_ISO", muon_eval_correction_ISO, {"Muon_eta", "Muon_pt"})
            .Define("muon_scale_factors_HLT", muon_eval_correction_HLT, {"Muon_eta", "Muon_pt"})
            .Redefine("weights", [] (double weights, double electron_scale_factors, double muon_scale_factors_ID, double muon_scale_factors_ISO, double muon_scale_factors_HLT) {return weights * electron_scale_factors * muon_scale_factors_ID * muon_scale_factors_ISO * muon_scale_factors_HLT;}, {"weights", "electron_scale_factors", "muon_scale_factors_ID", "muon_scale_factors_ISO", "muon_scale_factors_HLT"});
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
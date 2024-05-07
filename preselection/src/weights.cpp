#include "weights.h"
    
RNode goodRun(lumiMask golden, RNode df){
    auto goldenjson = [golden](unsigned int &run, unsigned int &luminosityBlock){return golden.accept(run, luminosityBlock);};
    return df.Define("goodRun", goldenjson, {"run", "luminosityBlock"})
            .Filter("goodRun == 1", "PASSES GOLDEN JSON"); 
}

RNode pileupCorrection(correction::Correction::Ref cset_pileup, RNode df){
    auto eval_correction = [cset_pileup] (float ntrueint) {
        return cset_pileup->evaluate({ntrueint, "nominal"});
    };

    return df.Define("pileup_weight", eval_correction, {"Pileup_nTrueInt"});
}

RNode electronScaleFactors(correction::Correction::Ref cset_electron, RNode df){
    auto eval_correction_ID = [cset_electron] (const RVec<float> eta, const RVec<float> pt) {
        float electron_sf_weight = 1;
        if (eta.size() == 0) {
            return electron_sf_weight;
        }
        for (size_t i = 0; i < eta.size(); i++) {
            electron_sf_weight *= cset_electron->evaluate({"2022Re-recoE+PromptFG", "sf", "Medium", eta[i], pt[i]});
        }
        return electron_sf_weight;
    };

    auto eval_correction_Reco = [cset_electron] (const RVec<float> eta, const RVec<float> pt) {
        float electron_sf_weight = 1;
        if (eta.size() == 0) {
            return electron_sf_weight;
        }
        for (size_t i = 0; i < eta.size(); i++) {
            if (pt[i] < 20) {
                electron_sf_weight *= cset_electron->evaluate({"2022Re-recoE+PromptFG", "sf", "RecoBelow20", eta[i], pt[i]});
            }
            else if (pt[i] < 75) {
                electron_sf_weight *= cset_electron->evaluate({"2022Re-recoE+PromptFG", "sf", "Reco20to75", eta[i], pt[i]});
            }
            else {
                electron_sf_weight *= cset_electron->evaluate({"2022Re-recoE+PromptFG", "sf", "RecoAbove75", eta[i], pt[i]});
            }
        }
        return electron_sf_weight;
    };

    auto electron_sf_weight = [eval_correction_ID, eval_correction_Reco] (const RVec<float> eta, const RVec<float> pt) {
        return eval_correction_ID(eta, pt) * eval_correction_Reco(eta, pt);
    };

    return df.Define("electron_scale_factors", electron_sf_weight, {"GElectron_eta", "GElectron_pt"});
}

RNode muonScaleFactors(correction::Correction::Ref cset_muon_ID, correction::Correction::Ref cset_muon_ISO, correction::Correction::Ref cset_muon_HLT, RNode df){
    auto eval_correction_ID = [cset_muon_ID] (const RVec<float> eta, const RVec<float> pt) {
        float muon_sf_weight = 1;
        if (eta.size() == 0) {
            return muon_sf_weight;
        }
        for (size_t i = 0; i < eta.size(); i++) {
            muon_sf_weight *= cset_muon_ID->evaluate({abs(eta[i]), pt[i], "nominal"});
        }
        return muon_sf_weight;
    };

    auto eval_correction_ISO = [cset_muon_ISO] (const RVec<float> eta, const RVec<float> pt) {
        float muon_sf_weight = 1;
        if (eta.size() == 0) {
            return muon_sf_weight;
        }
        for (size_t i = 0; i < eta.size(); i++) {
            muon_sf_weight *= cset_muon_ISO->evaluate({abs(eta[i]), pt[i], "nominal"});
        }
        return muon_sf_weight;
    };

    auto eval_correction_HLT = [cset_muon_HLT] (const RVec<float> eta, const RVec<float> pt) {
        float muon_sf_weight = 1;
        // if (eta.size() == 0) {
        //     return muon_sf_weight;
        // }
        // for (size_t i = 0; i < eta.size(); i++) {
        //     muon_sf_weight *= cset_muon_HLT->evaluate({abs(eta[i]), pt[i], "nominal"});
        // }
        return muon_sf_weight;
    };

    auto muon_sf_weight = [eval_correction_ID, eval_correction_ISO, eval_correction_HLT] (const RVec<float> eta, const RVec<float> pt) {
        return eval_correction_ID(eta, pt) * eval_correction_ISO(eta, pt) * eval_correction_HLT(eta, pt);
    };

    return df.Define("muon_scale_factors", muon_sf_weight, {"GMuon_eta", "GMuon_pt"});
}

RNode jetveto(correction::Correction::Ref cset_jet_veto, RNode df){
    auto eval_correction = [cset_jet_veto] (const RVec<float> eta, const RVec<float> phi) {
        float jet_sf_weight = 1;
        if (eta.size() == 0) {
            return jet_sf_weight;
        }
        for (size_t i = 0; i < eta.size(); i++) {
            jet_sf_weight *= cset_jet_veto->evaluate({"jetvetomap", eta[i], phi[i]});
        }
        return jet_sf_weight;
    };

    return df.Define("jet_veto", eval_correction, {"loosejet_eta", "loosejet_phi"})
            .Filter("jet_veto == 0", "Jet Veto Maps");
}


RNode finalMCWeight(RNode df){
    return df.Define("weight", "pileup_weight * electron_scale_factors * muon_scale_factors * xsec_weight * genWeight");
}
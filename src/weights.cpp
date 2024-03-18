#include "weights.hpp"

namespace Weights {
    RNode xsecWeights(RNode df) {
        auto calcWeight = [](float genWeight, double xsec, double lumi, double sumw){return genWeight * xsec * lumi / sumw;};
        return df.Define("weights", calcWeight, {"genWeight", "xsec", "lumi", "sumw"});
    }
    
    RNode pileupCorrection(RNode df){
        auto cset = CorrectionSet::from_file("configs/pileup/puWeights.json");
        auto pileupweights = cset->at("Collisions2022_359022_362760_eraEFG_GoldenJson");

        auto eval_correction = [pileupweights] (float ntrueint, double weights) {
            return weights * pileupweights->evaluate({ntrueint, "nominal"});
        };

        return df.Redefine("weights", eval_correction, {"Pileup_nTrueInt", "weights"});
    }

    RNode leptonScaleFactors(RNode df){
        auto electron_cset = CorrectionSet::from_file("configs/scalefactors/electron/electron.json");
        auto electronscalefactors = electron_cset->at("Electron-ID-SF");
        auto electron_eval_correction = [electronscalefactors] (const RVec<float>& eta, const RVec<float>& pt) {
            return electronscalefactors->evaluate({"2022Re-recoE+PromptFG", "sf", "wp80iso", eta[0], pt[0]});
        };

        auto muon_cset_ID_ISO = CorrectionSet::from_file("configs/scalefactors/muon/ScaleFactors_Muon_Z_ID_ISO_2022_EE_schemaV2.json");
        auto muonscalefactors_ID = muon_cset_ID_ISO->at("NUM_MediumID_DEN_TrackerMuons");
        auto muon_eval_correction_ID = [muonscalefactors_ID] (const RVec<float>& eta, const RVec<float>& pt) {
            return muonscalefactors_ID->evaluate({abs(eta[0]), pt[0], "nominal"});
        };
        auto muonscalefactors_ISO = muon_cset_ID_ISO->at("NUM_MediumMiniIso_DEN_MediumID");
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

    RNode goodRun(lumiMask golden, RNode df){
        auto goldenjson = [golden](unsigned int &run, unsigned int &luminosityBlock){return golden.accept(run, luminosityBlock);};
        return df.Define("weights", goldenjson, {"run", "luminosityBlock"}); 
    }

    // RNode jetEnergyCorrections(RNode df, std::string era, int era_hash){
    //     auto jet_cset = CorrectionSet::from_file("configs/scalefactors/jet/jet_jerc.json");
    //     std::hash<std::string> str_hash;
    //     correction::Correction::Ref& jetscalefactors;
    //     switch (str_hash(era)){
    //         case era_hash:
    //             jetscalefactors = jet_cset->at("Summer22EE_22Sep2023_RunE_V2_DATA_L1L2L3Res_AK4PFPuppi");
    //             break;
    //         case era_hash:
    //             jetscalefactors = jet_cset->at("Summer22EE_22Sep2023_RunF_V2_DATA_L1L2L3Res_AK4PFPuppi");
    //             break;
    //         case era_hash:
    //             jetscalefactors = jet_cset->at("Summer22EE_22Sep2023_RunG_V2_DATA_L1L2L3Res_AK4PFPuppi");
    //             break;
    //         default:
    //             jetscalefactors = jet_cset->at("Summer22EE_22Sep2023_V2_MC_L1FastJet_AK4PFPuppi");
    //     };
    //     auto eval_correction = [jetscalefactors] (const RVec<float>& area, const RVec<float>& eta, const RVec<float>& pt, const RVec<float>& raw_factor, const float rho) {
    //         RVec<float> jet_energy_corrections;
    //         for (int i = 0; i < area.size(); i++) {
    //             jet_energy_corrections.push_back(jetscalefactors->evaluate({area[i], eta[i], pt[i] * (1-raw_factor[i]), rho}));
    //         }
    //     };
    //     return df.Define("Jet_CPt", [jetEnergyCorrections](const RVec<float>& pt, const RVec<float>& raw_factor, const RVec<float>& jet_energy_corrections) {
    //         RVec<float> corrected_pt;
    //         for (int i = 0; i < pt.size(); i++) {
    //             corrected_pt.push_back(pt[i] * (1-raw_factor[i]) * jet_energy_corrections[i]);
    //         }
    //         return corrected_pt;
    //     }, {"Jet_pt", "Jet_rawFactor", "jet_energy_corrections"});
    // }
}
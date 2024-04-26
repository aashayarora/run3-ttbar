#include "selections.h"

RNode flagSelections(RNode df) {
    auto df_flags = df.Filter("Flag_goodVertices && "
                "(!isData || Flag_globalSuperTightHalo2016Filter) && "
                "Flag_HBHENoiseFilter && "
                "Flag_HBHENoiseIsoFilter && "
                "Flag_EcalDeadCellTriggerPrimitiveFilter && "
                "Flag_BadPFMuonFilter &&"
                "Flag_BadPFMuonDzFilter && "
                "Flag_eeBadScFilter && "
                "(!(is2017 || is2018) || Flag_ecalBadCalibFilter)", 
                "PASSED FLAGS");

    return df_flags;
}

RNode triggerSelections(RNode df) {
    auto df_hlt = df.Define("passes_elHLT", "(HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ || HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL || HLT_Ele30_WPTight_Gsf || HLT_Ele32_WPTight_Gsf || HLT_Ele32_WPTight_Gsf_L1DoubleEG || HLT_Ele35_WPTight_Gsf || HLT_Ele115_CaloIdVT_GsfTrkIdT || HLT_DoubleEle25_CaloIdL_MW)")
                    .Define("passes_muHLT", "(HLT_IsoMu24 || HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8 || HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8 || HLT_IsoMu27 || HLT_Mu50)");

    return df_hlt;
}

RNode electronSelections(RNode df) {
    auto df_electrons = df_hlt.Define("goodElectrons", 
                        "Electron_pt > 15 &&"
                        "abs(Electron_eta) < 2.5 && "
                        "abs(Electron_dxy) < 0.05 && abs(Electron_dz) < 0.1 && "
                        "Electron_mvaIso_WP80 == 1")
                    .Define("nGoodElectrons", "nElectron == 0 ? 0 : Sum(goodElectrons)")
                    .Define("GElectron_pt", "Electron_pt[goodElectrons]")
                    .Define("GElectron_eta", "Electron_eta[goodElectrons]")
                    .Define("GElectron_phi", "Electron_phi[goodElectrons]")
                    .Define("GElectron_mass", "Electron_mass[goodElectrons]")
                    .Define("GElectron_charge", "Electron_charge[goodElectrons]")
                    .Define("isElectron", "nGoodElectrons > 0");
        
    return df_electrons;
}

RNode muonSelections(RNode df) {
    auto df_muons = df_electrons.Define("goodMuons", 
                        "Muon_pt > 10 && "
                        "abs(Muon_eta) < 2.4 && "
                        "abs(Muon_dxy) < 0.05 && abs(Muon_dz) < 0.1 && "
                        "Muon_mediumId == 1 && "
                        "Muon_pfIsoId >= 4")
                    .Define("nGoodMuons", "nMuon == 0 ? 0 : Sum(goodMuons)")
                    .Define("GMuon_pt", "Muon_pt[goodMuons]")
                    .Define("GMuon_eta", "Muon_eta[goodMuons]")
                    .Define("GMuon_phi", "Muon_phi[goodMuons]")
                    .Define("GMuon_mass", "Muon_mass[goodMuons]")
                    .Define("GMuon_charge", "Muon_charge[goodMuons]")
                    .Define("isMuon", "nGoodMuons > 0");
    return df_muons;
}

RNode leptonSelections(RNode df) {
    auto df_el = electronSelections(df);
    auto df_mu = muonSelections(df_el);
    auto df_leps = df_mu.Filter("(passes_elHLT && nGoodElectrons == 2 && nGoodMuons == 0 && Product(GElectron_charge) < 0) || "
                        "(passes_muHLT && nGoodElectrons == 0 && nGoodMuons == 2 && Product(GMuon_charge) < 0)", "exactly 2 OS Leptons")
                    .Define("GLepton_pt", "isElectron ? GElectron_pt : GMuon_pt")
                    .Define("GLepton_eta", "isElectron ? GElectron_eta : GMuon_eta")
                    .Define("GLepton_phi", "isElectron ? GElectron_phi : GMuon_phi")
                    .Define("GLepton_mass", "isElectron ? GElectron_mass : GMuon_mass")
                    .Define("m_ll", "InvariantMass(GLepton_pt, GLepton_eta, GLepton_phi, GLepton_mass)");

    return df_leps;
}


RNode jetSelections(RNode df) {
    auto df_jets = df_leps.Define("goodJets", "Jet_pt > 30 && abs(Jet_eta) < 2.4 && Jet_jetId >= 4")
                    .Define("nGJet", "nJet == 0 ? 0 : Sum(goodJets)")
                    .Filter("nGJet >= 2", "atleast two good jets")
                    .Define("GJet_pt", "Jet_pt[goodJets]")
                    .Define("GJet_eta", "Jet_eta[goodJets]")
                    .Define("GJet_phi", "Jet_phi[goodJets]");

    return df_jets;
}

RNode bJetSelections(RNode df) {
    auto df_bjet = df.Define("GJet_btagPNetB", "Jet_btagPNetB[goodJets]")
                    .Define("nBJet_medium", "GJet_btagPNetB.size() == 0 ? 0 : Sum(GJet_btagPNetB > 0.2605)")
                    .Define("nBJet_tight", "GJet_btagPNetB.size() == 0 ? 0 : Sum(GJet_btagPNetB > 0.6915)")
                    .Define("nGBJet", "nBJet_medium + nBJet_tight")
                    .Filter("nGBJet >= 2", "has atleast 2 bJets");

    return df_bjet;
}
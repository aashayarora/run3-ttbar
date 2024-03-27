#include "selections.h"

RNode electronSelections(RNode df) {
    return df.Define("goodElectrons", 
                "Electron_pt > 15 &&"
                "abs(Electron_eta) < 2.5 && "
                "abs(Electron_dxy) < 0.05 && abs(Electron_dz) < 0.1 && "
                "Electron_mvaIso_WP80 == 1")
            .Define("nGoodElectrons", "Sum(goodElectrons)")
            .Define("GElectron_pt", "Electron_pt[goodElectrons]")
            .Define("GElectron_eta", "Electron_eta[goodElectrons]")
            .Define("GElectron_phi", "Electron_phi[goodElectrons]")
            .Define("GElectron_mass", "Electron_mass[goodElectrons]")
            .Define("GElectron_charge", "Electron_charge[goodElectrons]")
            .Define("isElectron", "nGoodElectrons == 2")
            .Define("m_ee", get_inv_mass, {"GElectron_pt", "GElectron_eta", "GElectron_phi", "GElectron_mass"});
}

RNode muonSelections(RNode df) {
    return df.Define("goodMuons", 
                "Muon_pt > 10 && "
                "abs(Muon_eta) < 2.4 && "
                "abs(Muon_dxy) < 0.05 && abs(Muon_dz) < 0.1 && "
                "Muon_mediumId == 1 && "
                "Muon_pfIsoId >= 4")
            .Define("nGoodMuons", "Sum(goodMuons)")
            .Define("GMuon_pt", "Muon_pt[goodMuons]")
            .Define("GMuon_eta", "Muon_eta[goodMuons]")
            .Define("GMuon_phi", "Muon_phi[goodMuons]")
            .Define("GMuon_mass", "Muon_mass[goodMuons]")
            .Define("GMuon_charge", "Muon_charge[goodMuons]")
            .Define("isMuon", "nGoodMuons == 2")
            .Define("m_mumu", get_inv_mass, {"GMuon_pt", "GMuon_eta", "GMuon_phi", "GMuon_mass"});
}

RNode eventElectronSelections(RNode df) {
    return df.Filter("(isElectron && Sum(GElectron_charge) == 0)", "opposite sign leptons")
             .Filter("(isElectron && (HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ || HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL || HLT_Ele30_WPTight_Gsf || HLT_Ele32_WPTight_Gsf || HLT_Ele32_WPTight_Gsf_L1DoubleEG || HLT_Ele35_WPTight_Gsf || HLT_Ele115_CaloIdVT_GsfTrkIdT || HLT_DoubleEle25_CaloIdL_MW))", "Passes HLT")
             .Filter("(isElectron && All(GElectron_pt > 25))", "lepton pt > 25 GeV");
}

RNode eventMuonSelections(RNode df){
    return df.Filter("(isMuon && Sum(GMuon_charge) == 0)")
             .Filter("(isMuon && (HLT_IsoMu24 || HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8 || HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8 || HLT_IsoMu27 || HLT_Mu50))", "Passes HLT")
             .Filter("(isMuon && All(GMuon_pt > 25))", "lepton pt > 25 GeV");
}
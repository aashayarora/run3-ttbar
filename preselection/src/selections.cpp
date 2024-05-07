#include "selections.h"

RNode flagSelections(RNode df) {
    auto df_flags = df.Filter("Flag_goodVertices && "
            "Flag_globalSuperTightHalo2016Filter && "
            "Flag_EcalDeadCellTriggerPrimitiveFilter && "
            "Flag_BadPFMuonFilter && "
            "Flag_ecalBadCalibFilter && "
            "Flag_BadPFMuonDzFilter && "
            "Flag_hfNoisyHitsFilter && "
            "Flag_eeBadScFilter",
            "PASSED FLAGS");
    return df_flags;
}

RNode triggerSelections(RNode df) {
    auto df_hlt = df.Define("passes_elHLT", 
                "(HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ == 1 || "
                "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL == 1 || "
                "HLT_Ele30_WPTight_Gsf == 1 || "
                "HLT_Ele32_WPTight_Gsf == 1 || "
                "HLT_Ele32_WPTight_Gsf_L1DoubleEG == 1 || "
                "HLT_Ele35_WPTight_Gsf == 1 || "
                "HLT_Ele115_CaloIdVT_GsfTrkIdT == 1)")
            .Define("passes_muHLT", 
                "(HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8 == 1 || "
                "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8 == 1 || "
                "HLT_IsoMu24 == 1 || "
                "HLT_IsoMu27 == 1 || "
                "HLT_Mu50 == 1 )");
    return df_hlt;
}

RNode electronSelections(RNode df) {
    auto df_electrons = df.Define("looseElectrons", 
                "Electron_pt > 10 &&"
                "abs(Electron_eta) < 2.4 && "
                "Electron_cutBased >= 1 && "
                "Electron_convVeto == 1")
            .Define("nLooseElectrons", "nElectron == 0 ? 0 : Sum(looseElectrons)")
            .Define("goodElectrons", "looseElectrons &&"
                "abs(Electron_dxy) < 0.05 && abs(Electron_dz) < 0.1 && "
                "Electron_cutBased >= 3")
            .Define("nGoodElectrons", "nElectron == 0 ? 0 : Sum(goodElectrons)")
            .Define("GElectron_pt", "Electron_pt[goodElectrons]")
            .Define("GElectron_eta", "Electron_eta[goodElectrons]")
            .Define("GElectron_phi", "Electron_phi[goodElectrons]")
            .Define("GElectron_mass", "Electron_mass[goodElectrons]")
            .Define("GElectron_charge", "Electron_charge[goodElectrons]")
            .Define("isElectron", "nGoodElectrons == 2");
    return df_electrons;
}

RNode muonSelections(RNode df) {
    auto df_muons = df.Define("looseMuons", 
                "Muon_pt > 10 && "
                "abs(Muon_eta) < 2.4 && "
                "Muon_looseId == 1")
            .Define("nLooseMuons", "nMuon == 0 ? 0 : Sum(looseMuons)")
            .Define("goodMuons", "looseMuons && "
                "abs(Muon_dxy) < 0.05 && abs(Muon_dz) < 0.1 && "
                "Muon_mediumId == 1 && "
                "Muon_pfIsoId >= 4")
            .Define("nGoodMuons", "nMuon == 0 ? 0 : Sum(goodMuons)")
            .Define("GMuon_pt", "Muon_pt[goodMuons]")
            .Define("GMuon_eta", "Muon_eta[goodMuons]")
            .Define("GMuon_phi", "Muon_phi[goodMuons]")
            .Define("GMuon_mass", "Muon_mass[goodMuons]")
            .Define("GMuon_charge", "Muon_charge[goodMuons]")
            .Define("isMuon", "nGoodMuons == 2");
    return df_muons;
}

RNode leptonSelections(RNode df) {
    auto df_el = electronSelections(df);
    auto df_mu = muonSelections(df_el);
    auto df_leps = df_mu.Filter("(nLooseElectrons == 2 && nGoodElectrons == 2 && nLooseMuons == 0 && nGoodMuons == 0 && Sum(GElectron_charge) == 0 && passes_elHLT) || "
                "(nLooseElectrons == 0 && nGoodElectrons == 0 && nLooseMuons == 2 && nGoodMuons == 2 && Sum(GMuon_charge) == 0 && passes_muHLT)", 
                "exactly 2 OS Leptons")
            .Define("GLepton_pt", "isElectron ? GElectron_pt : GMuon_pt")
            .Define("GLepton_eta", "isElectron ? GElectron_eta : GMuon_eta")
            .Define("GLepton_phi", "isElectron ? GElectron_phi : GMuon_phi")
            .Define("GLepton_mass", "isElectron ? GElectron_mass : GMuon_mass")
            .Define("m_ee", "isElectron ? InvariantMass(GElectron_pt, GElectron_eta, GElectron_phi, GElectron_mass) : 0")
            .Define("m_mumu", "isMuon ? InvariantMass(GMuon_pt, GMuon_eta, GMuon_phi, GMuon_mass) : 0")
            .Define("leading_lepton_pt_idx", "ArgMax(GLepton_pt)")
            .Define("subleading_lepton_pt_idx", "ArgMin(GLepton_pt)")
            .Define("leading_lepton_pt", "GLepton_pt[leading_lepton_pt_idx]")
            .Define("subleading_lepton_pt", "GLepton_pt[subleading_lepton_pt_idx]")
            .Define("leading_lepton_eta", "GLepton_eta[leading_lepton_pt_idx]")
            .Define("subleading_lepton_eta", "GLepton_eta[subleading_lepton_pt_idx]")
            .Define("leading_lepton_phi", "GLepton_phi[leading_lepton_pt_idx]")
            .Define("subleading_lepton_phi", "GLepton_phi[subleading_lepton_pt_idx]")
            .Define("leading_lepton_mass", "GLepton_mass[leading_lepton_pt_idx]")
            .Define("subleading_lepton_mass", "GLepton_mass[subleading_lepton_pt_idx]")
            .Define("m_ll", "InvariantMass(GLepton_pt, GLepton_eta, GLepton_phi, GLepton_mass)")
            .Filter("leading_lepton_pt > 25 && subleading_lepton_pt > 15", "lepton pT cuts")
            .Filter("m_ll > 75 && m_ll < 105", "Z mass window");
    return df_leps;
}

RNode jetSelections(RNode df) {
    auto df_jets = df.Define("goodJets", "Jet_pt > 30 && abs(Jet_eta) < 2.4")
            .Define("nGJet", "nJet == 0 ? 0 : Sum(goodJets)")
            // .Define("GJet_pt", "Jet_pt[goodJets]")
            // .Define("GJet_eta", "Jet_eta[goodJets]")
            // .Define("GJet_phi", "Jet_phi[goodJets]")
            // .Define("GJet_mass", "Jet_mass[goodJets]")
            // .Define("sorted_GJets", "Argsort(GJet_pt)")
            // .Define("leading_jet_pt", "GJet_pt[sorted_GJets[0]]")
            // .Define("subleading_jet_pt", "GJet_pt[sorted_GJets[1]]")
            // .Define("leading_jet_eta", "GJet_eta[sorted_GJets[0]]")
            // .Define("subleading_jet_eta", "GJet_eta[sorted_GJets[1]]")
            // .Define("leading_jet_phi", "GJet_phi[sorted_GJets[0]]")
            // .Define("subleading_jet_phi", "GJet_phi[sorted_GJets[1]]")
            // .Define("leading_jet_mass", "GJet_mass[sorted_GJets[0]]")
            // .Define("subleading_jet_mass", "GJet_mass[sorted_GJets[1]]")
            // .Define("pt_first_two", "Take(Take(GJet_pt, sorted_GJets), 2)")
            // .Define("eta_first_two", "Take(Take(GJet_eta, sorted_GJets), 2)")
            // .Define("phi_first_two", "Take(Take(GJet_phi, sorted_GJets), 2)")
            // .Define("mass_first_two", "Take(Take(GJet_mass, sorted_GJets), 2)")
            // .Define("delta_phi_jj", "abs(leading_jet_phi - subleading_jet_phi)")
            // .Define("delta_eta_jj", "abs(leading_jet_eta - subleading_jet_eta)")
            // .Define("pt_jj", "leading_jet_pt + subleading_jet_pt")
            // .Define("m_jj", "InvariantMass(pt_first_two, eta_first_two, phi_first_two, mass_first_two)")
            // .Define("looseJets", "Jet_pt > 15 && Jet_jetId >= 3 && Jet_chEmEF < 0.9")
            // .Define("loosejet_eta", "Jet_eta[looseJets]")
            // .Define("loosejet_phi", "Jet_phi[looseJets]")
            .Define("n_pvs", "PV_npvs");
    return df_jets;
}

RNode bJetSelections(RNode df) {
    auto df_bjet = df.Define("GJet_btagPNetB", "Jet_btagPNetB[goodJets]")
            .Define("nBJet_medium", "GJet_btagPNetB.size() == 0 ? 0 : Sum(GJet_btagPNetB > 0.2605)")
            .Define("nBJet_tight", "GJet_btagPNetB.size() == 0 ? 0 : Sum(GJet_btagPNetB > 0.6915)")
            .Define("nGBJet", "nBJet_medium + nBJet_tight");
            // .Filter("nGBJet >= 2", "has atleast 2 bJets");
    return df_bjet;
}
#include "common.hpp"

namespace MuonEG {
    RNode leptonTriggers(RNode df) {
        return df.Filter("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ || HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ", "Passes e mu triggers");
    }
    RNode leptonSelections(RNode df) {
        return df.Filter("nElectron == 1 && nMuon == 1", "has one electron and one muon")
            .Filter("Electron_charge[0] != Muon_charge[0]", "Leptons are os")
            .Filter("Electron_pt[0] > 25", "passes electron pt cut")
            .Filter("abs(Electron_eta[0]) < 2.5", "passes electron eta cut")
            .Filter("abs(Electron_dxy[0]) < 0.1 && abs(Electron_dz[0]) < 0.2", "electron passes dxyz")
            .Filter("Electron_cutBased[0] >= 3", "electron passes medium id")
            .Filter("Electron_mvaIso_WP80[0]", "passes electron mvaIso WP80")
            .Filter("Muon_pt[0] > 26", "passes muon pt cut")
            .Filter("abs(Muon_eta[0]) < 2.4", "passes eta cut")
            .Filter("abs(Muon_dxy[0]) < 0.1 && abs(Muon_dz[0]) < 0.2", "muon passes dxyz")
            .Filter("Muon_mediumId[0]", "muon passes medium id")
            .Filter("Muon_pfIsoId[0] >= 4", "muon tight muon iso")
            .Define("m_ll", "ROOT::VecOps::InvariantMasses(Electron_pt, Electron_eta, Electron_phi, Electron_mass, Muon_pt, Muon_eta, Muon_phi, Muon_mass)");
    }
    RNode jetSelections(RNode df) {
        return df.Define("goodJets", "Jet_pt > 30 && abs(Jet_eta) < 2.5 && Jet_jetId >= 4")
            .Define("nGJet", "Sum(goodJets)")
            .Define("GJet_pt", "Jet_pt[goodJets]")
            .Define("GJet_eta", "Jet_eta[goodJets]")
            .Define("GJet_jetId", "Jet_jetId[goodJets]")
            .Define("GJet_btagPNetB", "Jet_btagPNetB[goodJets]")
            .Filter("nGJet >= 2", "atleast two good jets")
            .Define("nBJet_tight", "Sum(GJet_btagPNetB > 0.6915)")
            .Define("nBJet_medium", "Sum(GJet_btagPNetB > 0.2605)")
            .Define("nBJet", "nBJet_tight + nBJet_medium")
            .Filter("nBJet >= 2", "has atleast 2 bJets");
    }
    RNode selections(RNode df) {
        auto df1 = leptonTriggers(df);
        auto df2 = leptonSelections(df1);
        auto df_last = jetSelections(df2);
        return df_last;
    }
}

namespace Muon {
    RNode leptonTriggers(RNode df) {
        return df.Filter("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8", "Passes Muon trigger");
    }
    RNode leptonSelections(RNode df) {
        return df.Filter("nMuon == 2", "has two muons")
            .Filter("Muon_charge[0] != Muon_charge[1]", "muons are os")
            .Filter("All(Muon_mediumId || Muon_tightId)", "passes loose_id")
            .Filter("All(Muon_pt > 25)", "passes pt cut")
            .Filter("All(abs(Muon_eta) < 2.4)", "passes eta cut")
            .Filter("All(abs(Muon_dxy) < 0.1) && All(abs(Muon_dz) < 0.2)", "passes dxyz")
            .Filter("All(Muon_pfIsoId >= 4)", "passes tight iso")
            .Define("leading_muon_idx", "ROOT::VecOps::ArgMax(Muon_pt)")
            .Define("subleading_muon_idx", "ROOT::VecOps::ArgMin(Muon_pt)")
            .Define("leading_muon_pt", "Muon_pt[leading_muon_idx]")
            .Define("leading_muon_eta", "Muon_eta[leading_muon_idx]")
            .Define("leading_muon_phi", "Muon_phi[leading_muon_idx]")
            .Define("leading_muon_m", "Muon_mass[leading_muon_idx]")
            .Define("subleading_muon_pt", "Muon_pt[subleading_muon_idx]")
            .Define("subleading_muon_eta", "Muon_eta[subleading_muon_idx]")
            .Define("subleading_muon_phi", "Muon_phi[subleading_muon_idx]")
            .Define("subleading_muon_m", "Muon_mass[subleading_muon_idx]")
            .Define("m_ll", "ROOT::VecOps::InvariantMass(Muon_pt, Muon_eta, Muon_phi, Muon_mass)");
    }
    RNode jetSelections(RNode df) {
        return df.Define("goodJets", "Jet_pt > 30 && abs(Jet_eta) < 2.5 && Jet_jetId >= 4")
            .Define("nGJet", "Sum(goodJets)")
            .Define("GJet_pt", "Jet_pt[goodJets]")
            .Define("GJet_eta", "Jet_eta[goodJets]")
            .Define("GJet_jetId", "Jet_jetId[goodJets]")
            .Filter("nGJet >= 1", "atleast one good jets");
    }
    RNode selections(RNode df) {
        auto df1 = leptonTriggers(df);
        auto df2 = leptonSelections(df1);
        auto df_last = jetSelections(df2);
        return df_last;
    }
}

namespace EGamma {
    RNode leptonTriggers(RNode df) {
        return df.Filter("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ", "Passes Electron trigger");
    }
    RNode leptonSelections(RNode df) {
        return df.Filter("nElectron == 2", "has two Electrons")
            .Filter("Electron_charge[0] != Electron_charge[1]", "Electrons are os")
            .Filter("All(Electron_cutBased >= 3)", "passes medium_id")
            .Filter("All(Electron_pt > 25)", "passes pt cut")
            .Filter("All(abs(Electron_eta) < 2.5)", "passes eta cut")
            .Filter("All(abs(Electron_dxy) < 0.1) && All(abs(Electron_dz) < 0.2)", "passes dxyz")
            .Filter("All(Electron_mvaIso_WP80)", "passes mvaIso WP80")
            .Define("leading_electron_idx", "ROOT::VecOps::ArgMax(Electron_pt)")
            .Define("subleading_electron_idx", "ROOT::VecOps::ArgMin(Electron_pt)")
            .Define("leading_electron_pt", "Electron_pt[leading_electron_idx]")
            .Define("leading_electron_eta", "Electron_eta[leading_electron_idx]")
            .Define("leading_electron_phi", "Electron_phi[leading_electron_idx]")
            .Define("leading_electron_m", "Electron_mass[leading_electron_idx]")
            .Define("subleading_electron_pt", "Electron_pt[subleading_electron_idx]")
            .Define("subleading_electron_eta", "Electron_eta[subleading_electron_idx]")
            .Define("subleading_electron_phi", "Electron_phi[subleading_electron_idx]")
            .Define("subleading_electron_m", "Electron_mass[subleading_electron_idx]")
            .Define("m_ll", "ROOT::VecOps::InvariantMass(Electron_pt, Electron_eta, Electron_phi, Electron_mass)");
    }
    RNode jetSelections(RNode df) {
        return df.Define("goodJets", "Jet_pt > 30 && abs(Jet_eta) < 2.5 && Jet_jetId >= 4")
            .Define("nGJet", "Sum(goodJets)")
            .Define("GJet_pt", "Jet_pt[goodJets]")
            .Define("GJet_eta", "Jet_eta[goodJets]")
            .Define("GJet_jetId", "Jet_jetId[goodJets]")
            .Filter("nGJet >= 1", "atleast one good jets");
    }
    RNode selections(RNode df) {
        auto df1 = leptonTriggers(df);
        auto df2 = leptonSelections(df1);
        auto df_last = jetSelections(df2);
        return df_last;
    }
}
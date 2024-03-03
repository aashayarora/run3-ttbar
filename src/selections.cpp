#include "common.hpp"

namespace Lepton {
    RNode triggers(RNode df) {
        return df.Filter("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL || HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL", "Passes e mu triggers");
    }
    RNode preSelections(RNode df) {
        return df.Filter("nElectron == 1 && nMuon == 1", "has one electron and one muon")
            .Filter("Electron_charge[0] != Muon_charge[0]", "Leptons are os")
            .Filter("Electron_pt[0] > 15", "passes electron pt cut")
            .Filter("abs(Electron_eta[0]) < 2.5", "passes electron eta cut")
            .Filter("abs(Electron_dxy[0]) < 0.1 && abs(Electron_dz[0]) < 0.2", "electron passes dxyz")
            .Filter("Electron_cutBased[0] >= 2", "electron passes loose_id")
            .Filter("Electron_mvaIso_WP80[0]", "passes electron mvaIso WP80")
            .Filter("Muon_pt[0] > 30", "passes muon pt cut")
            .Filter("abs(Muon_eta[0]) < 2.4", "passes eta cut")
            .Filter("abs(Muon_dxy[0]) < 0.1 && abs(Muon_dz[0]) < 0.2", "muon passes dxyz")
            .Filter("Muon_looseId[0]", "muon passes loose_id")
            .Filter("Muon_pfIsoId[0] >= 4", "muon tight muon iso");
    }
    RNode defineVariables(RNode df) {
        return df.Define("m_ll", "ROOT::VecOps::InvariantMasses(Electron_pt, Electron_eta, Electron_phi, Electron_mass, Muon_pt, Muon_eta, Muon_phi, Muon_mass)");
    }
    RNode selections(RNode df) {
        auto df1 = triggers(df);
        auto df2 = preSelections(df1);
        auto df_last = defineVariables(df2);
        return df_last;
    }
}

namespace Jet {
    RNode preSelections(RNode df) {
        return df.Filter("nJet >= 2", "has atleast two jets")
            .Filter("Any(Jet_pt > 30 && abs(Jet_eta) < 2.5)", "jet pt and eta cut")
            .Filter("All(Jet_jetId >= 2)", "jet passes jetId")
            .Define("nBJet", "Sum(Jet_btagPNetB > 0.6)")
            .Filter("nBJet >= 2", "has atleast 2 bJets");
    }
    RNode selections(RNode df) {
        auto df_last = preSelections(df);
        return df_last;
    }
}
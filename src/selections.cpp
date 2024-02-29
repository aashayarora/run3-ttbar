#include "main.hpp"

namespace Lepton {
    RNode triggers(RNode df) {
        return df.Filter("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL || HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL", "Passes e mu triggers");
    }
    RNode preSelections(RNode df) {
        return df.Filter("nElectron == 1 && nMuon == 1", "has one electron and one muon")
            .Filter("Electron_charge[0] != Muon_charge[0]", "Leptons are os")
            .Filter("Electron_cutBased[0] >= 2", "passes loose_id")
            .Filter("Electron_pt[0] > 15", "passes pt cut")
            .Filter("abs(Electron_eta[0]) < 2.5", "passes eta cut")
            .Filter("abs(Electron_dxy[0]) < 0.1 && abs(Electron_dz[0]) < 0.2", "electron passes dxyz")
            .Filter("Electron_mvaIso_WP80[0]", "passes electron mvaIso WP80")
            .Filter("Muon_looseId[0]", "passes loose_id")
            .Filter("Muon_pt[0] > 25", "passes pt cut")
            .Filter("abs(Muon_eta[0]) < 2.4", "passes eta cut")
            .Filter("abs(Muon_dxy[0]) < 0.1 && abs(Muon_dz[0]) < 0.2", "muon passes dxyz")
            .Filter("Muon_pfIsoId[0] >= 4", "passes tight muon iso");
    }
    RNode defineVariables(RNode df) {
        auto get_p4 = [](float pt, float eta, float phi, float mass) {return ROOT::Math::PtEtaPhiMVector(pt, eta, phi, mass);};
        auto invariant_mass = [](ROOT::Math::PtEtaPhiMVector& p4) {return p4.M();};
        return df.Define("electron_pt", "Electron_pt[0]")
            .Define("electron_eta", "Electron_eta[0]")
            .Define("electron_phi", "Electron_phi[0]")
            .Define("electron_mass", "Electron_mass[0]")
            .Define("muon_pt", "Muon_pt[0]")
            .Define("muon_eta", "Muon_eta[0]")
            .Define("muon_phi", "Muon_phi[0]")
            .Define("muon_mass", "Muon_mass[0]")
            .Define("electron_p4", get_p4, {"electron_pt", "electron_eta", "electron_phi", "electron_mass"})
            .Define("muon_p4", get_p4, {"muon_pt", "muon_eta", "muon_phi", "muon_mass"})
            .Define("dilepton_p4", "electron_p4 + muon_p4")
            .Define("m_ll", invariant_mass, {"dilepton_p4"});
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
        return df.Filter("nJet >= 1", "has atleast one jet")
            .Filter("Any(Jet_pt > 30 && abs(Jet_eta) < 2.5)", "jet pt and eta cut");
    }
    
    RNode defineVariables(RNode df) {
        return df.Define("num_jets", "nJet");
    }   

    RNode selections(RNode df) {
        auto df1 = preSelections(df);
        auto df_last = defineVariables(df1);
        return df_last;
    }
}
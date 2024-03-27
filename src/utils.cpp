#include "utils.h"

// Selection Utils
float get_inv_mass(RVec<float> pt, RVec<float> eta, RVec<float> phi, RVec<float> mass) {
    ROOT::Math::PtEtaPhiMVector p1(pt[0], eta[0], phi[0], mass[0]);
    ROOT::Math::PtEtaPhiMVector p2(pt[1], eta[1], phi[1], mass[1]);
    return (p1 + p2).M();
}

// General Utils
void saveSnapshot(RNode df, const std::string& finalFile) {
    auto ColNames = df.GetColumnNames();
    std::vector<std::string> final_variables;
    // for (auto &&ColName : ColNames)
    //     {
    //         TString colName = ColName;
    //         if(colName.Contains("P4")) continue;
    //         if(colName.Contains("LHE")) continue;
    //         if(colName.BeginsWith("FatJet")) continue;
    //         if(colName.BeginsWith("PPS")) continue;
    //         if(colName.BeginsWith("Gen")) continue;
    //         if(colName.BeginsWith("Sub")) continue;
    //         if(colName.BeginsWith("HLT")) continue;
    //         if(colName.BeginsWith("Flag")) continue;
    //         if(colName.BeginsWith("PS")) continue;
    //         if(colName.BeginsWith("nCorr")) continue;
    //         if(colName.BeginsWith("nGen")) continue;
    //         if(colName.BeginsWith("nOther")) continue;
    //         if(colName.BeginsWith("nSV")) continue;

    //         std::string name = colName.Data();
    //         final_variables.push_back(name);
    //     }
    final_variables.push_back("run");
    final_variables.push_back("luminosityBlock");
    final_variables.push_back("event");
    df.Snapshot("Events", std::string("output/") + finalFile, final_variables);
}
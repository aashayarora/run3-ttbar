// C++ Includes
#include <iostream>

// RDF Includes
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "ROOT/RVec.hxx"

#include "weights.h"
#include "selections.h"
#include "utils.h"

using std::cout, std::endl;
using ROOT::RDF::RSampleInfo;

int main(int argc, char** argv) {
    // Config
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <input_spec>" << endl;
        return 0;
    }
    std::string input_spec = argv[1];
    
    // golden json lumimask
    const auto LumiMask = lumiMask::fromJSON("corrections/goodrun/Cert_Collisions2022_355100_362760_Golden.json");

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df_ = ROOT::RDF::Experimental::FromSpec(input_spec);
    ROOT::RDF::Experimental::AddProgressBar(df_);

    auto df = defineMetadata(df_);
    // selections
    auto df1 = flagSelections(df);
    auto df2 = triggerSelections(df1);
    auto df3 = leptonSelections(df2);
    auto df4 = jetSelections(df3);
    auto df5 = bJetSelections(df4);

    // data only corrections
    auto df_data = df5.Filter("isData");
    auto df_data_final = goodRun(LumiMask, df_data);
    saveSnapshot(df_data_final, "data.root");

    // MC only corrections
    auto df_mc = df5.Filter("!isData");
    auto df_mc_final = pileupWeight(df_mc);
    saveSnapshot(df_mc_final, "mc.root");

    // print cutflow
    auto report = df_.Report();
    report->Print();

    return 0;
}

#include "ROOT/RDataFrame.hxx"

#include "weights.h"
#include "selections.h"
#include "utils.h"

#include "correction.h"
using correction::CorrectionSet;

#include "argparser.hpp"

struct MyArgs : public argparse::Args {
    std::string &spec = kwarg("i,input", "spec.json path");
    std::string &output = kwarg("o,output", "output root file");
    bool &isData = flag("data", "is data");
};

int main(int argc, char** argv) {
    auto args = argparse::parse<MyArgs>(argc, argv);
    std::string input_spec = args.spec;
    
    /*
    ANALYSIS
    */
    ROOT::EnableImplicitMT(16);
    ROOT::RDataFrame df_ = ROOT::RDF::Experimental::FromSpec(input_spec);
    ROOT::RDF::Experimental::AddProgressBar(df_);

    auto df = defineMetadata(df_);
    // selections
    auto df1 = flagSelections(df);
    auto df2 = triggerSelections(df1);
    auto df3 = leptonSelections(df2);
    auto df4 = jetSelections(df3);
    // auto df4 = jetveto(cset_jet_veto, df45); // wait for it to get fixed
    auto df5 = bJetSelections(df4);

    if (args.isData) {
        auto df_data_final = goodRun(LumiMask, df5);
        saveSnapshot(df_data_final, args.output);
    }
    else {
        auto df_pileup = pileupCorrection(cset_pileup, df5);
        auto df_electron_sf = electronScaleFactors(cset_electron, df_pileup);
        auto df_muon_sf = muonScaleFactors(cset_muon_ID, cset_muon_ISO, cset_muon_HLT, df_electron_sf);
        auto df_mc_final = finalMCWeight(df_muon_sf); // remember to add weights to finalMCWeight everytime.
        saveSnapshot(df_mc_final, args.output);
    }
    // print cutflow
    auto report = df_.Report();
    report->Print();

    return 0;
}

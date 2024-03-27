// C++ Includes
#include <iostream>

// RDF Includes
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "ROOT/RVec.hxx"

#include "weights.h"
#include "selections.h"
#include "lumimask.h"
#include "plot.h"
#include "utils.h"

using std::cout, std::endl;
using ROOT::RDF::RSampleInfo;

int main(int argc, char** argv) {
    // Config
    std::string input_spec = "configs/input_m.json";
    
    ROOT::EnableImplicitMT(32); // enable multithreaded rdfs
    ROOT::RDataFrame df_ = ROOT::RDF::Experimental::FromSpec(input_spec);
    ROOT::RDF::Experimental::AddProgressBar(df_);

    auto df = df_.DefinePerSample("sample_category", [](unsigned int slot, const RSampleInfo &id) { return id.GetS("sample_category");})
                 .DefinePerSample("sample_type", [](unsigned int slot, const RSampleInfo &id) { return id.GetS("sample_type");});
    auto df_mc = df.Filter("sample_category == \"mc\"");
    auto df_el = df.Filter("sample_category == \"data\" && sample_type == \"EGamma\"");
    auto df_mu = df.Filter("sample_category == \"data\" && sample_type == \"Muon\"");

    // Electron
    auto df1_mc_el = electronSelections(df_mc);
    auto df2_mc_el = eventElectronSelections(df1_mc_el);

    auto df1_data_el = electronSelections(df_el);
    auto df2_data_el = eventElectronSelections(df1_data_el);

    //Muon
    auto df1_mc_mu = muonSelections(df_mc);
    auto df2_mc_mu = eventMuonSelections(df1_mc_mu);

    auto df1_data_mu = muonSelections(df_mu);
    auto df2_data_mu = eventMuonSelections(df1_data_mu);

    saveSnapshot(df2_mc_mu, "output_mc_mu.root");
    saveSnapshot(df2_mc_el, "output_mc_el.root");
    saveSnapshot(df2_data_el, "output_data_el.root");
    saveSnapshot(df2_data_mu, "output_data_mu.root");

    makePlots("configs/output_el.json", df2_mc_el, df2_data_el);
    makePlots("configs/output_mu.json", df2_mc_mu, df2_data_mu);

    return 0;
}

#include "main.hpp"

#define INT_LUMI 26671.7

int main(int argc, char** argv) {
    // Config
    std::string mc_input_spec = "configs/input_mc_m.json";
    std::string data_input_spec = "configs/input_data_m.json";
    std::string output_json = "configs/output.json";
    std::string golden_jsons = "configs/goodrun/Cert_Collisions2022_355100_362760_Golden.json";

    const auto myLumiMask = lumiMask::fromJSON(golden_jsons);

    ROOT::EnableImplicitMT(32); // enable multithreaded rdfs
    ROOT::RDataFrame df_mc_ = ROOT::RDF::Experimental::FromSpec(mc_input_spec);
    ROOT::RDataFrame df_data_ = ROOT::RDF::Experimental::FromSpec(data_input_spec);
    ROOT::RDF::Experimental::AddProgressBar(df_mc_);
    ROOT::RDF::Experimental::AddProgressBar(df_data_);

    auto df_data = df_data_.DefinePerSample("sample_category", [](unsigned int slot, const RSampleInfo &id) { return id.GetS("sample_category"); });
    auto data_final = Muon::selections(df_data);
    // auto data_final = Weights::goodRun(myLumiMask, data_selections);

    auto df_mc = df_mc_.DefinePerSample("xsec", [](unsigned int slot, const RSampleInfo &id) { return id.GetD("xsec"); })
      .DefinePerSample("lumi", [](unsigned int slot, const RSampleInfo &id) { return id.GetD("lumi"); })
      .DefinePerSample("sumw", [](unsigned int slot, const RSampleInfo &id) { return id.GetD("sumw"); })
      .DefinePerSample("sample_category", [](unsigned int slot, const RSampleInfo &id) { return id.GetS("sample_category"); });
    auto mc_final = Muon::selections(df_mc);
    // auto mc_xsec = Weights::xsecWeights(mc_selections);
    // auto mc_pileup = Weights::pileupCorrection(mc_xsec);
    // auto mc_final = Weights::leptonScaleFactors(mc_pileup);

    Plot::makePlots(output_json, mc_final, data_final);

    df_mc_.Report()->Print();
    df_data_.Report()->Print();

    return 0;
}

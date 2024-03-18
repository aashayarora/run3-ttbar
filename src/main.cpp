#include "main.hpp"

#define INT_LUMI 26671.7

int main(int argc, char** argv) {
    // Config
    std::string outputBasePath = "./output/";

    std::string output_json = "configs/output.json";
    std::string golden_jsons = "configs/goodrun/Cert_Collisions2022_355100_362760_Golden.json";

    // HIST BINNING
    std::ifstream output_ifs(output_json);
    json output_data = json::parse(output_ifs);
    std::map<std::string, std::vector<float>> hist_binning;
    for (const auto& [key, value] : output_data.items()) {
        std::vector<float> binning;
        for (const auto& num : value) {
            binning.push_back(num);
        }
        hist_binning[key] = binning;
    }
    std::vector<std::string> finalVariables;
    for(auto const& imap: hist_binning) {
        finalVariables.push_back(imap.first);
    }
    
    const auto myLumiMask = lumiMask::fromJSON(golden_jsons);

    ROOT::EnableImplicitMT(32); // enable multithreaded rdfs
    std::string mc_input_spec = "configs/input_mc.json";
    std::string data_input_spec = "configs/input_data.json";
    ROOT::RDataFrame df_mc_ = ROOT::RDF::Experimental::FromSpec(mc_input_spec);
    ROOT::RDataFrame df_data_ = ROOT::RDF::Experimental::FromSpec(data_input_spec);
    ROOT::RDF::Experimental::AddProgressBar(df_mc_);
    ROOT::RDF::Experimental::AddProgressBar(df_data_);

    auto df_data = df_data_.DefinePerSample("sample_category", [](unsigned int slot, const RSampleInfo &id) { return id.GetS("sample_category"); });
    auto data_selections = MuonEG::selections(df_data);
    auto data_final = Weights::goodRun(myLumiMask, data_selections);

    auto df_mc = df_mc_.DefinePerSample("xsec", [](unsigned int slot, const RSampleInfo &id) { return id.GetD("xsec"); })
      .DefinePerSample("lumi", [](unsigned int slot, const RSampleInfo &id) { return id.GetD("lumi"); })
      .DefinePerSample("sumw", [](unsigned int slot, const RSampleInfo &id) { return id.GetD("sumw"); })
      .DefinePerSample("sample_category", [](unsigned int slot, const RSampleInfo &id) { return id.GetS("sample_category"); });
    auto mc_selections = MuonEG::selections(df_mc);
    auto mc_xsec = Weights::xsecWeights(mc_selections);
    auto mc_pileup = Weights::pileupCorrection(mc_xsec);
    auto mc_final = Weights::leptonScaleFactors(mc_pileup);

    // book all histogram calculations
    std::vector<ROOT::RDF::RResultPtr<TH1D>> mc_hists;
    std::vector<ROOT::RDF::RResultPtr<TH1D>> data_hists;
    // book all histogram calculations
    for (auto i : finalVariables) {
        if (hist_binning.count(i)){
            try {
                auto tmp_datahist = data_final.Histo1D({("data_" + i).c_str(), i.c_str(), (int)hist_binning[i][0], hist_binning[i][1], hist_binning[i][2]}, i.c_str(), "weights");
                auto tmp_mchist = mc_final.Histo1D({("mc_" + i).c_str(), i.c_str(), (int)hist_binning[i][0], hist_binning[i][1], hist_binning[i][2]}, i.c_str(), "weights");
                data_hists.push_back(tmp_datahist);
                mc_hists.push_back(tmp_mchist);
            }
            catch (const std::exception& e) {
                finalVariables.erase(std::remove(finalVariables.begin(), finalVariables.end(), i), finalVariables.end());
            }
        }
    }
    // actually make the histograms
    for (size_t i = 0; i < mc_hists.size(); i++) {
        auto tmp_datahist = data_hists[i];
        auto tmp_mchist = mc_hists[i];
        std::string title = tmp_mchist->GetTitle();
        Plot::makeHist(tmp_mchist, tmp_datahist, title.c_str(), outputBasePath + title + std::string(".png").c_str());
    }

    // mc_final.Snapshot("Events", outputBasePath + std::string("mc.root"), finalVariables);
    // data_final.Snapshot("Events", outputBasePath + std::string("data.root"), finalVariables);

    // auto report_mc = df_mc.Report();
    // auto report_data = df_data.Report();
    // cout << "\n\n" << "MC Report:" << endl;
    // report_mc->Print();
    // cout << "\n\n" << "Data Report:" << endl;
    // report_data->Print();

    return 0;
}

#include "common.hpp"

#define INT_LUMI 26671.7

void run(const char* mc_sample_name, const char* data_sample_name, bool save, bool report) {
    // Config
    std::string outputBasePath = "./output/";
    std::string input_datasets = "configs/input.json";
    std::string golden_jsons = "configs/goodrun/Cert_Collisions2022_355100_362760_Golden.json";

    // hist binning
    std::ifstream ifs("configs/output_muoneg.json");
    json hist_bin_data = json::parse(ifs);
    std::map<std::string, std::vector<float>> hist_binning;
    for (const auto& [key, value] : hist_bin_data.items()) {
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

    std::ifstream datasets_ifs(input_datasets);
    json json_datasets = json::parse(datasets_ifs);

    std::ifstream golden_json_ifs(golden_jsons);
    json golden_json = json::parse(golden_json_ifs);

    double xsec = json_datasets["MC"][mc_sample_name]["xsec"];
    double mc_sumw = json_datasets["MC"][mc_sample_name]["sumw"];

    cout << "Loaded configs..." << endl;

    // Build TChains from datasets defined in configs/datasets.json
    TChain* mcChain = new TChain("Events");
    TChain* dataChain = new TChain("Events");
    for (auto mc_dataset : json_datasets["MC"][mc_sample_name]["files"]) {
        mcChain->Add((std::string(mc_dataset) + std::string("*.root")).c_str());
    }
    for (auto data_dataset : json_datasets["Data"][data_sample_name]["files"]) {
        dataChain->Add((std::string(data_dataset) + std::string("*.root")).c_str());
    }

    cout << "Loaded TChains..." << endl;
    // Run dataframes
    
    cout << "Running data..." << endl;
    ROOT::RDataFrame df_data(*dataChain);
    ROOT::RDF::Experimental::AddProgressBar(df_data);
    auto data_weighted = Weights::goodRun(df_data, golden_json);
    auto data_final = MuonEG::selections(data_weighted);

    cout << "Running mc..." << endl;
    ROOT::RDataFrame df_mc(*mcChain);
    ROOT::RDF::Experimental::AddProgressBar(df_mc);
    auto mc_xsec = Weights::xsecWeights(df_mc, xsec, INT_LUMI, mc_sumw);
    auto mc_pileup = Weights::pileupCorrection(mc_xsec);
    auto mc_final = MuonEG::selections(mc_pileup);
    // auto mc_final = Weights::leptonScaleFactors(mc_selections);

    cout << "Running histograms..." << endl;
    // book all histogram calculations
    std::vector<ROOT::RDF::RResultPtr<TH1D>> mc_hists;
    std::vector<ROOT::RDF::RResultPtr<TH1D>> data_hists;
    // book all histogram calculations
    for (auto i : finalVariables) {
        if (hist_binning.count(i)){
            auto tmp_mchist = mc_final.Histo1D({("mc_" + i).c_str(), i.c_str(), (int)hist_binning[i][0], hist_binning[i][1], hist_binning[i][2]}, i.c_str(), "weights");
            auto tmp_datahist = data_final.Histo1D({("data_" + i).c_str(), i.c_str(), (int)hist_binning[i][0], hist_binning[i][1], hist_binning[i][2]}, i.c_str(), "weights");
            mc_hists.push_back(tmp_mchist);
            data_hists.push_back(tmp_datahist);
        }
    }
    // actually make the histograms
    for (size_t i = 0; i < mc_hists.size(); i++) {
        auto tmp_mchist = mc_hists[i];
        auto tmp_datahist = data_hists[i];
        std::string title = tmp_mchist->GetTitle();
        makeHist(tmp_mchist, tmp_datahist, title.c_str(), outputBasePath + title + std::string(".png").c_str());
    }

    // save screenshot and print report if prompted
    if (save) {
        cout << "--SAVE option selected, saving histograms and snapshots..." << endl;
        // Save snapshots after all cuts
        mc_final.Snapshot("Events", outputBasePath + std::string("mc.root") , finalVariables);
        data_final.Snapshot("Events", outputBasePath + std::string("data.root") , finalVariables);
    }

    if (report) {
        // print reports
        cout << "--REPORT option selected, print reports..." << endl;
        auto report_mc = df_mc.Report();
        auto report_data = df_data.Report();
        cout << "\n\n" << "MC Report:" << endl;
        report_mc->Print();
        cout << "\n\n" << "Data Report:" << endl;
        report_data->Print();
    }
}

int main(int argc, char** argv) {
    // parse command line arguments
    bool save = false;
    bool report = false;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (std::string(argv[i]) == "--save") {
                save = true;
            }
            if (std::string(argv[i]) == "--report") {
                report = true;
            }
        }
    }
    ROOT::EnableImplicitMT(16); // enable multithreaded rdfs
    run("TTBar", "MuonEG", save, report);
    return 0;
}

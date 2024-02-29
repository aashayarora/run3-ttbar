#include "main.hpp"

int main(int argc, char** argv) {
    // Config
    ROOT::EnableImplicitMT(); // enable multithreaded rdfs
    std::string outputBasePath = "./output/";
    std::map<std::string, std::vector<float>> hist_binning = {
        {"electron_pt", {100., 0., 200.}},
        {"electron_eta", {25., -2.5, 2.5}},
        {"electron_phi", {40., -3.14, 3.14}},
        {"muon_pt", {100., 0., 200.}},
        {"muon_eta", {25., -2.5, 2.5}},
        {"muon_phi", {40., -3.14, 3.14}},
        {"PV_npvs", {100,0,100}},
        {"m_ll", {100., 0., 500.}},
    };
    std::vector<std::string> finalVariables;
    for(auto const& imap: hist_binning) {
        finalVariables.push_back(imap.first);
    }
    std::ifstream datasets_ifs("configs/datasets.json");
    json json_datasets = json::parse(datasets_ifs);
    std::ifstream golden_json_ifs("configs/Cert_Collisions2022_355100_362760_Golden.json");
    json golden_json = json::parse(golden_json_ifs);

    cout << "Loaded configs..." << endl;

    // Build TChains from datasets defined in configs/datasets.json
    TChain* mcChain = new TChain("Events");
    TChain* dataChain = new TChain("Events");
    for (auto mc_dataset : json_datasets["MC"]["TTbar"]) {
        mcChain->Add((std::string(mc_dataset) + std::string("*.root")).c_str());
    }
    for (auto data_dataset : json_datasets["Data"]["MuonEG"]) {
        dataChain->Add((std::string(data_dataset) + std::string("*.root")).c_str());
    }

    cout << "Loaded TChains..." << endl;

    // Run dataframes
    cout << "Running data..." << endl;
    ROOT::RDataFrame df_data(*dataChain);
    auto data_weighted = Weights::goodRun(df_data, golden_json);
    auto data_lep_selected = Lepton::selections(data_weighted);
    auto data_final = Jet::selections(data_lep_selected);

    cout << "Running mc..." << endl;    
    ROOT::RDataFrame df_mc(*mcChain);
    auto mc_xsec = Weights::xsecWeights(df_mc);
    auto mc_pileup = Weights::primaryVerticesNormalization(mc_xsec);
    auto mc_lep_selected = Lepton::selections(mc_pileup);
    auto mc_final = Jet::selections(mc_lep_selected);

    cout << "Running histograms..." << endl;

    TFile* file = new TFile((outputBasePath + std::string("hists.root")).c_str(), "RECREATE");
    std::vector<ROOT::RDF::RResultPtr<TH1D>> mc_hists;
    std::vector<ROOT::RDF::RResultPtr<TH1D>> data_hists;
    
    for (auto i : finalVariables) {
        if (hist_binning.count(i)){
            auto tmp_mchist = mc_final.Histo1D({("mc_" + i).c_str(), i.c_str(), (int)hist_binning[i][0], hist_binning[i][1], hist_binning[i][2]}, i.c_str(), "weights_after_pv");
            auto tmp_datahist = data_final.Histo1D({("data_" + i).c_str(), i.c_str(), (int)hist_binning[i][0], hist_binning[i][1], hist_binning[i][2]}, i.c_str(), "weights");
            
            ROOT::RDF::RunGraphs({tmp_mchist, tmp_datahist});
            mc_hists.push_back(tmp_mchist);
            tmp_mchist->Write();
            data_hists.push_back(tmp_datahist);
            tmp_datahist->Write();
            makeHist(tmp_mchist, tmp_datahist, i.c_str(), outputBasePath + i + std::string(".png").c_str());
        }
    }

    if (argc == 2 && std::string(argv[1]) == "--SAVE") {
        cout << "--SAVE option selected, saving histograms and snapshots..." << endl;
        auto report_mc = df_mc.Report();
        auto report_data = df_data.Report();
        // Save snapshots after all cuts
        mc_final.Snapshot("Events", outputBasePath + std::string("mc.root") , finalVariables);
        data_final.Snapshot("Events", outputBasePath + std::string("data.root") , finalVariables);
        // print reports
        cout << "\n\n" << "MC Report:" << endl;
        report_mc->Print();
        cout << "\n\n" << "Data Report:" << endl;
        report_data->Print();
    }

    file->Close();
    return 0;
}
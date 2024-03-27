#include "plot.hpp"

namespace Plot {
    void makeHist(ROOT::RDF::RResultPtr<TH1D> mc, ROOT::RDF::RResultPtr<TH1D> data, const std::string &x_label, const std::string &filename)
    {
        gStyle->SetOptStat(0);
        gStyle->SetTextFont(42);
        auto c = new TCanvas("", "", 800, 700);
        c->SetLeftMargin(0.15);

        auto h_mc = *mc;
        h_mc.SetLineWidth(2);
        h_mc.SetLineColor(kRed);
        // h_mc.GetXaxis()->SetRange(1, h_mc.GetNbinsX() + 1);

        auto h_data = *data;
        h_data.SetMarkerStyle(20);
        h_data.SetMarkerSize(1.0);
        h_data.SetLineWidth(1);
        h_data.SetMarkerColor(kBlack);
        h_data.SetLineColor(kBlack);
        // h_data.GetXaxis()->SetRange(1, h_data.GetNbinsX() + 1);

        auto rp = new TRatioPlot(&h_mc, &h_data);
        rp->Draw();
        rp->GetLowYaxis()->SetNdivisions(505);

        rp->GetUpperRefYaxis()->SetTitle("Events");
        rp->GetLowerRefYaxis()->SetTitle("MC/Data");
        rp->GetUpperRefYaxis()->SetRangeUser(0., 1.2 * std::max(h_mc.GetMaximum(), h_data.GetMaximum()));
        rp->GetLowerRefYaxis()->SetRangeUser(0.5, 1.5);
        rp->GetUpperRefXaxis()->SetTitle(x_label.c_str());
        rp->GetUpperPad()->cd();

        TLegend legend(.8,.75,.89,.89);
        legend.SetFillColor(0);
        legend.SetBorderSize(0);
        legend.SetTextSize(0.03);
        legend.AddEntry(&h_data, "Data", "PE1");
        legend.AddEntry(&h_mc, "MC", "f");
        legend.DrawClone();

        // add a text box with integrals of the MC and data hists
        TLatex text;
        text.SetTextSize(0.03);
        text.DrawLatexNDC(0.8, 0.6, Form("MC: %.0f", h_mc.Integral()));
        text.DrawLatexNDC(0.8, 0.55, Form("Data: %.0f", h_data.Integral()));

        TLatex cms_label;
        cms_label.SetTextSize(0.04);
        cms_label.DrawLatexNDC(0.16, 0.965, "#bf{CMS} #it{Exercise}");
        TLatex header;
        header.SetTextSize(0.04);
        header.DrawLatexNDC(0.63, 0.965, "#sqrt{s} = 13.6 TeV, L_{int} = 27 fb^{-1}");

        c->Update();
        c->SaveAs(filename.c_str());
        c->Clear();
    }

    void makePlots(std::string output_json, RNode mc_final, RNode data_final) {
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
        // book all histogram calculations
        std::vector<ROOT::RDF::RResultPtr<TH1D>> mc_hists;
        std::vector<ROOT::RDF::RResultPtr<TH1D>> data_hists;
        // book all histogram calculations
        for (auto i : finalVariables) {
            if (hist_binning.count(i)){
                try {
                    auto tmp_datahist = data_final.Histo1D({("data_" + i).c_str(), i.c_str(), (int)hist_binning[i][0], hist_binning[i][1], hist_binning[i][2]}, i.c_str());//, "weights");
                    auto tmp_mchist = mc_final.Histo1D({("mc_" + i).c_str(), i.c_str(), (int)hist_binning[i][0], hist_binning[i][1], hist_binning[i][2]}, i.c_str());//, "weights");
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
            Plot::makeHist(tmp_mchist, tmp_datahist, title.c_str(), std::string("output/") + title + std::string(".png").c_str());
        }
    }
}
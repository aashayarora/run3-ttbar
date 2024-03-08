#include "common.hpp"

void makeHist(ROOT::RDF::RResultPtr<TH1D> mc, ROOT::RDF::RResultPtr<TH1D> data, const std::string &x_label, const std::string &filename)
{
    gStyle->SetOptStat(0);
    gStyle->SetTextFont(42);
    auto c = new TCanvas("", "", 800, 700);
    c->SetLeftMargin(0.15);

    auto h_mc = *mc;
    h_mc.SetLineWidth(2);
    h_mc.SetLineColor(kRed);

    auto h_data = *data;
    h_data.SetMarkerStyle(20);
    h_data.SetMarkerSize(1.0);
    h_data.SetLineWidth(1);
    h_data.SetMarkerColor(kBlack);
    h_data.SetLineColor(kBlack);

    auto rp = new TRatioPlot(&h_mc, &h_data);
    rp->Draw();
    rp->GetLowYaxis()->SetNdivisions(505);

    rp->GetUpperRefYaxis()->SetTitle("Events");
    rp->GetLowerRefYaxis()->SetTitle("MC/Data");
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

RooDataSet fit(std::string var, RNode df) {   
    RooRealVar x(var.c_str(), var.c_str(), 0, 200);
    ROOT::RDF::RResultPtr<RooDataSet> rooDataSetResult = df.Book<double>(RooDataSetHelper("RooDataSet", "RooDataSet", RooArgSet(x)), {var.c_str()});
    return *rooDataSetResult;
}
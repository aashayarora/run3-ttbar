#ifndef PLOT_HPP
#define PLOT_HPP

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "ROOT/RVec.hxx"

#include "TCanvas.h"
#include "TStyle.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TRatioPlot.h"

using RNode = ROOT::RDF::RNode;

namespace Plot {
    void makeHist(ROOT::RDF::RResultPtr<TH1D> mc, ROOT::RDF::RResultPtr<TH1D> data, const std::string &x_label, const std::string &filename);
}

#endif
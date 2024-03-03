#ifndef COMMON_H
#define COMMON_H

// C++ Includes
#include <iostream>
#include <vector>
#include <string>
#include <map>

// RDF Includes
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "ROOT/RVec.hxx"

// ROOT Includes
#include "TChain.h"
#include "TRatioPlot.h"

// Plotting
#include "Math/Vector4D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TLegend.h"

// Roofit
#include "RooRealVar.h"
#include "RooAbsDataHelper.h"

// JSON parsing
#include <nlohmann/json.hpp>

#include "correction.h"

using json = nlohmann::json;
using std::cout, std::endl;
using RNode = ROOT::RDF::RNode;
using correction::CorrectionSet;
using ROOT::VecOps::RVec;

namespace Lepton {
    RNode triggers(RNode df);
    RNode preSelections(RNode df);
    RNode defineVariables(RNode df);
    RNode selections(RNode df);
}

namespace Jet {
    RNode preSelections(RNode df);
    RNode defineVariables(RNode df);
    RNode selections(RNode df);
}

namespace Weights {
    RNode goodRun(RNode df, json jf);
    RNode xsecWeights(RNode df, double xsec, double int_lumi, double n_events);
    RNode leptonScaleFactors(RNode df);
    RNode pileupCorrection(RNode df);
}

// Plotting
void makeHist(ROOT::RDF::RResultPtr<TH1D> mc, ROOT::RDF::RResultPtr<TH1D> data, const std::string &x_label, const std::string &filename);

RooDataSet fit(std::string var, RNode df);
#endif
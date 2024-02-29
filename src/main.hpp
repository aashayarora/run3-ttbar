#ifndef MAIN_H
#define MAIN_H

// C++ Includes
#include <iostream>
#include <vector>
#include <string>
#include <map>

// RDF Includes
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"

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
    RNode xsecWeights(RNode df);
    RNode primaryVerticesNormalization(RNode df_mc);
}

// Plotting
void makeHist(ROOT::RDF::RResultPtr<TH1D> mc, ROOT::RDF::RResultPtr<TH1D> data, const std::string &x_label, const std::string &filename);

RooDataSet fit(RNode df);

#endif
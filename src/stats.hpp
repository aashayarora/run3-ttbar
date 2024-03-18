// Roofit
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "ROOT/RVec.hxx"

#include "RooRealVar.h"
#include "RooAbsDataHelper.h"

using RNode = ROOT::RDF::RNode;

RooDataSet fit(std::string var, RNode df);
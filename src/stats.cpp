#include "stats.hpp"

RooDataSet fit(std::string var, RNode df) {   
    RooRealVar x(var.c_str(), var.c_str(), 0, 200);
    ROOT::RDF::RResultPtr<RooDataSet> rooDataSetResult = df.Book<double>(RooDataSetHelper("RooDataSet", "RooDataSet", RooArgSet(x)), {var.c_str()});
    return *rooDataSetResult;
}
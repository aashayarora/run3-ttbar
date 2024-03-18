#ifndef WEIGHTS_HPP
#define WEIGHTS_HPP

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "ROOT/RVec.hxx"

#include "utils.hpp"
#include "correction.h"

using correction::CorrectionSet;
using RNode = ROOT::RDF::RNode;
using ROOT::VecOps::RVec;

namespace Weights {
    RNode goodRun(lumiMask golden, RNode df);
    RNode xsecWeights(RNode df);
    RNode leptonScaleFactors(RNode df);
    RNode pileupCorrection(RNode df);
    RNode jetEnergyCorrections(RNode df, std::string era, int era_hash);
}

#endif
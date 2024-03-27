#ifndef WEIGHTS_H
#define WEIGHTS_H

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "ROOT/RVec.hxx"

#include "lumimask.h"
#include "correction.h"

using correction::CorrectionSet;
using RNode = ROOT::RDF::RNode;
using ROOT::VecOps::RVec;

RNode goodRun(lumiMask golden, RNode df);
RNode leptonScaleFactors(RNode df);
RNode pileupCorrection(RNode df);
// RNode jetEnergyCorrections(RNode df, std::string era, int era_hash);

#endif
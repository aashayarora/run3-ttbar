#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector4D.h"
#include "TString.h"

using RNode = ROOT::RDF::RNode;
using ROOT::VecOps::RVec;

float get_inv_mass(RVec<float> pt, RVec<float> eta, RVec<float> phi, RVec<float> mass);
void saveSnapshot(RNode df, const std::string& finalFile);

#endif
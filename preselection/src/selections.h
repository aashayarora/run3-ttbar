#ifndef SELECTIONS_H
#define SELECTIONS_H

#include "ROOT/RDataFrame.hxx"

using RNode = ROOT::RDF::RNode;
using ROOT::VecOps::RVec;

#include "utils.h"

RNode flagSelections(RNode df);
RNode triggerSelections(RNode df);
RNode electronSelections(RNode df);
RNode muonSelections(RNode df);
RNode leptonSelections(RNode df);
RNode jetSelections(RNode df);
RNode bJetSelections(RNode df);

#endif // SELECTIONS_H
#ifndef WEIGHTS_H
#define WEIGHTS_H

#pragma once

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "ROOT/RVec.hxx"

#include "utils.h"
#include "correction.h"

using correction::CorrectionSet;
using RNode = ROOT::RDF::RNode;
using ROOT::VecOps::RVec;

const auto LumiMask = lumiMask::fromJSON("corrections/goodrun/Cert_Collisions2022_355100_362760_Golden.json");
const auto cset_pileup = CorrectionSet::from_file("corrections/pileup/puWeights.json")->at("Collisions2022_359022_362760_eraEFG_GoldenJson");
const auto cset_electron = CorrectionSet::from_file("corrections/scalefactors/electron/electron.json")->at("Electron-ID-SF");
const auto cset_muon_ID = CorrectionSet::from_file("corrections/scalefactors/muon/ScaleFactors_Muon_Z_ID_ISO_2022_EE_schemaV2.json")->at("NUM_MediumID_DEN_TrackerMuons");
const auto cset_muon_ISO = CorrectionSet::from_file("corrections/scalefactors/muon/ScaleFactors_Muon_Z_ID_ISO_2022_EE_schemaV2.json")->at("NUM_TightPFIso_DEN_MediumID");
const auto cset_muon_HLT = CorrectionSet::from_file("corrections/scalefactors/muon/ScaleFactors_Muon_Z_HLT_2022_EE_abseta_pt_schemaV2.json")->at("NUM_IsoMu24_or_Mu50_or_CascadeMu100_or_HighPtTkMu100_DEN_CutBasedIdGlobalHighPt_and_TkIsoLoose");
const auto cset_jet_veto = CorrectionSet::from_file("corrections/scalefactors/jets/jetvetomaps.json")->at("Summer22EE_23Sep2023_RunEFG_V1");

RNode goodRun(lumiMask golden, RNode df);
RNode electronScaleFactors(correction::Correction::Ref cset_electron, RNode df);
RNode muonScaleFactors(correction::Correction::Ref cset_muon_ID, correction::Correction::Ref cset_muon_ISO, correction::Correction::Ref cset_muon_HLT, RNode df);
RNode pileupCorrection(correction::Correction::Ref cset_pileup, RNode df);
RNode jetveto(correction::Correction::Ref cset_jet_veto, RNode df);
RNode finalMCWeight(RNode df);

#endif
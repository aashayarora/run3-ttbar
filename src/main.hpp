#ifndef MAIN_HPP
#define MAIN_HPP

// C++ Includes
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

// RDF Includes
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "ROOT/RVec.hxx"

// ROOT Includes
#include "TChain.h"

// JSON parsing
#include <nlohmann/json.hpp>

#include "plot.hpp"
#include "weights.hpp"
#include "selections.hpp"
#include "stats.hpp"

using json = nlohmann::json;
using std::cout, std::endl;
using ROOT::RDF::RSampleInfo;

#endif
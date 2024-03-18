// RDF Includes
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector4D.h"

using RNode = ROOT::RDF::RNode;
using ROOT::VecOps::RVec;

namespace MuonEG {
    RNode leptonTriggers(RNode df);
    RNode leptonSelections(RNode df);
    RNode jetSelections(RNode df);
    RNode selections(RNode df);
}
namespace Muon {
    RNode leptonTriggers(RNode df);
    RNode leptonSelections(RNode df);
    RNode jetSelections(RNode df);
    RNode selections(RNode df);
}
namespace EGamma {
    RNode leptonTriggers(RNode df);
    RNode leptonSelections(RNode df);
    RNode jetSelections(RNode df);
    RNode selections(RNode df);
}

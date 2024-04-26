#ifndef UTILS_H
#define UTILS_H

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <unordered_map>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "ROOT/RVec.hxx"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "TString.h"

using RNode = ROOT::RDF::RNode;
using ROOT::VecOps::RVec;

/*
    LUMIMASK
*/

class lumiMask {
public:
    using Run = unsigned int;
    using LumiBlock = unsigned int;

    class LumiBlockRange {
    public:
        LumiBlockRange(Run run, LumiBlock firstLumi, LumiBlock lastLumi) : m_run(run), m_firstLumi(firstLumi), m_lastLumi(lastLumi ? lastLumi : std::numeric_limits<LumiBlock>::max()) {}
        Run run() const { return m_run; }
        LumiBlock firstLumi() const { return m_firstLumi; }
        LumiBlock lastLumi () const { return m_lastLumi ; }
    private:
        Run m_run;
        LumiBlock m_firstLumi;
        LumiBlock m_lastLumi;
    };

    explicit lumiMask(const std::vector<LumiBlockRange>& accept) : m_accept(accept) {
        std::sort(m_accept.begin(), m_accept.end());
    }

    bool accept(Run run, LumiBlock lumi) const { 
        return std::binary_search(m_accept.begin(), m_accept.end(), LumiBlockRange(run, lumi, lumi)); 
    }

    static lumiMask fromJSON(const std::string& fileName, lumiMask::Run firstRun=0, lumiMask::Run lastRun=0);

private:
    std::vector<LumiBlockRange> m_accept;
};

bool operator< ( const lumiMask::LumiBlockRange& lh, const lumiMask::LumiBlockRange& rh );


/*
    DEFINE METADATA
*/

RNode defineMetadata(RNode df);

/*
    DUPLICATE REMOVAL
*/

class FilterOnePerKind {
    std::unordered_set<size_t> _seenCategories;
public:
    bool operator()(unsigned int run, unsigned int luminosityBlock, unsigned long long event) {
        std::hash<std::string> categoryHasher;
        std::string eventStr = std::to_string(run) + "," + std::to_string(luminosityBlock) + "," + std::to_string(event);
        size_t category = categoryHasher(eventStr);
        {
        // build char category from run, luminosityBlock, event
        R__READ_LOCKGUARD(ROOT::gCoreMutex); // many threads can take a read lock concurrently
        if (_seenCategories.count(category) == 1)
            return false;
        }
        // if we are here, `category` was not already in _seenCategories
        R__WRITE_LOCKGUARD(ROOT::gCoreMutex); // only one thread at a time can take the write lock
        _seenCategories.insert(category);
        return true;
    }
};

RNode removeDuplicates(RNode df);

/*
    SELECTION UTILS
*/

float PNetBtagWP(std::string wp);

RVec<float> fDeltaR (RVec<float> vec_eta, RVec<float> vec_phi, float obj_eta, float obj_phi);

void saveSnapshot(RNode df, const std::string& finalFile);

#endif
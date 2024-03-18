// cpp code to parse a json using boost::property_tree and print the values

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <string>

int main() {
    std::ifstream is("configs/goodrun/Cert_Collisions2022_355100_362760_Golden.json");
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(is, pt);
    
    unsigned int run = 3;
    unsigned int lumi = 5;
    auto extract_from_gj = [&pt](unsigned int run, unsigned int lumi) {
        std::string run_str = std::to_string(run);
        unsigned int firstLumi, lastLumi;
        if (pt.count(run_str) > 0) {
            auto it = pt.get_child(run_str);
            for (auto it2 = it.begin(); it2 != it.end(); ++it2) {
                for (auto it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
                {
                    firstLumi = std::stoul(it3->second.data());
                    lastLumi = std::stoul((++it3)->second.data());
                    if (lumi >= firstLumi && lumi <= lastLumi) {
                        return 1.0;
                    }
                }
            }
        }
        return 0.0;
    };
    std::cout << extract_from_gj(run, lumi) << std::endl;
}
#ifndef FDVS_SRC_STRATEGY_HPP_
#define FDVS_SRC_STRATEGY_HPP_

#include "Models.hpp"
#include "PricingPolicies.hpp"
#include <fstream>
#include <functional>
#include <unordered_map>
#include <utility>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace Strategy {
struct Strategy : public Models::R2RFuncsGenerator {
  // Parameters of the contracts cannot be varied (e.g., strike, exec_price,
  // ...)
  // Even the variable parameters should have default values in params_
  Strategy() = default;
  Strategy(std::string strategy_name,
           std::unordered_map<std::string, Models::OptionContract> portfolio,
           std::array<std::string, 2> to_vary,
           std::shared_ptr<Models::PricingPolicy> policy);
  Strategy(int qty, std::array<std::string, 2> to_vary, std::string filename,
           std::shared_ptr<Models::PricingPolicy> policy);
  void WriteToFile();

  void AddToPortfolio(std::pair<std::string, Models::OptionContract> new_leg);
  void RemoveFromPortfolio(std::string name);

  void SetParam(std::string name, double val) {
    for (auto &[key, leg] : portfolio_) {
      leg.SetParam(name, val);
    }
  }

  // Ideally I want to stream this from an interface, so that we can get a
  // moving line in 3D space (Spot, Time, Option Price)
  void UpdateCurrentPrices(
      std::initializer_list<std::pair<std::string, double>> new_prices);

  // Interface Implementation
  std::function<float(double, double)> GetFunction(std::string name);
  float ComputePrice(double x, double y);
  // float ComputeModeledPL(double x, double y);
  float ComputeDelta(double x, double y);
  float ComputeGamma(double x, double y);
  float ComputeTheta(double x, double y);
  float ComputeVega(double x, double y);

  // Properties
  std::string strategy_name_;

  void SetDefaults() {
    for (auto &[key, value] : portfolio_) {
      value.SetDefaults();
    }
  }
  // Will do this via a root-finding algorithm using PricingFunction
  float GetBreakevenPoint();
  float Price();
  // float ModeledPL();
  float Delta();
  float Gamma();
  float Theta();
  float Vega();
  void UpdateProperties();
  float GetTotalPL();
  double delta_ = 0.0f;
  double gamma_ = 0.0f;
  double theta_ = 0.0f;
  double vega_ = 0.0f;
  double current_pl_ = 0.0f;
  double breakeven_spot_price_ = 0.0f;

  double time_since_start = 0.0f;

  ~Strategy() {}

private:
  std::unordered_map<std::string, Models::OptionContract> portfolio_;
  std::array<std::string, 2> to_vary_;
  int qty_;
  std::shared_ptr<Models::PricingPolicy> policy_;
};
} // namespace Strategy

#endif // !FDVS_SRC_STRATEGY_HPP_

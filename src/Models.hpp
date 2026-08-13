#ifndef FDVS_SRC_MODELS_HPP
#define FDVS_SRC_MODELS_HPP

#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <numbers>
#include <string>
#include <unordered_map>

using json = nlohmann::json;

namespace Models {

enum class PayoffType { Call = 1, Put = -1 };
enum class ComputeFuncs { Price, PL, Delta, Gamma, Theta, Vega, None };
const std::unordered_map<std::string, ComputeFuncs> funcs{

    {"Option Price", ComputeFuncs::Price}, {"PL", ComputeFuncs::PL},
    {"Delta", ComputeFuncs::Delta},        {"Gamma", ComputeFuncs::Gamma},
    {"Theta", ComputeFuncs::Theta},        {"Vega", ComputeFuncs::Vega}};

ComputeFuncs GetFuncID(const std::string &func_name);

class R2RFuncsGenerator {
public:
  R2RFuncsGenerator() = default;
  std::function<float(double, double)> GetFunction(std::string name);
};

struct OptionContract {

  // Constructors
  OptionContract() = default;
  OptionContract(int qty, PayoffType payoff_type,
                 std::map<std::string, double> params,
                 std::array<std::string, 2> to_vary);
  //            std::shared_ptr<PricingPolicy> policy);
  /*
  // Implementation of R2RFuncsGenerator Interface
  std::function<float(double, double)> GetFunction(std::string name);

  // Computations
  void SetPricingPolicy(std::shared_ptr<PricingPolicy> policy) {
    policy_ = policy;
  }
  float ComputePrice(double x, double y);
  float ComputeModeledPL(double x, double y);
  float ComputeDelta(double x, double y);
  float ComputeGamma(double x, double y);
  float ComputeTheta(double x, double y);
  float ComputeVega(double x, double y);
  */
  double PL(double current_price) {
    current_price_ = current_price;
    return qty_ * (current_price - params_.at("Purchase Price"));
  }
  double PL() { return qty_ * (current_price_ - params_.at("Purchase Price")); }
  double ModeledPL();

  // Set Parameters
  void SetParam(std::string name, double val) { params_.at(name) = val; }
  void SetParam(PayoffType payoff_type) { payoff_type_ = payoff_type; }
  void SetParam(int qty) { qty_ = qty; }
  void SetDefaults() {
    for (auto &[key, value] : defaults)
      SetParam(key, value);
  }

  ~OptionContract() {}

  PayoffType payoff_type_;
  int qty_;
  double current_price_ = 0.0f;
  std::map<std::string, double> params_;
  std::array<std::string, 2> to_vary_;

protected:
  // std::shared_ptr<PricingPolicy> policy_;
  std::unordered_map<std::string, double> defaults;
};

void to_json(json &j, const OptionContract &op);
void from_json(const json &j, OptionContract &op);

} // namespace Models

#endif // !FDVS_SRC_MODELS_HPP

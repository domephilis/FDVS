#ifndef FDVS_SRC_MODELS_HPP
#define FDVS_SRC_MODELS_HPP

#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <numbers>
#include <string>
#include <unordered_map>

namespace Models {

enum class PayoffType { Call = 1, Put = -1 };
enum class ComputeFuncs { Price, Delta, Gamma, Theta, Vega, None };
const std::unordered_map<std::string, ComputeFuncs> funcs{
    {"Option Price", ComputeFuncs::Price},
    {"Delta", ComputeFuncs::Delta},
    {"Gamma", ComputeFuncs::Gamma},
    {"Theta", ComputeFuncs::Theta},
    {"Vega", ComputeFuncs::Vega}};

ComputeFuncs GetFuncID(const std::string &func_name);

class OptionsPricer {
public:
  // Preconditions:
  // 1. params must contain everything even if it is unknown
  // 2. There may be at most 2 values that one may vary initialized to -1
  // 3. Any string in to_vary must be one of the keys
  OptionsPricer(PayoffType payoff_type, std::map<std::string, double> params,
                std::array<std::string, 2> to_vary);
  void SetParam(std::string name, float val) { params_.at(name) = val; }
  void SetParam(PayoffType payoff_type) { payoff_type_ = payoff_type; }
  float ComputePrice(double x, double y) {
    SetParam(to_vary_[0], x);
    SetParam(to_vary_[1], y);
    return PricingFunction();
  }
  float ComputeDelta(double x, double y) {
    SetParam(to_vary_[0], x);
    SetParam(to_vary_[1], y);
    return Delta();
  }
  float ComputeGamma(double x, double y) {
    SetParam(to_vary_[0], x);
    SetParam(to_vary_[1], y);
    return Gamma();
  }
  float ComputeTheta(double x, double y) {
    SetParam(to_vary_[0], x);
    SetParam(to_vary_[1], y);
    return Theta();
  }
  float ComputeVega(double x, double y) {
    SetParam(to_vary_[0], x);
    SetParam(to_vary_[1], y);
    return Vega();
  }
  virtual std::function<float(double, double)>
  GetComputeFunction(std::string to_compute_) = 0;
  virtual float PricingFunction() = 0;
  virtual float Delta() = 0;
  virtual float Gamma() = 0;
  virtual float Theta() = 0;
  virtual float Vega() = 0;
  virtual ~OptionsPricer() {}

protected:
  PayoffType payoff_type_;
  std::map<std::string, double> params_;
  std::array<std::string, 2> to_vary_;
};

class BlackScholes : public OptionsPricer {
public:
  BlackScholes(PayoffType payoff_type, std::map<std::string, double> params,
               std::array<std::string, 2> to_vary);
  std::function<float(double, double)>
  GetComputeFunction(std::string name) override {
    switch (GetFuncID(name)) {
    case ComputeFuncs::Price:
      return std::bind(&BlackScholes::ComputePrice, this, std::placeholders::_1,
                       std::placeholders::_2);
      break;
    case ComputeFuncs::Delta:
      return std::bind(&BlackScholes::ComputeDelta, this, std::placeholders::_1,
                       std::placeholders::_2);
      break;
    case ComputeFuncs::Gamma:
      return std::bind(&BlackScholes::ComputeGamma, this, std::placeholders::_1,
                       std::placeholders::_2);
      break;
    case ComputeFuncs::Theta:
      return std::bind(&BlackScholes::ComputeTheta, this, std::placeholders::_1,
                       std::placeholders::_2);
      break;
    case ComputeFuncs::Vega:
      return std::bind(&BlackScholes::ComputeVega, this, std::placeholders::_1,
                       std::placeholders::_2);
      break;
    default:
      std::cerr << "Did not pass in the right name." << std::endl;
      return std::function<float(double, double)>();
      break;
    }
  }
  float PricingFunction() override;
  float Delta() override;
  float Gamma() override;
  float Theta() override;
  float Vega() override;

private:
  std::function<double(double)> norm_cdf;
  std::function<double(double)> norm_pdf;
};

} // namespace Models

#endif // !FDVS_SRC_MODELS_HPP

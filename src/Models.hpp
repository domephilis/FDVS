#ifndef FDVS_SRC_MODELS_HPP
#define FDVS_SRC_MODELS_HPP

#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <string>

namespace Models {

enum class PayoffType { Call = 1, Put = -1 };

class OptionsPricer {
public:
  // Preconditions:
  // 1. params must contain everything even if it is unknown
  // 2. There may be at most 2 values that one may vary initialized to -1
  // 3. Any string in to_vary must be one of the keys
  OptionsPricer(PayoffType payoff_type, std::map<std::string, float> params,
                std::array<std::string, 2> to_vary);
  void SetParam(std::string name, float val) { params_.at(name) = val; }
  void SetParam(PayoffType payoff_type) { payoff_type_ = payoff_type; }
  float Compute(float x, float y) {
    SetParam(to_vary_[0], x);
    SetParam(to_vary_[1], y);
    /*
    std::cerr << "{" + to_vary_[0] + ": " << params_.at(to_vary_[0])
              << ", " + to_vary_[1] + ": " << params_.at(to_vary_[0]) << "}"
              << std::endl;*/
    return PricingFunction();
  }
  virtual std::function<float(float, float)> GetComputeFunction() = 0;
  virtual float PricingFunction() = 0;
  virtual ~OptionsPricer() {}

protected:
  PayoffType payoff_type_;
  std::map<std::string, float> params_;
  std::array<std::string, 2> to_vary_;
};

class BlackScholes : public OptionsPricer {
public:
  BlackScholes(PayoffType payoff_type, std::map<std::string, float> params,
               std::array<std::string, 2> to_vary);
  std::function<float(float, float)> GetComputeFunction() override {
    return std::bind(&BlackScholes::Compute, this, std::placeholders::_1,
                     std::placeholders::_2);
  }
  float PricingFunction() override;
};

} // namespace Models

#endif // !FDVS_SRC_MODELS_HPP

#ifndef FDVS_SRC_PRICING_POLICIES_HPP_
#define FDVS_SRC_PRICING_POLICIES_HPP_

#include <cmath>
#include <functional>
#include <memory>
#include <numbers>

#include "Models.hpp"

namespace Models {

class PricingPolicy {
public:
  virtual float Price(OptionContract &contract) = 0;
  virtual float Delta(OptionContract &contract) = 0;
  virtual float Gamma(OptionContract &contract) = 0;
  virtual float Theta(OptionContract &contract) = 0;
  virtual float Vega(OptionContract &contract) = 0;

  virtual ~PricingPolicy() {}

protected:
  PricingPolicy() = default;
};

struct BlackScholes : public PricingPolicy {
  BlackScholes();
  static std::shared_ptr<Models::PricingPolicy> policy() {
    static std::shared_ptr<Models::PricingPolicy> obj_ptr =
        std::dynamic_pointer_cast<Models::PricingPolicy>(
            std::make_shared<BlackScholes>());
    return obj_ptr;
  }
  float Price(OptionContract &contract);
  float Delta(OptionContract &contract);
  float Gamma(OptionContract &contract);
  float Theta(OptionContract &contract);
  float Vega(OptionContract &contract);
  ~BlackScholes() {}

private:
  std::function<double(double)> norm_cdf;
  std::function<double(double)> norm_pdf;
};

} // namespace Models

#endif

#include "Models.hpp"

Models::OptionsPricer::OptionsPricer(PayoffType payoff_type,
                                     std::map<std::string, float> params,
                                     std::array<std::string, 2> to_vary)
    : payoff_type_(payoff_type), params_(params), to_vary_(to_vary) {

  // Later on I want to check preconditions, but I just need an MVP for now
  //
}

Models::BlackScholes::BlackScholes(PayoffType payoff_type,
                                   std::map<std::string, float> params,
                                   std::array<std::string, 2> to_vary)
    : OptionsPricer(payoff_type, params, to_vary) {}

float Models::BlackScholes::PricingFunction() {
  float S, K, time_to_exp, sigma, rate, div;
  S = params_["Spot Price"];
  K = params_["Strike Price"];
  time_to_exp = params_["Time To Expiry"];
  sigma = params_["Implied Volatility"];
  rate = params_["Risk Free Rate"];
  div = params_["Dividend Rate"];

  const int phi = static_cast<int>(payoff_type_);

  if (time_to_exp > 0.0) {
    double d1 = (std::log(S / K) +
                 (rate - div + (std::pow(sigma, 2) / 2.0f)) * time_to_exp) /
                (sigma * std::sqrt(time_to_exp));
    double d2 = d1 - sigma * std::sqrt(time_to_exp);

    auto norm_cdf = [](double x) {
      return (1.0 + std::erf(x / std::sqrt(2))) / 2.0f;
    };

    double nd_1 = norm_cdf(phi * d1);
    double nd_2 = norm_cdf(phi * d2);
    double disc_fctr = std::exp(-rate * time_to_exp);
    return phi * (S * std::exp(-div * time_to_exp)) * nd_1 -
           disc_fctr * K * nd_2;
  } else {
    return std::max(phi * (K - S), 0.0f);
  }
}

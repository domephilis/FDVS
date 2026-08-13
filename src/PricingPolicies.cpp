#include "PricingPolicies.hpp"

Models::BlackScholes::BlackScholes() {
  norm_cdf = [](double x) -> double {
    return (1.0 + std::erf(x / std::sqrt(2))) / 2.0f;
  };
  norm_pdf = [](double x) -> double {
    return (std::exp(-std::pow(x, 2) / 2)) / std::sqrt(2 * std::numbers::pi);
  };
}

float Models::BlackScholes::Price(OptionContract &contract) {
  double S, K, time_to_exp, sigma, rate, div;
  S = contract.params_["Spot Price"];
  K = contract.params_["Strike Price"];
  time_to_exp = contract.params_["Time To Expiry"];
  sigma = contract.params_["Implied Volatility"];
  rate = contract.params_["Risk Free Rate"];
  div = contract.params_["Dividend Rate"];

  const int phi = static_cast<int>(contract.payoff_type_);

  if (time_to_exp > 0.0) {
    double d1 = (std::log(S / K) +
                 (rate - div + (std::pow(sigma, 2) / 2.0f)) * time_to_exp) /
                (sigma * std::sqrt(time_to_exp));
    double d2 = d1 - sigma * std::sqrt(time_to_exp);

    double nd_1 = norm_cdf(phi * d1);
    double nd_2 = norm_cdf(phi * d2);
    double disc_fctr = std::exp(-rate * time_to_exp);
    return static_cast<float>(phi * (S * std::exp(-div * time_to_exp)) * nd_1 -
                              disc_fctr * K * nd_2);
  } else {
    return static_cast<float>(
        std::max(phi * (K - S), static_cast<double>(0.0f)));
  }
}

float Models::BlackScholes::Delta(OptionContract &contract) {
  double S, K, time_to_exp, sigma, rate, div;
  double delta = 0.0f;
  S = contract.params_["Spot Price"];
  K = contract.params_["Strike Price"];
  time_to_exp = contract.params_["Time To Expiry"];
  sigma = contract.params_["Implied Volatility"];
  rate = contract.params_["Risk Free Rate"];
  div = contract.params_["Dividend Rate"];

  const int phi = static_cast<int>(contract.payoff_type_);

  double d1 = (std::log(S / K) +
               (rate - div + (std::pow(sigma, 2) / 2.0f)) * time_to_exp) /
              (sigma * std::sqrt(time_to_exp));
  double d2 = d1 - sigma * std::sqrt(time_to_exp);

  // Note: There's an extra discount term if you want dividends;
  // For now, I just want to replicate what worked before.

  if (phi == 1)
    delta = norm_cdf(d1);
  else if (phi == -1)
    delta = norm_cdf(d1) - 1;
  return contract.qty_ * delta;
}

float Models::BlackScholes::Gamma(OptionContract &contract) {
  double S, K, time_to_exp, sigma, rate, div;
  double gamma = 0.0f;
  S = contract.params_["Spot Price"];
  K = contract.params_["Strike Price"];
  time_to_exp = contract.params_["Time To Expiry"];
  sigma = contract.params_["Implied Volatility"];
  rate = contract.params_["Risk Free Rate"];
  div = contract.params_["Dividend Rate"];

  double d1 = (std::log(S / K) +
               (rate - div + (std::pow(sigma, 2) / 2.0f)) * time_to_exp) /
              (sigma * std::sqrt(time_to_exp));
  double d2 = d1 - sigma * std::sqrt(time_to_exp);

  // Note: Gamma is the same regardless of put/call
  gamma = norm_pdf(d1) / (S * sigma * std::sqrt(time_to_exp));
  return contract.qty_ * gamma;
}

float Models::BlackScholes::Theta(OptionContract &contract) {
  double S, K, time_to_exp, sigma, rate, div;
  double theta = 0.0f;
  S = contract.params_["Spot Price"];
  K = contract.params_["Strike Price"];
  time_to_exp = contract.params_["Time To Expiry"];
  sigma = contract.params_["Implied Volatility"];
  rate = contract.params_["Risk Free Rate"];
  div = contract.params_["Dividend Rate"];

  const int phi = static_cast<int>(contract.payoff_type_);
  // Could be calendar days or trading days (in which case 252)
  const double days_per_year = 365.0f;

  double d1 = (std::log(S / K) +
               (rate - div + (std::pow(sigma, 2) / 2.0f)) * time_to_exp) /
              (sigma * std::sqrt(time_to_exp));
  double d2 = d1 - sigma * std::sqrt(time_to_exp);

  if (phi == 1)
    theta = ((-(S * norm_pdf(d1) * sigma) / (2 * std::sqrt(time_to_exp))) -
             ((rate * K * std::exp(-rate * time_to_exp)) * norm_cdf(d2)) +
             (div * S * std::exp(-div * time_to_exp) * norm_cdf(d1))) /
            days_per_year;
  else if (phi == -1)
    theta = ((-(S * norm_pdf(d1) * sigma) / (2 * std::sqrt(time_to_exp))) +
             ((rate * K * std::exp(-rate * time_to_exp)) * norm_cdf(-d2)) -
             (div * S * std::exp(-div * time_to_exp) * norm_cdf(-d1))) /
            days_per_year;
  return contract.qty_ * theta;
}

float Models::BlackScholes::Vega(OptionContract &contract) {
  double S, K, time_to_exp, sigma, rate, div;
  double vega = 0;
  S = contract.params_["Spot Price"];
  K = contract.params_["Strike Price"];
  time_to_exp = contract.params_["Time To Expiry"];
  sigma = contract.params_["Implied Volatility"];
  rate = contract.params_["Risk Free Rate"];
  div = contract.params_["Dividend Rate"];

  double d1 = (std::log(S / K) +
               (rate - div + (std::pow(sigma, 2) / 2.0f)) * time_to_exp) /
              (sigma * std::sqrt(time_to_exp));
  double d2 = d1 - sigma * std::sqrt(time_to_exp);

  // Same for puts and calls
  // Divide by 100 to get change in V / 1 percentage point change in Volatility

  vega =
      S * std::exp(-div * time_to_exp) * std::sqrt(time_to_exp) * norm_pdf(d1);
  return contract.qty_ * (vega / 100);
}

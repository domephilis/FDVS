#include "Strategy.hpp"

Strategy::Strategy::Strategy(
    std::string strategy_name,
    std::unordered_map<std::string, Models::OptionContract> portfolio,
    std::array<std::string, 2> to_vary,
    std::shared_ptr<Models::PricingPolicy> policy)
    : strategy_name_(strategy_name), portfolio_(portfolio), to_vary_(to_vary),
      policy_(policy) {}

Strategy::Strategy::Strategy(int qty, std::array<std::string, 2> to_vary,
                             std::string filename,
                             std::shared_ptr<Models::PricingPolicy> policy)
    : qty_(qty), to_vary_(to_vary), policy_(policy), strategy_name_(filename) {
  try {
    std::ifstream f("strats/" + filename + ".json");
    json data = json::parse(f);
    for (auto &[key, value] : data.items()) {
      json data{value};
      if (data["Model"] = "BSM") {
        Models::OptionContract temp;
        data.get_to(temp);
        std::string const &ckey = key;
        portfolio_.insert(std::make_pair(key, temp));
      }
    }
    UpdateProperties();
  } catch (...) {
    throw;
  }
}

void Strategy::Strategy::AddToPortfolio(
    std::pair<std::string, Models::OptionContract> new_leg) {
  portfolio_.insert(new_leg);
}
void Strategy::Strategy::RemoveFromPortfolio(std::string key) {
  try {
    portfolio_.erase(key);
  } catch (...) {
    throw;
  }
}
void Strategy::Strategy::UpdateCurrentPrices(
    std::initializer_list<std::pair<std::string, double>> new_prices) {
  for (auto &leg : new_prices) {
    portfolio_.at(leg.first).current_price_ = leg.second;
  }
}

float Strategy::Strategy::Price() {
  float sum = 0.0f;
  for (auto &[key, leg] : portfolio_) {
    leg.params_.at("Time To Expiry") -= time_since_start;
    sum += policy_->Price(leg);
    leg.params_.at("Time To Expiry") += time_since_start;
  }
  return sum;
}

float Strategy::Strategy::GetTotalPL() {
  // Later on I want each call of UpdateCurrentPrices to be time stamped
  // I assume here that UpdateCurrentPrices is called before GetTotalPL
  float sum = 0.0f;
  for (auto &[key, leg] : portfolio_) {
    sum += leg.PL();
  }
  return sum;
}
/*
float Strategy::Strategy::ModeledPL() {
  float sum = 0.0f;
  for (auto &[key, leg] : portfolio_) {
    leg.params_.at("Time To Expiry") -= time_since_start;
    sum += leg.ModeledPL();
    leg.params_.at("Time To Expiry") += time_since_start;
  }
  return sum;
} */

float Strategy::Strategy::Delta() {
  float sum = 0.0f;
  for (auto &[key, leg] : portfolio_) {
    leg.params_.at("Time To Expiry") -= time_since_start;
    sum += policy_->Delta(leg);
    leg.params_.at("Time To Expiry") += time_since_start;
  }
  return sum;
}

float Strategy::Strategy::Gamma() {
  float sum = 0.0f;
  for (auto &[key, leg] : portfolio_) {
    leg.params_.at("Time To Expiry") -= time_since_start;
    sum += policy_->Gamma(leg);
    leg.params_.at("Time To Expiry") += time_since_start;
  }
  return sum;
}

float Strategy::Strategy::Theta() {
  float sum = 0.0f;
  for (auto &[key, leg] : portfolio_) {
    leg.params_.at("Time To Expiry") -= time_since_start;
    sum += policy_->Theta(leg);
    leg.params_.at("Time To Expiry") += time_since_start;
  }
  return sum;
}

float Strategy::Strategy::Vega() {
  float sum = 0.0f;
  for (auto &[key, leg] : portfolio_) {
    // My intention is to preserve the time to expiry at the time
    // of purchase of the contract
    leg.params_.at("Time To Expiry") -= time_since_start;
    sum += policy_->Vega(leg);
    leg.params_.at("Time To Expiry") += time_since_start;
  }
  return sum;
}

void Strategy::Strategy::UpdateProperties() {
  // Even the to_vary_ parameters shall be set to a default that is used here
  SetDefaults();
  delta_ = Delta();
  gamma_ = Gamma();
  theta_ = Theta();
  vega_ = Vega();
  current_pl_ = GetTotalPL();
  breakeven_spot_price_ = GetBreakevenPoint();
}

// Will do this via a root-finding algorithm using PricingFunction
float Strategy::Strategy::GetBreakevenPoint() { return 0.0f; }

float Strategy::Strategy::ComputePrice(double x, double y) {
  SetParam(to_vary_[0], x);
  SetParam(to_vary_[1], y);
  return Price();
}
/*
float Strategy::Strategy::ComputeModeledPL(double x, double y) {
  SetParam(to_vary_[0], x);
  SetParam(to_vary_[1], y);
  return ModeledPL();
} */
float Strategy::Strategy::ComputeDelta(double x, double y) {
  SetParam(to_vary_[0], x);
  SetParam(to_vary_[1], y);
  return Delta();
}
float Strategy::Strategy::ComputeGamma(double x, double y) {
  SetParam(to_vary_[0], x);
  SetParam(to_vary_[1], y);
  return Gamma();
}
float Strategy::Strategy::ComputeTheta(double x, double y) {
  SetParam(to_vary_[0], x);
  SetParam(to_vary_[1], y);
  return Theta();
}
float Strategy::Strategy::ComputeVega(double x, double y) {
  SetParam(to_vary_[0], x);
  SetParam(to_vary_[1], y);
  return Vega();
}

std::function<float(double, double)>
Strategy::Strategy::GetFunction(std::string name) {
  switch (Models::GetFuncID(name)) {
  case Models::ComputeFuncs::Price:
    return std::bind(&Strategy::Strategy::ComputePrice, this,
                     std::placeholders::_1, std::placeholders::_2);
    break;
  /*
  case Models::ComputeFuncs::PL:
    return std::bind(&Strategy::Strategy::ComputeModeledPL, this,
                     std::placeholders::_1, std::placeholders::_2);
    break; */
  case Models::ComputeFuncs::Delta:
    return std::bind(&Strategy::Strategy::ComputeDelta, this,
                     std::placeholders::_1, std::placeholders::_2);
    break;
  case Models::ComputeFuncs::Gamma:
    return std::bind(&Strategy::Strategy::ComputeGamma, this,
                     std::placeholders::_1, std::placeholders::_2);
    break;
  case Models::ComputeFuncs::Theta:
    return std::bind(&Strategy::Strategy::ComputeTheta, this,
                     std::placeholders::_1, std::placeholders::_2);
    break;
  case Models::ComputeFuncs::Vega:
    return std::bind(&Strategy::Strategy::ComputeVega, this,
                     std::placeholders::_1, std::placeholders::_2);
    break;
  default:
    std::cerr << "Did not pass in the right name." << std::endl;
    return std::function<float(double, double)>();
    break;
  }
}

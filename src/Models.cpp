#include "Models.hpp"

void Models::to_json(json &j, const Models::OptionContract &op) {
  j = json{{"Qty", static_cast<int>(op.qty_)},
           {"Payoff Type", static_cast<int>(op.payoff_type_)}};
  j.push_back({"params", json(op.params_)});
  j.push_back({"to_vary", json(op.to_vary_)});
}
void Models::from_json(const json &j, Models::OptionContract &op) {
  op.qty_ = j["Qty"];
  op.payoff_type_ = j["Payoff Type"];
  op.params_ = j["params"];
  op.to_vary_ = j["to_vary"];
}

Models::ComputeFuncs Models::GetFuncID(const std::string &func_name) {
  try {
    return funcs.at(func_name);
  } catch (...) {
    return ComputeFuncs::None;
  }
}

Models::OptionContract::OptionContract(int qty, PayoffType payoff_type,
                                       std::map<std::string, double> params,
                                       std::array<std::string, 2> to_vary)
    : qty_(qty), payoff_type_(payoff_type), params_(params), to_vary_(to_vary) {

  // Later on I want to check preconditions, but I just need an MVP for now
  //
  for (auto it = to_vary_.begin(); it < to_vary_.end(); it++)
    defaults.insert(std::make_pair(*it, params_.at(*it)));
}
/*
double Models::OptionContract::ModeledPL() {
  return qty_ * (policy_->Price(this) - params_.at("Purchase Price"));
}

std::function<float(double, double)>
Models::OptionContract::GetFunction(std::string name) {
  switch (GetFuncID(name)) {
  case ComputeFuncs::Price:
    return std::bind(&Models::OptionContract::ComputePrice, this,
                     std::placeholders::_1, std::placeholders::_2);
    break;
  case ComputeFuncs::PL:
    return std::bind(&Models::OptionContract::ComputeModeledPL, this,
                     std::placeholders::_1, std::placeholders::_2);
    break;
  case ComputeFuncs::Delta:
    return std::bind(&Models::OptionContract::ComputeDelta, this,
                     std::placeholders::_1, std::placeholders::_2);
    break;
  case ComputeFuncs::Gamma:
    return std::bind(&Models::OptionContract::ComputeGamma, this,
                     std::placeholders::_1, std::placeholders::_2);
    break;
  case ComputeFuncs::Theta:
    return std::bind(&Models::OptionContract::ComputeTheta, this,
                     std::placeholders::_1, std::placeholders::_2);
    break;
  case ComputeFuncs::Vega:
    return std::bind(&Models::OptionContract::ComputeVega, this,
                     std::placeholders::_1, std::placeholders::_2);
    break;
  default:
    std::cerr << "Did not pass in the right name." << std::endl;
    return std::function<float(double, double)>();
    break;
  }
}

float Models::OptionContract::ComputePrice(double x, double y) {
  SetParam(to_vary_[0], x);
  SetParam(to_vary_[1], y);
  return policy_->Price(this);
}
float Models::OptionContract::ComputeModeledPL(double x, double y) {
  SetParam(to_vary_[0], x);
  SetParam(to_vary_[1], y);
  return ModeledPL();
}
float Models::OptionContract::ComputeDelta(double x, double y) {
  SetParam(to_vary_[0], x);
  SetParam(to_vary_[1], y);
  return policy_->Delta(this);
}
float Models::OptionContract::ComputeGamma(double x, double y) {
  SetParam(to_vary_[0], x);
  SetParam(to_vary_[1], y);
  return policy_->Gamma(this);
}
float Models::OptionContract::ComputeTheta(double x, double y) {
  SetParam(to_vary_[0], x);
  SetParam(to_vary_[1], y);
  return policy_->Theta(this);
}
float Models::OptionContract::ComputeVega(double x, double y) {
  SetParam(to_vary_[0], x);
  SetParam(to_vary_[1], y);
  return policy_->Vega(this);
} */

#include <CL/sycl.hpp>
#include <iostream>
#include <random>

//namespace sycl = cl::sycl;
namespace my_sycl = cl::sycl;

int main() {
    constexpr size_t num_simulations = 10000;
    constexpr size_t num_years = 30;
    constexpr float initial_portfolio_value = 1000000.0f;
    constexpr float desired_withdrawal_rate = 0.04f;
    constexpr float expected_return_rate = 0.06f;
    constexpr float stock_volatility = 0.15f;
    constexpr float bond_volatility = 0.05f;
    constexpr float stock_weight = 0.7f;
    constexpr float bond_weight = 0.3f;
    constexpr float inflation_rate = 0.02f;

    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<float> stock_dist{expected_return_rate, stock_volatility};
    std::normal_distribution<float> bond_dist{expected_return_rate, bond_volatility};

    std::vector<float> portfolio_values(num_simulations, initial_portfolio_value);
    for (size_t year = 0; year < num_years; ++year) {
        for (size_t i = 0; i < num_simulations; ++i) {
            const float stock_return = stock_dist(rng);
            const float bond_return = bond_dist(rng);
            const float portfolio_return = stock_weight * stock_return + bond_weight * bond_return;
            portfolio_values[i] *= 1 + portfolio_return;
            portfolio_values[i] -= desired_withdrawal_rate * portfolio_values[i];
            portfolio_values[i] /= (1 + inflation_rate);
        }
    }

    const float success_threshold = 1.0f; // multiple of initial portfolio value
    const size_t num_successful_simulations = std::count_if(
        portfolio_values.begin(), portfolio_values.end(),
        [success_threshold](const float value) { return value >= success_threshold * initial_portfolio_value; });
    const float success_rate = static_cast<float>(num_successful_simulations) / num_simulations;
    std::cout << "Success rate: " << success_rate << std::endl;

    return 0;
}

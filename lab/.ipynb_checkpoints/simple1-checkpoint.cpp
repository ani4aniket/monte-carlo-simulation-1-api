#include <CL/sycl.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>

using namespace cl::sycl;
constexpr size_t MAX_STRING_SIZE = 64;

class StockData {
public:
    std::array<char, MAX_STRING_SIZE> date;
    std::array<char, MAX_STRING_SIZE> stock;
    double open = 0.0, high = 0.0, low = 0.0, close = 0.0;
    int volume = 0;
};



std::vector<StockData> parse_stock_data(const std::vector<std::string>& raw_data) {
    std::vector<StockData> stock_data;
    for (const auto& line : raw_data) {
        std::stringstream ss(line);
        StockData data;
        std::string date;
        getline(ss, date, '\t');
        std::copy(date.begin(), date.end(), data.date.begin());
        data.date[date.size()] = '\0'; // Null-terminate the string
        ss >> data.open >> data.high >> data.low >> data.close >> data.volume;
        std::string stock;
        getline(ss, stock);
        stock = stock.substr(1);
        std::copy(stock.begin(), stock.end(), data.stock.begin());
        data.stock[stock.size()] = '\0'; // Null-terminate the string
        stock_data.push_back(data);
    }
    return stock_data;
}

  

double black_scholes_call(const StockData& stock_data, double K, double r, double div_yield, double sigma, double T) {
    double S = stock_data.close;
    double d1 = (log(S / K) + (r - div_yield + 0.5 * sigma * sigma) * T) / (sigma * sqrt(T));
    double d2 = d1 - sigma * sqrt(T);
    double Nd1 = 0.5 * erfc(-d1 / sqrt(2.0));
    double Nd2 = 0.5 * erfc(-d2 / sqrt(2.0));
    return S * exp(-div_yield * T) * Nd1 - K * exp(-r * T) * Nd2;
}

int main() {
    double K = 100.0;
    double r = 0.05;
    double div_yield = 0.02;
    double sigma = 0.25;
    double T = 1.0;
    int N = 10000;
    int M = 50;
    int option_type = 1;

   std::vector<std::string> raw_data = {
    "22-03-2017\t63.417\t64.426\t63.417\t64.316\t20909363\tMSFT",
    "1/3/2016\t107.83\t109.82\t107.6\t109.82\t23852557\tFB",
    "13-06-2016\t115\t115.48\t113.31\t113.95\t30974633\tFB",
    "15-08-1995\t4.6709\t4.6709\t4.5536\t4.6291\t84087209\tMSFT",
    "26-08-1987\t0.30191\t0.31032\t0.30191\t0.30191\t151655535\tMSFT",
    "25-08-2010\t19.16\t19.98\t18.56\t19.9\t503289\tTSLA",
    "15-01-1987\t0.1593\t0.16762\t0.1593\t0.16762\t155521562\tMSFT",
    "4/8/2017\t71.986\t72.653\t71.857\t72.295\t22698650\tMSFT",
    "15-08-2017\t171.49\t171.5\t170.01\t171\t8714394\tFB",
    "25-01-2007\t26.054\t26.399\t25.535\t25.535\t115326006\tMSFT",
    "20-10-2017\t352.69\t354.55\t344.34\t345.1\t4926667\tTSLA",
    "6/4/2011\t21.794\t22.064\t21.686\t21.93\t78193256\tMSFT",
    "21-11-2007\t28.856\t29.124\t28.662\t28.705\t106742755\tMSFT",
    "17-01-1996\t3.9834\t4.1174\t3.9834\t3.9917\t190162722\tMSFT",
    "6/6/1996\t5.7274\t5.7864\t5.6352\t5.6352\t117139844\tMSFT",
    "11/10/1999\t35.598\t35.741\t35.412\t35.483\t26506913\tMSFT",
    "24-06-2002\t19.598\t20.554\t19.505\t20.377\t138603167\tMSFT",
    "2/1/2009\t16.386\t17.106\t16.241\t17.049\t59723535\tMSFT",
    "6/2/2013\t28.74\t29.29\t28.66\t29.05\t38343606\tFB"
};


    std::vector<StockData> stock_data = parse_stock_data(raw_data);

    for (const auto& data : stock_data) {
        queue q{ cpu_selector{} };

        std::vector<double> rand_nums(N * M);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> dis(0, 1);

        for (int i = 0; i < N * M; ++i) {
            rand_nums[i] = dis(gen);
        }

        buffer<double> rand_nums_buf{ rand_nums };
        buffer<double> result_buf{ range{1} };

        double dt = T / M;

        q.submit([&](handler& h) {
            auto rand_nums_acc = rand_nums_buf.get_access<access::mode::read>(h);
            auto result_acc = result_buf.get_access<access::mode::write>(h);

            h.single_task<class MonteCarloKernel>([=]() {
                double sum = 0.0;
                for (int i = 0; i < N; ++i)
                {
                    double S = data.close;
                    for (int j = 0; j < M; ++j) {
                        double rand_num = rand_nums_acc[i * M + j];
                        S *= exp((r - div_yield - 0.5 * sigma * sigma) * dt + sigma * sqrt(dt) * rand_num);
                    }

                    double payoff = 0.0;
                    if (option_type == 1) {
                        payoff = std::max(S - K, 0.0);
                    }
                    sum += payoff;
                }
                double result = sum / N;
                result_acc[0] = result;
                });
            });

        double option_price_cpu = exp(-r * T) * result_buf.get_access<access::mode::read>()[0];
        std::cout << "Date: " << data.date.data() << " Stock: " << data.stock.data() << std::endl;

        double analytical_result = black_scholes_call(data, K, r, div_yield, sigma, T);
        std::cout << "Date: " << data.date.data() << " Stock: " << data.stock.data() << std::endl;


        double error = abs(option_price_cpu - analytical_result) / analytical_result * 100.0;
        std::cout << "Relative error: " << error << "%\n";

        if (error < 1.0) {
            std::cout << "The result is good, with a relative error below 1%." << std::endl;
        }
        else {
            std::cout << "The result has a relative error above 1%. Consider increasing the number of simulations or steps for a more accurate result." << std::endl;
        }
    }

    return 0;
}



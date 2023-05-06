# Problem statement 

## Monte Carlo Simulation with user input

Performing 2 simulations
The Black-Scholes formula is a mathematical model that provides an analytical solution for pricing European-style options, which are financial contracts that give the holder the right, but not the obligation, to buy (call option) or sell (put option) an underlying asset at a predetermined price (strike price) on or before a specific date (expiration date).

The formula assumes that the price of the underlying asset follows a geometric Brownian motion, which means that the rate of return on the asset is normally distributed with a constant expected return and volatility over time.

The formula calculates the theoretical value of the option by taking into account several factors, including:

Current price of the underlying asset (S)
Strike price of the option (K)
Time to expiration of the option (T)
Risk-free interest rate (r)
Dividend yield of the underlying asset (q)
Volatility of the underlying asset (σ)

The formula is:

scss
Copy code
`C = S * N(d1) - K * e^(-rT) * N(d2)`

`P = K * e^(-rT) * N(-d2) - S * N(-d1)`

where:

C is the call option price

P is the put option price

N(d1) and N(d2) are the cumulative normal distribution functions of the standardized variables d1 and d2, respectively, which are defined as follows:

`d1 = [ln(S/K) + (r - q + σ^2/2)T] / (σ * sqrt(T))`

`d2 = d1 - σ * sqrt(T)`

The Black-Scholes formula assumes that the underlying asset follows a lognormal distribution, which means that small changes in the price of the underlying asset have a proportional effect on the option price. However, the model has some limitations, such as assuming constant volatility and that stock prices follow a random walk, which may not always hold in real-world financial markets.

## Coding approach

In the Monte Carlo simulation approach, a large number of random simulations are generated to approximate the expected value of the option payoff. The program generates random numbers using the normal distribution, and uses these numbers to simulate the future stock prices based on the Black-Scholes model. For each simulation, the option payoff is computed, and the average of all payoffs is used to estimate the option price.

The Black-Scholes model is a mathematical model that uses partial differential equations to describe the price movement of financial assets, such as stocks. The model assumes that stock prices follow a geometric Brownian motion, and that the price of an option can be calculated based on the underlying stock price, the strike price, the risk-free interest rate, the dividend yield, the volatility, and the time to maturity.

The Black-Scholes formula provides an analytical solution for pricing European options, but for more complex options, such as American options or options with exotic features, Monte Carlo simulation is often used as a numerical method to estimate the option price.

## Simulation 1

This is a C++ program that uses the SYCL (SYCL stands for "Standard for C++ Parallel Programming") framework to perform a Monte Carlo simulation for pricing options using the Black-Scholes model.

The program starts by prompting the user to enter parameters such as the initial stock price, the strike price, the risk-free interest rate, the dividend yield, the volatility, the time to maturity, the number of simulations, the number of steps, and the option type (call or put).

After reading the user inputs, the program creates a SYCL queue object to specify the device where the computation will be performed (in this case, the CPU). It also creates a buffer object to hold a vector of random numbers generated using the normal distribution.

Next, the program defines a SYCL kernel function, MonteCarloKernel, which performs the Monte Carlo simulation. The kernel function is submitted to the SYCL queue, which executes it on the specified device.

The kernel function reads the vector of random numbers from the buffer and uses them to generate stock prices using the Black-Scholes model. The payoff of the option is calculated based on the option type (call or put). The sum of the payoffs is accumulated over all simulations, and the average is computed. The result is written to a buffer.

After the kernel function completes, the result buffer is read back to the CPU, and the final option price is computed by multiplying the result by the discount factor (exp(-r*T)). The program then calculates the analytical result using the Black-Scholes formula and computes the relative error between the two results. If the error is less than 1%, the program prints a message indicating that the result is good; otherwise, it suggests increasing the number of simulations or steps for a more accurate result.

Overall, this program demonstrates how SYCL can be used to write parallel C++ code that can run on a variety of devices, such as CPUs and GPUs, and how it can be used to accelerate complex computations such as Monte Carlo simulations.

## Simulation 2 with Dataset mentioned by Techgig



The above code is using SYCL standards to perform a Monte Carlo simulation to price European call options using the Black-Scholes model for multiple stocks. It takes in stock data for different dates and performs a Monte Carlo simulation to calculate the option price for each stock. The StockData class stores information about the stock for a particular date, including the date, stock name, open price, high price, low price, closing price, and trading volume.

The parse_stock_data function takes in raw stock data as a vector of strings, where each string represents data for a particular date, and parses it to create a vector of StockData objects. The data is separated by tabs, and each field is stored in the appropriate variable in the StockData object.

The black_scholes_call function takes in a StockData object, strike price, risk-free interest rate, dividend yield, volatility, and time to maturity, and uses the Black-Scholes formula to calculate the price of a European call option for the given stock.

In the main function, a loop is used to iterate over each StockData object in the stock_data vector. For each stock, a SYCL queue is created using the CPU selector, and a Monte Carlo simulation is performed to calculate the option price. The simulation is performed using the same parameters for all stocks, including the strike price, risk-free interest rate, dividend yield, volatility, time to maturity, number of simulations, and number of steps.

Inside the loop, a normal distribution is used to generate random numbers, which are stored in a vector and then transferred to a SYCL buffer. A kernel is then executed on the SYCL device, which performs the Monte Carlo simulation to calculate the option price. The result is stored in a SYCL buffer and then transferred back to the host. The relative error between the Monte Carlo simulation result and the analytical result calculated using the Black-Scholes formula is calculated and printed to the console along with the date and stock name. If the relative error is less than 1%, the result is considered good, and otherwise, the number of simulations or steps is suggested to be increased for a more accurate result.






## Funtion definition 
`class StockData:`

This is a class that holds the data for a single stock. It has the following members:
`date: a std::array` of char that holds the date of the stock.
`stock: a std::array` of char that holds the stock symbol.
open, high, low, close: double values that hold the stock prices for that day.
`volume:` an int value that holds the trading volume for that day.
`parse_stock_data:`

This function takes in a vector of strings, where each string is a line of raw stock data in a specific format, and returns a vector of StockData objects that represent the parsed data.
It loops through each line of raw data, and uses a stringstream to parse the values into a StockData object. The date and stock symbol are copied into the date and stock members of the StockData object using std::copy, and the other values are read into the other members directly.
The function then returns the vector of StockData objects.
`black_scholes_call:`

This function takes in a StockData object, strike price (K), risk-free interest rate (r), dividend yield (div_yield), volatility (sigma), and time to maturity (T), and calculates the price of a call option using the Black-Scholes formula.
It first gets the closing stock price from the StockData object, and then calculates the d1, d2, Nd1, and Nd2 values as per the Black-Scholes formula.
Finally, it calculates and returns the call option price.
`main:`

This function is the entry point for the program.
It sets up the input parameters for the Black-Scholes formula (K, r, div_yield, sigma, T, and option_type), as well as the number of simulations (N) and steps (M).
It also defines a vector of raw stock data, and calls parse_stock_data to get a vector of StockData objects.
For each StockData object, it creates a SYCL queue using the cpu_selector device selector, generates N * M random numbers using a normal distribution with mean 0 and standard deviation 1, and stores them in a std::vector.
It then creates SYCL buffers for the random numbers and the result of the Monte Carlo simulation.
It calculates the time step (dt), and submits a SYCL kernel that performs the Monte Carlo simulation for the current stock using the rand_nums_buf and result_buf buffers.
It then gets the result from the result_buf, calculates the analytical result using black_scholes_call, and prints out the date, stock symbol, and relative error between the two results.

























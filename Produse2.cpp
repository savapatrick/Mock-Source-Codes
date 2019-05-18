/*Patrick Catalin Alexandru Sava
The University of Manchester
O (T * N * log (VALMAX) * log(log(VALMAX)))
Expected : Accepted*/
 
#include <bits/stdc++.h>
 
using namespace std;
 
vector <int> sieve (int limit)
{
    // sieve to optimize the computation of prime numbers
    vector <bool> isNotPrime (static_cast<unsigned int>(limit + 1), false);
    vector <int> primes;
    for (int currentNumber = 2; currentNumber <= limit; ++ currentNumber)
    {
        if (!isNotPrime [currentNumber])
        {
            for (int multiple = currentNumber + currentNumber; multiple <= limit; multiple += currentNumber)
            {
                isNotPrime [multiple] = true;
            }
            primes.push_back(currentNumber);
        }
    }
    return primes;
}// sieve
 
vector < pair <int, int> > factorize (int number, vector <int> &primes)
{
    // get the factors and exponents for a certain number
    vector < pair <int, int> > factorsAndExponents;
 
    for (auto &primeNumber : primes)
    {
        if (primeNumber * primeNumber > number)
        {
            break;
        }
        int exponent = 0;
        while (number % primeNumber == 0)
        {
            number /= primeNumber;
            exponent += 1;
        }
        if (exponent)
        {
            factorsAndExponents.emplace_back(primeNumber, exponent);
        }
    }
    if (number != 1)
    {
        // take into consideration the case in which the number is
        // different by 1
        // hence the number has a prime factor bigger than its
        // square root
        factorsAndExponents.emplace_back(number, 1);
    }
    return factorsAndExponents;
} // factorize
 
 
bool canChoose (int fixedExponent, vector <int> &exponents)
{
    // check for a certain exponent for a fixed factor
    // if it is suitable
    int remainder = fixedExponent;
    for (int currentExponent : exponents)
    {
        remainder = currentExponent - remainder;
        if (remainder < 0)
        {
            return false;
        }
    }
    return true;
} // canChoose
 
vector <int> buildSolution (vector <int> &products, int &firstElement)
{
    // build the solution
    vector <int> solution;
    int lastNumber = firstElement;
    solution.push_back(firstElement);
    for (auto currentNumber : products)
    {
        if (currentNumber % lastNumber)
        {
            // take care to this tricky case
            // no solution exists
            firstElement = 0;
            return solution;
        }
        currentNumber /= lastNumber;
        solution.push_back(currentNumber);
        lastNumber = currentNumber;
    }
    return solution;
}// buildSolution
 
 
int main() {
    // file streams
    ifstream input ("produse2.in");
    ofstream output ("produse2.out");
 
    // assertions
    assert(input.is_open());
    assert(output.is_open());
 
    // constants
    const auto MaxNumberOfTests = static_cast<const int>(7e3);
    const auto MaxNumberOfElements = static_cast<const int>(3e6);
    const auto ValMax = static_cast<const int>(3e6);
 
    // for assertion
    auto isBetween = [](const int inf, const int number, const int sup) -> bool {
        return inf <= number and number <= sup;
    };
 
    int numberOfTests;
    assert(input >> numberOfTests);
    assert(isBetween(1, numberOfTests, MaxNumberOfTests));
    auto primes = sieve (ValMax);
    for (int currentTest = 1; currentTest <= numberOfTests; ++ currentTest)
    {
        // for each test
        int numberOfProducts;
        assert(input >> numberOfProducts); // number of products + 1 (number of elements)
        assert(isBetween (2, numberOfProducts, MaxNumberOfElements));
        numberOfProducts -= 1;
        vector <int> products (static_cast<unsigned int>(numberOfProducts));
        for (int currentIndex = 0; currentIndex < numberOfProducts; ++ currentIndex)
        {
            // read each product
            assert(input >> products[currentIndex]);
            assert(isBetween(1, products [currentIndex], ValMax));
        }
        int answer = 1;
        int numberOfWays = 1;
        // get the factors and exponents
        auto factorsAndExponents = factorize (products[0], primes);
        for (auto factorAndExponent : factorsAndExponents)
        {
            // for each factor
            int factor = factorAndExponent.first; // fixed prime factor
            int exponent = factorAndExponent.second;
            vector <int> exponentForCurrentFactor (static_cast<unsigned int>(numberOfProducts));
            vector <int> powers;
            powers.push_back(1);
            for (int currentPower = 1; 1LL * powers[currentPower - 1] * factor <= ValMax; ++ currentPower)
                powers.push_back(powers[currentPower - 1] * factor);
            for (int currentIndex = 0; currentIndex < numberOfProducts; ++ currentIndex)
            {
                int copy = products [currentIndex];
                int dividingTimes = 0;
                for (int currentBit = 4; currentBit >= 0; -- currentBit)
                    if ((1 << currentBit) < (int)powers.size() and copy % powers[(1 << currentBit)] == 0)
                        copy /= powers[(1 << currentBit)], dividingTimes |= (1 << currentBit);
                // precompute the biggest exponent for the fixed prime factor
                exponentForCurrentFactor [currentIndex] = dividingTimes;
            }
            // we will iterate how many exponents respect might be choosed
            int numberOfGoodExponents = 0;
            int bestExponent;
            for (int iteratedExponent = 0; iteratedExponent <= exponent; ++ iteratedExponent)
            {
                if (canChoose(iteratedExponent, exponentForCurrentFactor))
                {
                    // increase the number of good exponents
                    numberOfGoodExponents += 1;
                    // keep the last exponent for build the solution
                    bestExponent = iteratedExponent;
                }
            }
            if (numberOfGoodExponents == 0)
            {
                // no solution exists
                answer = 0;
                break;
            }
            else
            {
                // multiply the number of ways with the number of
                // good exponents for this factor
                numberOfWays *= numberOfGoodExponents;
                // compute the first number
                while (bestExponent)
                {
                    // computing A[1]...
                    answer *= factor;
                    bestExponent -= 1;
                }
            }
        }
 
        // once we have the first number we may build a valid solution
        if (answer == 0)
        {
            // no good exponent found for at least one of
            // the factors of B[1]
            output << 0 << '\n';
            continue;
        }
        auto solution = buildSolution(products, answer);
        if (answer)
        {
            output << numberOfWays << '\n';
            for (int element : solution)
            {
                output << element << ' ';
            }
            output << '\n';
        }
        else
        {
            // the solution crashed for the factors
            // whose exponents are 0 in B[1]
            output << 0 << '\n';
        }
 
    }// for
    return 0;
}// main

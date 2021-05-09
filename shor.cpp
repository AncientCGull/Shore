#include "shor.h"
using namespace qpp;

shor::shor()
{
}

void shor::calculate() // number to factor
{
    bigint N = 32;
    bigint a = rand(static_cast<bigint>(3), N - 1); // random co-prime with N
    while (gcd(a, N) != 1)
    {
        a = rand(static_cast<bigint>(3), N - 1);
    }
    // qubits required for half of the circuit, in total we need 2n qubits
    // if you know the order 'r' of 'a', then you can take the smallest 'n' s.t.
    // 2^n >= 2 * r^2, i.e., n = ceil(log2(2 * r^2))
    idx n = static_cast<idx>(std::ceil(2 * std::log2(N)));
    idx D = static_cast<idx>(std::llround(std::pow(2, n))); // dimension 2^n

    emit write(">> Factoring N = " + QString::number(N) + " with coprime a = " + QString::number(a) + '\n' +
                         ">> n = " + QString::number(n) + ", D = 2^n = " + QString::number(D) +
                         " and 2^(2n) = " + QString::number(D * D) + '\n');

    // vector with labels of the first half of the qubits
    std::vector<idx> first_subsys(n);
    std::iota(std::begin(first_subsys), std::end(first_subsys), 0);

    // vector with labels of the second half of the qubits
    std::vector<idx> second_subsys(n);
    std::iota(std::begin(second_subsys), std::end(second_subsys), n);

    // QUANTUM STAGE
    // prepare the initial state |0>^\otimes n \otimes |0...01>
    ket psi = kron(st.zero(2 * n - 1), 1_ket);

    // apply Hadamards H^\otimes n on first half of the qubits
    for (idx i = 0; i < n; ++i)
    {
        psi = apply(psi, gt.H, {i});
    }

    // perform the modular exponentiation as a sequence of
    // modular multiplications
    for (idx i = 0; i < n; ++i)
    {
        // compute 2^(n-i-1) mod N
        idx j = static_cast<idx>(std::llround(std::pow(2, n - i - 1)));
        // compute the a^(2^(n-i-1)) mod N
        idx aj = modpow(a, j, N);
        // apply the controlled modular multiplication
        psi = applyCTRL(psi, gt.MODMUL(aj, N, n), {i}, second_subsys); // long part !!
    }

    // apply inverse QFT on first half of the qubits
    psi = applyTFQ(psi, first_subsys);
    // END QUANTUM STAGE

    // FIRST MEASUREMENT STAGE
    auto measured1 = measure_seq(psi, first_subsys); // measure first n qubits
    std::vector<idx> vect_results1 = std::get<RES>(measured1); // results
    double prob1 = std::get<PROB>(measured1); // probability of the result
    idx n1 = multiidx2n(vect_results1, std::vector<idx>(n, 2)); // binary to int
    double x1 = static_cast<double>(n1) / D; // multiple of 1/r

    QString vec1_str = "[";
    for (auto val : vect_results1)
    {
        vec1_str += QString::number(val) + " ";
    }
    vec1_str += "]";

    emit write(">> First measurement:  " + vec1_str + " " +
                         "i.e., j = " + QString::number(n1) + " with probability " + QString::number(prob1) + '\n');

    bool failed = true;
    idx r1 = 0, c1 = 0;
    for (auto&& elem : convergents(x1, 10))
    {
        std::tie(c1, r1) = elem;
        double c1r1 = static_cast<double>(c1) / r1;
        if (abs(x1 - c1r1) < 1. / std::pow(2, (n - 1.) / 2.))
        {
            failed = false;
            break;
        }
    }
    if (failed)
    {
        emit write(">> Factoring failed at stage 1, please try again!\n");
        emit done();
        //std::exit(EXIT_FAILURE);
    }
    // END FIRST MEASUREMENT STAGE

    // SECOND MEASUREMENT STAGE
    auto measured2 = measure_seq(psi, first_subsys); // measure first n qubits
    std::vector<idx> vect_results2 = std::get<RES>(measured2); // results
    double prob2 = std::get<PROB>(measured2); // probability of the result
    idx n2 = multiidx2n(vect_results2, std::vector<idx>(n, 2)); // binary to int
    double x2 = static_cast<double>(n2) / D; // multiple of 1/r

    QString vec2_str = "[";
    for (auto val : vect_results2)
    {
        vec2_str += QString::number(val) + " ";
    }
    vec2_str += "]";

    emit write(">> Second measurement: " + vec2_str + " " +
                         "i.e., j = " + QString::number(n2) +
                         " with probability " + QString::number(prob2) + '\n');

    failed = true;
    idx r2 = 0, c2 = 0;
    for (auto&& elem : convergents(x2, 10))
    {
        std::tie(c2, r2) = elem;
        double c2r2 = static_cast<double>(c2) / r2;
        if (abs(x2 - c2r2) < 1. / std::pow(2, (n - 1.) / 2.))
        {
            failed = false;
            break;
        }
    }
    if (failed)
    {
        emit write(">> Factoring failed at stage 2, please try again!\n");
        emit done();
        //std::exit(EXIT_FAILURE);
    }
    // END SECOND MEASUREMENT STAGE

    // THIRD POST-PROCESSING STAGE
    idx r = lcm(r1, r2); // candidate order of a mod N
    emit write(">> r = " + QString::number(r) + ", a^r mod N = " +
                         QString::number(modpow(a, r, N)) +'\n');
    if (r % 2 == 0 && modpow(a, r / 2, N) != static_cast<bigint>(N - 1))
    {
        emit write(">> Possible factors: " +
                             QString::number(gcd(modpow(a, r / 2, N) - 1, N)) + " " +
                             QString::number(gcd(modpow(a, r / 2, N) + 1, N)) + '\n');
    }
    else
    {
        emit write(">> Factoring failed at stage 3, please try again!\n");
        emit done();
        //std::exit(EXIT_FAILURE);
    }
    // END THIRD POST-PROCESSING STAGE

    emit done();
}

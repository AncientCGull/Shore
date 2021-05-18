#include "shor.h"
using namespace qpp;

shor::shor()
{
}

bigint shor::check()
{
    //bigint a = rand(1, N/2);
    bigint b = rand(2, std::ceil(std::log2(N+1)));
    //emit write("a = " + QString::number(a) + " b = " + QString::number(b));
    double y = std::log2(N);
    double x = y/b;
    bigint u1 = std::floor(std::pow(2, x)), u2 = std::ceil(std::pow(2, x));

    if (std::pow(u1, b) == N and u1 > 0 and u1 < N/2)
    {
        return u1;
    }
    if (std::pow(u2, b) == N and u2 > 0 and u2 < N/2)
    {
        return u2;
    }

    return 0;
}

void shor::calculate() // number to factor
{
    static QString msg_step2 = "";
    static QString msg = "";
    if (N % 2 == 0 and N > 29)
    {
        N /= 2;
        msg += "Исходное N = " + QString::number(N*2) + " четное, поэтому мы можем взять число " +
                QString::number(N) + " как N и записать 2 в список множителей.\n";
//        emit write(0, "Исходное N = " + QString::number(N*2) + " четное, поэтому мы можем взять число " +
//                   QString::number(N) + " как N и записать 2 в список множителей.");
        factors += "2 ";
        return calculate();
    }

    if (N % 2 == 0)
    {
        msg += "N = " + QString::number(N) + " четное, но N/2 < 15, так что продолжаем алгоритм.";
    }
    else
    {
        msg += "N = " + QString::number(N) + " нечетное, так что продолжаем алгоритм.";
    }
//    emit write(msg);
//    emit write (0, "Исходное N = " + QString::number(N) + " нечетное, так что продолжаем алогитм.");

    bigint x = check();
    if (x != 0)
    {
        N /= x;
        if (N > 14)
        {
            factors += QString::number(x) + " ";
            msg_step2 += "Найден множитель " + QString::number(x) + "\n";
            return calculate();
        }
        else
        {
//            msg_step2 += "Возможный множитель: " + QString::number(x) + "\n";
            emit write (msg);
            emit write("Возможный множитель: " + QString::number(x));
            emit done();
            msg = "";
            return;
        }
    }
    else
    {
        msg_step2 += "Равество N = a^b не выполнилось";
    }

    emit write(msg);
    emit write(msg_step2);
    msg = "";

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

    emit write("Факторизуем число N = " + QString::number(N) + "; в качестве взиаимно простого a возьмем "+
               QString::number(a) + '\n');

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

    msg += ">> First measurement:  " + vec1_str + " " +
            "i.e., j = " + QString::number(n1) + " with probability " + QString::number(prob1) + '\n';
//    emit write(3, ">> First measurement:  " + vec1_str + " " +
//                         "i.e., j = " + QString::number(n1) + " with probability " + QString::number(prob1) + '\n');

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
        msg += ">> Factoring failed at stage 1, please try again!\n";
        emit write(msg);
        emit done();
        return;
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

    msg += ">> Second measurement: " + vec2_str + " " +
            "i.e., j = " + QString::number(n2) +
            " with probability " + QString::number(prob2) + '\n';
//    emit write(3, ">> Second measurement: " + vec2_str + " " +
//                         "i.e., j = " + QString::number(n2) +
//                         " with probability " + QString::number(prob2) + '\n');

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
        msg += ">> Factoring failed at stage 2, please try again!\n";
        emit write(msg);
        emit done();
        return;
        //std::exit(EXIT_FAILURE);
    }
    // END SECOND MEASUREMENT STAGE

    emit write (msg);
    msg = "";

    // THIRD POST-PROCESSING STAGE
    idx r = lcm(r1, r2); // candidate order of a mod N
    msg += ">> r = " + QString::number(r) + ", a^r mod N = " +
            QString::number(modpow(a, r, N)) +'\n';
//    emit write(4, ">> r = " + QString::number(r) + ", a^r mod N = " +
//                         QString::number(modpow(a, r, N)) +'\n');
    if (r % 2 == 0 && modpow(a, r / 2, N) != static_cast<bigint>(N - 1))
    {
        factors += QString::number(gcd(modpow(a, r / 2, N) - 1, N)) + " ";
        factors += QString::number(gcd(modpow(a, r / 2, N) + 1, N));
        factors.remove("1");
        msg += ">> Possible factors: " + factors + '\n';
//        emit write(4, ">> Possible factors: " + factors + '\n');
    }
    else
    {
        msg += ">> Factoring failed at stage 3, please try again!\n";
//        emit write(4, ">> Factoring failed at stage 3, please try again!\n");
//        emit done();
        //std::exit(EXIT_FAILURE);
    }
    // END THIRD POST-PROCESSING STAGE

    emit write (msg);
    msg = "";
    emit done();
}

void shor::getNum(bigint num)
{
    N = num;
}

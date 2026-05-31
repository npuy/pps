#include <iostream>
#include <iomanip>

#include "SimpsonIntegration.h"

double gamma(double x)
{
    if (x == 1)
        return 1;
    if (x == 0.5)
        return sqrt(M_PI);
    return (x - 1) * gamma(x - 1);
}

double ajustarX(double x, double d, double err)
{
    if (err < 0)
        return x + d;
    else
        return x - d;
}

void imprimirResultado(double resultado, double finIntervalo)
{
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Resultado de la integral: " << resultado << std::endl;
    std::cout << "Valor de x que aproxima el resultado: " << finIntervalo << std::endl;
}

bool isErrorAcceptable(double error, double cotaError)
{
    return std::abs(error) < cotaError;
}

int main(int argc, char *argv[])
{
    double dof, pEsperado, finIntervalo, inicioIntervalo, cotaError, numSegIni;
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <dof> <p_esperado> <fin_intervalo> <inicio_intervalo> <cota_error> <num_seg_ini>" << std::endl;
        exit(1);
    }
    else
    {
        dof = std::stod(argv[1]);
        pEsperado = std::stod(argv[2]);
        finIntervalo = std::stod(argv[2]) ? std::stod(argv[2]) : 1.0;
        inicioIntervalo = (argc > 3) ? std::stod(argv[3]) : 0.0;
        cotaError = (argc > 4) ? std::stod(argv[4]) : 0.000001;
        numSegIni = (argc > 5) ? std::stod(argv[5]) : 10;
    }

    auto tDist = [dof](double x) -> double
    {
        double numerador = gamma((dof + 1) / 2);
        double denominador = sqrt(dof * M_PI) * gamma(dof / 2);
        double potencia = pow(1 + (x * x) / dof, -(dof + 1) / 2);
        return (numerador / denominador) * potencia;
    };

    // imprimir los parámetros de entrada
    SimpsonIntegration integrator(cotaError, numSegIni, finIntervalo, inicioIntervalo, tDist);
    double resultado = integrator.integrate();

    double error = resultado - pEsperado;
    double errorAnterior;
    if (isErrorAcceptable(error, cotaError))
    {
        imprimirResultado(resultado, integrator.getFinIntervalo());
        return 0;
    }

    double d = 0.5;
    double x = finIntervalo;

    x = ajustarX(x, d, error);
    integrator.setFinIntervalo(x);
    resultado = integrator.integrate();
    errorAnterior = error;
    error = resultado - pEsperado;

    while (!isErrorAcceptable(error, cotaError))
    {
        if (errorAnterior * error < 0)
        {
            d /= 2;
        }

        x = ajustarX(x, d, error);
        integrator.setFinIntervalo(x);
        resultado = integrator.integrate();

        errorAnterior = error;
        error = resultado - pEsperado;
    }

    imprimirResultado(resultado, integrator.getFinIntervalo());
    return 0;
}

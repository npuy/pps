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

int main(int argc, char *argv[])
{
    // Obtener dof, fin intervalo, inicio intervalo (opcional), cota error (opcional) y
    // numero de segmentos inicial (opcional) desde los argumentos de la línea de comandos
    double dof, finIntervalo, inicioIntervalo, cotaError, numSegIni;
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <dof> <fin_intervalo> <inicio_intervalo> <cota_error> <num_seg_ini>" << std::endl;
        exit(1);
    }
    else
    {
        dof = std::stod(argv[1]);
        finIntervalo = std::stod(argv[2]);
        inicioIntervalo = (argc > 3) ? std::stod(argv[3]) : 0.0;
        cotaError = (argc > 4) ? std::stod(argv[4]) : 0.00001;
        numSegIni = (argc > 5) ? std::stod(argv[5]) : 10;
    }

    auto tDist = [dof](double x) -> double
    {
        double numerador = gamma((dof + 1) / 2);
        double denominador = sqrt(dof * M_PI) * gamma(dof / 2);
        double potencia = pow(1 + (x * x) / dof, -(dof + 1) / 2);
        return (numerador / denominador) * potencia;
    };
    SimpsonIntegration integrator(cotaError, numSegIni, finIntervalo, inicioIntervalo, tDist);
    double resultado = integrator.integrate();

    // Imprimir el resultado
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Resultado de la integral: " << resultado << std::endl;

    return 0;
}

#include <iostream>
#include <iomanip>

#include "SimpsonIntegration.h"
#include "list.h"

struct Data
{
    float x;
    float y;
};

Data parseLine(const std::string &line);
float addX(const float &acc, const Data &data);
float addY(const float &acc, const Data &data);
float addXY(const float &acc, const Data &data);
float addX2(const float &acc, const Data &data);
float addY2(const float &acc, const Data &data);
double gamma(double x);
double ajustarX(double x, double d, double err);
bool isErrorAcceptable(double error, double cotaError);
float addXminusAvgX(const float &acc, const Data &data, float avgX);
float stdDevXYStep(const float &acc, const Data &data, float b0, float b1);

int main(int argc, char *argv[])
{
    // Obtener el nombre del archivo por parametro
    const char *fname;
    float x_k;
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <filename> <x_k>" << std::endl;
        exit(1);
    }
    else
    {
        fname = argv[1];
        x_k = std::stof(argv[2]);
        printf("Archivo ingresado: %s\n", fname);
    }

    // Crear la lista y cargar los datos desde el archivo
    List<Data> list;
    list.loadFromFile(fname, parseLine);

    // Procesar los datos
    float sumX = list.reduce(addX, 0.0f);
    float sumY = list.reduce(addY, 0.0f);

    int n = list.count();

    float avgX = sumX / n;
    float avgY = sumY / n;

    float sumXY = list.reduce(addXY, 0.0f);
    float sumX2 = list.reduce(addX2, 0.0f);
    float sumY2 = list.reduce(addY2, 0.0f);

    float b1 = (sumXY - n * avgX * avgY) / (sumX2 - n * avgX * avgX);
    float b0 = avgY - b1 * avgX;

    float r = (n * sumXY - sumX * sumY) / sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
    float r2 = r * r;

    float y_k = b0 + b1 * x_k;

    // Calcular tail area
    double dof, finIntervalo, inicioIntervalo, cotaError, numSegIni;
    dof = n - 2;
    inicioIntervalo = 0.0;
    finIntervalo = abs(r) * sqrt((n - 2) / (1 - r2));
    cotaError = 0.000001;
    numSegIni = 10;

    auto tDist = [dof](double x) -> double
    {
        double numerador = gamma((dof + 1) / 2);
        double denominador = sqrt(dof * M_PI) * gamma(dof / 2);
        double potencia = pow(1 + (x * x) / dof, -(dof + 1) / 2);
        return (numerador / denominador) * potencia;
    };

    SimpsonIntegration integrator1(cotaError, numSegIni, finIntervalo, inicioIntervalo, tDist);
    double p = integrator1.integrate();
    double tailArea = 1 - 2 * p;

    // Calcular range
    double pEsperado;
    dof = n - 2;
    pEsperado = 0.35;
    inicioIntervalo = 0.0;
    finIntervalo = 1.0;
    cotaError = 0.000001;
    numSegIni = 10;

    SimpsonIntegration integrator(cotaError, numSegIni, finIntervalo, inicioIntervalo, tDist);
    double resultado = integrator.integrate();

    double error = resultado - pEsperado;
    double errorAnterior;
    if (!isErrorAcceptable(error, cotaError))
    {
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
    }

    double t035dof = integrator.getFinIntervalo();

    float sumStdDevXY = list.reduce(stdDevXYStep, 0.0f, b0, b1);
    float stdDevXY = sqrt(sumStdDevXY / (n - 2));

    float sumXminusAvgX2 = list.reduce(addXminusAvgX, 0.0f, avgX);

    double range = t035dof * stdDevXY * sqrt(1 + (1.0 / n) + (pow(x_k - avgX, 2) / sumXminusAvgX2));
    double UPI = y_k + range;
    double LPI = y_k - range;

    // Imprimir los resultados
    std::cout << "r: " << r << std::endl;
    std::cout << "r^2: " << r2 << std::endl;

    std::cout << "tailArea: " << tailArea << std::endl;

    std::cout << "b0: " << b0 << std::endl;
    std::cout << "b1: " << b1 << std::endl;
    std::cout << "y_k: " << y_k << std::endl;

    std::cout << "range: " << range << std::endl;
    std::cout << "UPI: " << UPI << std::endl;
    std::cout << "LPI: " << LPI << std::endl;

    return 0;
}

Data parseLine(const std::string &line)
{
    Data data;
    sscanf(line.c_str(), "%f,%f", &data.x, &data.y);
    return data;
}

float addX(const float &acc, const Data &data)
{
    return acc + data.x;
}

float addY(const float &acc, const Data &data)
{
    return acc + data.y;
}

float addXY(const float &acc, const Data &data)
{
    return acc + data.x * data.y;
}

float addX2(const float &acc, const Data &data)
{
    return acc + data.x * data.x;
}

float addY2(const float &acc, const Data &data)
{
    return acc + data.y * data.y;
}

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

bool isErrorAcceptable(double error, double cotaError)
{
    return std::abs(error) < cotaError;
}

float addXminusAvgX(const float &acc, const Data &data, float avgX)
{
    float diff = data.x - avgX;
    return acc + diff * diff;
}

float stdDevXYStep(const float &acc, const Data &data, float b0, float b1)
{
    float diff = data.y - b0 - data.x * b1;
    return acc + diff * diff;
}

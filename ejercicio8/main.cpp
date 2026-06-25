#include <iostream>
#include <iomanip>

#include "SimpsonIntegration.h"
#include "list.h"
#include "GaussMethod.h"

struct Data
{
    float x;
    float y;
    float w;
    float z;
};

Data parseLine(const std::string &line);
float addW(const float &acc, const Data &data);
float addX(const float &acc, const Data &data);
float addY(const float &acc, const Data &data);
float addZ(const float &acc, const Data &data);
float addW2(const float &acc, const Data &data);
float addWX(const float &acc, const Data &data);
float addWY(const float &acc, const Data &data);
float addWZ(const float &acc, const Data &data);
float addX2(const float &acc, const Data &data);
float addXY(const float &acc, const Data &data);
float addXZ(const float &acc, const Data &data);
float addY2(const float &acc, const Data &data);
float addYZ(const float &acc, const Data &data);
float addZ2(const float &acc, const Data &data);
double gamma(double x);
double ajustarX(double x, double d, double err);
bool isErrorAcceptable(double error, double cotaError);
float addWAvg(const float &acc, const Data &data, float avg);
float addXAvg(const float &acc, const Data &data, float avg);
float addYAvg(const float &acc, const Data &data, float avg);
float varStep(const float &acc, const Data &data, float b0, float b1, float b2, float b3);

int main(int argc, char *argv[])
{
    // Obtener el nombre del archivo por parametro
    const char *fname;
    float w_k, x_k, y_k;
    if (argc < 5)
    {
        std::cerr << "Usage: " << argv[0] << " <filename> <w_k> <x_k> <y_k>" << std::endl;
        exit(1);
    }
    else
    {
        fname = argv[1];
        w_k = std::stof(argv[2]);
        x_k = std::stof(argv[3]);
        y_k = std::stof(argv[4]);
        printf("Archivo ingresado: %s\n", fname);
    }

    // Crear la lista y cargar los datos desde el archivo
    List<Data> list;
    list.loadFromFile(fname, parseLine);

    // Calcular terminos de la matriz
    int n = list.count();
    float sumW = list.reduce(addW, 0.0f);
    float sumX = list.reduce(addX, 0.0f);
    float sumY = list.reduce(addY, 0.0f);
    float sumZ = list.reduce(addZ, 0.0f);
    float sumW2 = list.reduce(addW2, 0.0f);
    float sumWX = list.reduce(addWX, 0.0f);
    float sumWY = list.reduce(addWY, 0.0f);
    float sumWZ = list.reduce(addWZ, 0.0f);
    float sumX2 = list.reduce(addX2, 0.0f);
    float sumXY = list.reduce(addXY, 0.0f);
    float sumXZ = list.reduce(addXZ, 0.0f);
    float sumY2 = list.reduce(addY2, 0.0f);
    float sumYZ = list.reduce(addYZ, 0.0f);

    float matriz[4][5] = {
        {static_cast<float>(n), sumW, sumX, sumY, sumZ},
        {sumW, sumW2, sumWX, sumWY, sumWZ},
        {sumX, sumWX, sumX2, sumXY, sumXZ},
        {sumY, sumWY, sumXY, sumY2, sumYZ},
    };

    // Calcular parametros beta b0, b1, b2 y b3 con metodo de gauss
    GaussMethod gauss((float *)matriz);
    float result[4];
    gauss.solve(4, result);
    float b0 = result[0];
    float b1 = result[1];
    float b2 = result[2];
    float b3 = result[3];

    // Calcular t(0.35, n-4)
    double dof, finIntervalo, inicioIntervalo, cotaError, numSegIni, pEsperado;
    dof = n - 4;
    pEsperado = 0.35;
    inicioIntervalo = 0.0;
    finIntervalo = 1.0;
    cotaError = 0.000001;
    numSegIni = 10;

    auto tDist = [dof](double x) -> double
    {
        double numerador = gamma((dof + 1) / 2);
        double denominador = sqrt(dof * M_PI) * gamma(dof / 2);
        double potencia = pow(1 + (x * x) / dof, -(dof + 1) / 2);
        return (numerador / denominador) * potencia;
    };

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

    // Calcular desviacion estandar
    double varSum = list.reduce(varStep, 0.0f, b0, b1, b2, b3);
    double var = varSum / (n - 4);
    double stdDev = sqrt(var);

    // Calcular rango
    float avgW = sumW / n;
    float sumWAvg = list.reduce(addWAvg, 0.0f, avgW);
    float avgX = sumX / n;
    float sumXAvg = list.reduce(addXAvg, 0.0f, avgX);
    float avgY = sumY / n;
    float sumYAvg = list.reduce(addYAvg, 0.0f, avgY);
    float wTerm = pow(w_k - avgW, 2) / sumWAvg;
    float xTerm = pow(x_k - avgX, 2) / sumXAvg;
    float yTerm = pow(y_k - avgY, 2) / sumYAvg;
    float rootTerm = sqrt(1 + 1 / n + wTerm + xTerm + yTerm);

    float range = t035dof * stdDev * rootTerm;

    // Calculamos las horas proyectadas
    float projectedHours = b0 + w_k * b1 + x_k * b2 + y_k * b3;
    float UPI = projectedHours + range;
    float LPI = projectedHours - range;

    // Imprimir los resultados
    // b0, b1, b2, b3, projectedHours, UPI, LPI
    std::cout << "b0: " << b0 << std::endl;
    std::cout << "b1: " << b1 << std::endl;
    std::cout << "b2: " << b2 << std::endl;
    std::cout << "b3: " << b3 << std::endl;
    std::cout << "projectedHours: " << projectedHours << std::endl;
    std::cout << "UPI: " << UPI << std::endl;
    std::cout << "LPI: " << LPI << std::endl;

    return 0;
}

Data parseLine(const std::string &line)
{
    Data data;
    sscanf(line.c_str(), "%f,%f,%f,%f", &data.w, &data.x, &data.y, &data.z);
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

float addW(const float &acc, const Data &data)
{
    return acc + data.w;
}

float addZ(const float &acc, const Data &data)
{
    return acc + data.z;
}

float addW2(const float &acc, const Data &data)
{
    return acc + data.w * data.w;
}

float addWX(const float &acc, const Data &data)
{
    return acc + data.x * data.w;
}

float addWY(const float &acc, const Data &data)
{
    return acc + data.y * data.w;
}

float addX2(const float &acc, const Data &data)
{
    return acc + data.x * data.x;
}

float addXY(const float &acc, const Data &data)
{
    return acc + data.x * data.y;
}

float addXZ(const float &acc, const Data &data)
{
    return acc + data.x * data.z;
}

float addY2(const float &acc, const Data &data)
{
    return acc + data.y * data.y;
}

float addYZ(const float &acc, const Data &data)
{
    return acc + data.y * data.z;
}

float addWZ(const float &acc, const Data &data)
{
    return acc + data.w * data.z;
}

float addZ2(const float &acc, const Data &data)
{
    return acc + data.z * data.z;
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

float addWAvg(const float &acc, const Data &data, float avg)
{
    float diff = data.w - avg;
    return acc + diff * diff;
}

float addXAvg(const float &acc, const Data &data, float avg)
{
    float diff = data.x - avg;
    return acc + diff * diff;
}

float addYAvg(const float &acc, const Data &data, float avg)
{
    float diff = data.y - avg;
    return acc + diff * diff;
}

float varStep(const float &acc, const Data &data, float b0, float b1, float b2, float b3)
{
    float diff = data.z - b0 - data.w * b1 - data.x * b2 - data.y * b3;
    return acc + diff * diff;
}

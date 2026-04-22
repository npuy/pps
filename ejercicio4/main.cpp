#include <iostream>
#include <iomanip>

#include "list.h"

struct Data
{
    float x;
    float y;
    float n;
    float ln;
};

Data parseLine(const std::string &line);
float addLN(const float &acc, const Data &data);
float addLNminusAvgSquared(const float &acc, const Data &data, float avgLN);

int main(int argc, char *argv[])
{
    // Obtener el nombre del archivo por parametro
    const char *fname;
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        exit(1);
    }
    else
    {
        fname = argv[1];
        printf("Archivo ingresado: %s\n", fname);
    }

    // Crear la lista y cargar los datos desde el archivo
    List<Data> list;
    list.loadFromFile(fname, parseLine);

    // Procesar los datos
    int count = list.count();

    // Calcular meddia de valores ln(n)
    float sumLN = list.reduce(addLN, 0.0f);
    float avgLN = sumLN / count;

    // Calcular varianza de ln(n)
    float sumLNminusAvgSquared = list.reduce(addLNminusAvgSquared, 0.0f, avgLN);
    float varianceLN = sumLNminusAvgSquared / (count - 1);

    // Calcular desviacion estandar de ln(n)
    float stddevLN = sqrt(varianceLN);

    // Calcular rangos logaritmicos
    float lnVS = avgLN - 2 * stddevLN;
    float lnS = avgLN - stddevLN;
    float lnM = avgLN;
    float lnL = avgLN + stddevLN;
    float lnVL = avgLN + 2 * stddevLN;

    // Calcular valores medios de cada rango
    float VS = exp(lnVS);
    float S = exp(lnS);
    float M = exp(lnM);
    float L = exp(lnL);
    float VL = exp(lnVL);

    // Imprimir los resultados
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "VS: " << VS << std::endl;
    std::cout << "S: " << S << std::endl;
    std::cout << "M: " << M << std::endl;
    std::cout << "L: " << L << std::endl;
    std::cout << "VL: " << VL << std::endl;

    return 0;
}

Data parseLine(const std::string &line)
{
    Data data;
    sscanf(line.c_str(), "%f,%f", &data.x, &data.y);

    data.n = data.x / data.y;
    data.ln = log(data.n);

    return data;
}

float addLN(const float &acc, const Data &data)
{
    return acc + data.ln;
}

float addLNminusAvgSquared(const float &acc, const Data &data, float avgLN)
{
    float diff = data.ln - avgLN;
    return acc + diff * diff;
}

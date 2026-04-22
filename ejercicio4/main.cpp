#include <iostream>
#include <iomanip>

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

    // Imprimir los resultados
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "b0: " << b0 << std::endl;
    std::cout << "b1: " << b1 << std::endl;
    std::cout << "r: " << r << std::endl;
    std::cout << "r^2: " << r2 << std::endl;
    std::cout << "y_k: " << y_k << std::endl;

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

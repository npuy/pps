#include <cmath>
#include <stdexcept>
#include <iostream>

#ifndef GAUSS_METHOD_H
#define GAUSS_METHOD_H

// Recibe una matriz de nx(n+1) y devuelve un vector con los parametros b0, b1, b2, ..., bn-1
class GaussMethod
{
private:
    float *matriz;

public:
    GaussMethod(float *matriz) : matriz(matriz) {}
    void solve(int n, float *result)
    {
        for (int i = 0; i < n; i++)
        {
            // se encuentra el valor maximo en la columna i, desde la fila i hasta la fila n-1
            int maxRow = i;
            for (int k = i + 1; k < n; k++)
            {
                if (std::abs(matriz[k * (n + 1) + i]) > std::abs(matriz[maxRow * (n + 1) + i]))
                {
                    maxRow = k;
                }
            }

            // si el valor maximo es 0, entonces no hay solucion unica
            if (matriz[maxRow * (n + 1) + i] == 0)
            {
                throw std::runtime_error("No unique solution exists.");
            }

            // se obtiene el valor maximo y se intercambia con la fila i
            if (maxRow != i)
            {
                for (int j = i; j < n + 1; j++)
                {
                    std::swap(matriz[i * (n + 1) + j], matriz[maxRow * (n + 1) + j]);
                }
            }

            // se divide toda la fila i por el valor maximo
            float pivotValue = matriz[i * (n + 1) + i];
            for (int j = i; j < n + 1; j++)
            {
                matriz[i * (n + 1) + j] /= pivotValue;
            }

            // se hace 0 la columna i de las filas debajo de la fila i
            for (int l = i + 1; l < n; l++)
            {
                float factor = matriz[l * (n + 1) + i];
                for (int j = i; j < n + 1; j++)
                {
                    matriz[l * (n + 1) + j] -= factor * matriz[i * (n + 1) + j];
                }
            }
        }

        // Se resuelve el sistema de ecuaciones resultante
        for (int i = n - 1; i >= 0; i--)
        {
            result[i] = matriz[i * (n + 1) + n];
            for (int j = i + 1; j < n; j++)
            {
                result[i] -= matriz[i * (n + 1) + j] * result[j];
            }
        }
    }
};

#endif
#include <stdexcept>
#include <iostream>
#include <fstream>

#ifndef SIMPSON_INTEGRATION_H
#define SIMPSON_INTEGRATION_H

class SimpsonIntegration
{
private:
    double delta_error;
    int num_seg_ini;
    double fin_intervalo;
    double inicio_intervalo;
    // Funcion a integrar
    std::function<double(double)> f;

    double calcularPaso(int n, double W)
    {
        double integral_curr = 0.0;
        integral_curr = f(inicio_intervalo) + f(fin_intervalo);
        for (int i = 1; i < n; ++i)
        {
            double x_i = inicio_intervalo + i * W;
            if (i % 2 == 0)
                integral_curr += 2 * f(x_i);
            else
                integral_curr += 4 * f(x_i);
        }

        integral_curr *= (W / 3);
        return integral_curr;
    }

public:
    SimpsonIntegration(double delta_error, int num_seg_ini, double fin_intervalo, double inicio_intervalo, std::function<double(double)> func)
        : delta_error(delta_error), num_seg_ini(num_seg_ini), fin_intervalo(fin_intervalo), inicio_intervalo(inicio_intervalo), f(func) {}

    double integrate()
    {
        int n = num_seg_ini;
        double W = (fin_intervalo - inicio_intervalo) / n;
        double integral_prev = 0.0;
        double integral_curr = 0.0;

        // Inicializar integral_curr con la primera aproximación usando el método de Simpson
        integral_curr = calcularPaso(n, W);
        n *= 2; // Duplicar el número de segmentos
        W /= 2; // Reducir el tamaño del paso

        do
        {
            integral_prev = integral_curr;

            integral_curr = calcularPaso(n, W);

            n *= 2; // Duplicar el número de segmentos
            W /= 2; // Reducir el tamaño del paso

        } while (std::abs(integral_curr - integral_prev) > delta_error);

        return integral_curr;
    }

    void setFinIntervalo(double fin)
    {
        fin_intervalo = fin;
    }

    double getFinIntervalo() const
    {
        return fin_intervalo;
    }
};

#endif
#include <iostream>
#include <iomanip>

#include "list.h"

int main()
{
    // Se obtiene de la entrada estndar un numero real y se agrega a la lista, hasta que se ingrese un numero negativo.
    List list;
    float input;
    std::cout << "Ingrese un numero real (negativo para terminar): ";
    while (std::cin >> input && input >= 0)
    {
        list.add(input);
    }

    // Se imprime la cantidad de elementos en la lista.
    int count = list.count();
    std::cout << "Cantidad de elementos en la lista: " << count << std::endl;

    // Se calcula la media de los elementos en la lista
    float sum = 0;
    for (int i = 0; i < count; i++)
    {
        sum += list.getElement(i);
    }
    float average = sum / count;
    std::cout << "Media de los elementos en la lista: " << average << std::endl;

    // Se calcula la desviacion estandar de los elementos en la lista
    float variance = 0;
    for (int i = 0; i < count; i++)
    {
        float diff = list.getElement(i) - average;
        variance += diff * diff;
    }
    variance /= (count - 1);
    float stddev = sqrt(variance);
    std::cout << "Desviacion estandar de los elementos en la lista: " << std::fixed << std::setprecision(10) << stddev << std::endl;

    return 0;
}

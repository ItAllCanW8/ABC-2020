#include <iostream>
#include <omp.h>
#include <time.h>
#include <chrono>

int main() 
{
    int size;
    int** matrix;
    int *vector, *compVector;

    std::cout << "Size (matrix[size][size] & vector[size]) = ";
    std::cin >> size;

    matrix = new int* [size];
    vector = new int[size];
    compVector = new int[size];

    for (int i = 0; i < size; i++) 
    {
        matrix[i] = new int[size];
        vector[i] = rand();
        compVector[i] = 0;
    }

    for (int i = 0; i < size; i++) 
    {
        for (int j = 0; j < size; j++) 
            matrix[i][j] = rand();      
    }

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            compVector[i] += matrix[i][j] * vector[j];
        }
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\n>Single thread execution time: " << duration.count() << " ms" << std::endl;

    for (size_t i = 0; i < size; i++)
        compVector[i] = 0;

    int i, j;

    start = std::chrono::steady_clock::now();

#pragma omp parallel for private(j)
        for ( i = 0; i < size; i++)
        {
            for (j = 0; j < size; j++)
            {
                compVector[i] += matrix[i][j] * vector[j];
            }
        }

    end = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "\n>Multi thread execution time with OpenMP: " << duration.count() << " ms" << std::endl;

    for (int i = 0; i < size; i++) {
        delete[] matrix[i];
    }

    delete[] matrix;
    delete[] vector;
    delete[] compVector;

    return 0;
}
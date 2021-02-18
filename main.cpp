#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip> // std::setprecision
#include <cmath>
#include "fft/dj_fft.h"

struct Data
{
    std::vector<std::complex<double>> x, y, z; // input data
    int dataCount;
    int n;
    float sampleRate;
};

int ReadFile(std::string, Data &);

int main()
{
    Data data;
    if (!ReadFile("res/1602245833-2715-NAO7856.txt", data))
    {

        auto fftDataX = dj::fft1d(data.x, dj::fft_dir::DIR_FWD);
        auto fftDataY = dj::fft1d(data.y, dj::fft_dir::DIR_FWD);
        auto fftDataZ = dj::fft1d(data.z, dj::fft_dir::DIR_FWD);

        // salva resultados em arquivo de texto csv
        std::ofstream outputFile;
        outputFile.open("output.txt");

        int N = data.x.size(); // tamanho do input
        for (int i = 0; i < N; ++i)
        {
            float freq = i * data.sampleRate / N;
            outputFile << fftDataX[i].real() << "," << fftDataY[i].real() << "," << fftDataZ[i].real() << "," << freq << "\n";
        }
        outputFile.close();
    }

    return 0;
}

int ReadFile(std::string filename, Data &dataOut)
{
    std::ifstream inputFile(filename);

    // Checa se foi possivel abrir o arquivo
    if (!inputFile.is_open())
    {
        return -1;
    }

    std::string line;

    std::string elapsedTimeMsStr = filename.substr(filename.find_first_of("-") + 1, filename.find_last_of("-") - filename.find_first_of("-") - 1); // separa tempo elapsado do nome do arquivo
    float elapsedTime = std::atof(elapsedTimeMsStr.c_str()) / 1000;                                                                                // converte para segundos

    int lineCount = 0;
    if (inputFile.good())
    {
        //  remove o cabeçalho
        std::getline(inputFile, line);

        // le os dados linha por linha
        while (std::getline(inputFile, line))
        {
            lineCount++; // incrementa contagem de linhas

            std::stringstream ss(line); // transforma linha em stream para extrair os dados
            double i;

            ss >> i; // extrai primeiro numero (x)
            dataOut.x.push_back(i);
            ss.ignore(); // ignora primeira virgula
            ss >> i;     // extrai o segundo numero (y)
            dataOut.y.push_back(i);
            ss.ignore(); // ignora segunda virgula
            ss >> i;     // extrai o terceiro numero(z)
            dataOut.z.push_back(i);
        }
        int next = std::pow(2, ceil(log(lineCount) / log(2))); // calcula proxima potencia de 2 dps de lineCount
        while (dataOut.x.size() < next)                        // padding nos dados para que o algoritimo de fft funcione(potencia de 2)
        {
            dataOut.x.push_back(0);
            dataOut.y.push_back(0);
            dataOut.z.push_back(0);
        }
        dataOut.n = next;
        dataOut.dataCount = lineCount;
        dataOut.sampleRate = float(lineCount) / elapsedTime; // calcula sample rate (medidas/s)

        inputFile.close();
        return 0;
    }

    inputFile.close();
    return -1;
}
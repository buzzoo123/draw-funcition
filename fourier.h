#include <iostream>
#include <vector>
#include <complex>
#include <numbers>

using namespace std;

const complex<double> imaginary_i(0.0, 1.0);

// 2D Discrete Fourier Transform using FFT
vector<vector<complex<double>>> DFT2D(vector<vector<int>>& matrix);

// 2D FFT-based DFT for faster transformation
vector<vector<complex<double>>> DFT2DFFT(vector<vector<int>>& matrix);

// Helper function to calculate individual elements in the DFT matrix
complex<double> calculateDFT2D(vector<vector<int>>& matrix, size_t k, size_t l);

// Helper function to calculate each step in the DFT formula
complex<double> getDftStep(vector<vector<int>>& matrix, size_t r, size_t c, size_t k, size_t l);

// 1D FFT function for integer input with complex output and power-of-2 padding
vector<complex<double>> FFT(vector<complex<double>>& input);

// Helper functions for FFT
void reorderInput(vector<complex<double>>& input); // Reorder input for FFT using bit-reversal
int getBitReversedIndex(int index, int listSize);  // Bit reversal function for FFT reordering

// Utility function to find the next power of 2 for padding
int getNextPowerOf2(size_t currentSize);

// Utility function to transpose a complex matrix (for 2D FFT)
vector<vector<complex<double>>> transposeMatrix(const vector<vector<complex<double>>>& matrix);

// Utility function to print a matrix of complex numbers for debugging
void printComplexMatrix(const vector<vector<complex<double>>>& matrix);

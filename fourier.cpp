#include "fourier.h"

using namespace std;

vector<vector<complex<double>>> DFT2D(vector<vector<int>>& matrix)   {
    vector<vector<complex<double>>> dftMatrix(matrix.size(), vector<complex<double>>(matrix[0].size(), 0));
    for(size_t r = 0; r < matrix.size(); ++r)   {
        for(size_t c = 0; c < matrix[0].size(); ++c)    {
            //naive approach O(n^4)!
            dftMatrix[r][c] = calculateDFT2D(matrix, r, c);
        }
    }
    return dftMatrix;
}

int getNextPowerOf2(size_t currentSize)   {
    currentSize--;
    currentSize |= currentSize >> 1;
    currentSize |= currentSize >> 2;
    currentSize |= currentSize >> 4;
    currentSize |= currentSize >> 8;
    currentSize |= currentSize >> 16;
    currentSize++;
    return currentSize;
}

vector<vector<complex<double>>> DFT2DFFT(vector<vector<int>>& matrix) {
    // Step 1: Convert each row to complex and apply FFT
    vector<vector<complex<double>>> rowTransformed(matrix.size());
    for (size_t r = 0; r < matrix.size(); ++r) {
        vector<complex<double>> complexRow(matrix[r].begin(), matrix[r].end());
        rowTransformed[r] = FFT(complexRow);
    }

    // Step 2: Transpose the result of the row-wise FFT
    auto transposed = transposeMatrix(rowTransformed);

    // Step 3: Perform FFT on each "column" of the original matrix (each row of the transposed matrix)
    for (size_t c = 0; c < transposed.size(); ++c) {
        transposed[c] = FFT(transposed[c]);
    }

    // Step 4: Transpose back to original orientation
    auto finalResult = transposeMatrix(transposed);

    return finalResult;
}

vector<vector<complex<double>>> transposeMatrix(const vector<vector<complex<double>>>& matrix) {
    size_t rows = matrix.size();
    size_t cols = matrix[0].size();
    
    // Create a new matrix with transposed dimensions
    vector<vector<complex<double>>> transposed(cols, vector<complex<double>>(rows));

    // Fill the new matrix by swapping rows and columns
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            transposed[j][i] = matrix[i][j];
        }
    }
    
    return transposed;
}



vector<complex<double>> FFT(vector<complex<double>>& input) {
    // Find next power of two to pad for efficient FFT calculation
    int powerOf2Size = getNextPowerOf2(input.size());
    vector<complex<double>> paddedInput(powerOf2Size, 0);

    // Copy over initial values to padded vector as complex numbers
    for (size_t i = 0; i < input.size(); ++i) {
        paddedInput[i] = input[i];
    }

    // Reorder paddedInput array for bottom-up FFT solution
    reorderInput(paddedInput);

    // Perform FFT calculations
    // Outer loop over stages (each stage doubles the butterfly size)
    for (int stageSize = 2; stageSize <= powerOf2Size; stageSize *= 2) {
        
        // Calculate the twiddle increment factor for this stage
        complex<double> twiddleIncrement = exp(-2.0 * numbers::pi * imaginary_i / static_cast<double>(stageSize));

        // Loop over each butterfly block in the current stage
        for (int start = 0; start < powerOf2Size; start += stageSize) {
            complex<double> twiddleFactor = 1.0;  // Reset for each new butterfly

            // Inner loop over each butterfly pair in the block
            for (int pair = 0; pair < stageSize / 2; ++pair) {
                // Top and bottom index within the butterfly
                int topIndex = start + pair;
                int bottomIndex = start + pair + stageSize / 2;

                // Calculate butterfly values
                complex<double> top = paddedInput[topIndex];
                complex<double> bottom = twiddleFactor * paddedInput[bottomIndex];

                // Update values in-place
                paddedInput[topIndex] = top + bottom;
                paddedInput[bottomIndex] = top - bottom;

                // Update twiddle factor
                twiddleFactor *= twiddleIncrement;
            }
        }
    }

    // Return the transformed array
    return paddedInput;
}


void reorderInput(vector<complex<double>>& input)   {
    for (int i = 0; i < input.size(); ++i) {
        int j = getBitReversedIndex(i, input.size()); // Function to calculate bit-reversed index
        if (i < j) {
            swap(input[i], input[j]);
        }
    }
}

int getBitReversedIndex(int index, int listSize) {
    int rev = 0;
    int bits = static_cast<int>(log2(listSize)); // Number of bits in listSize (assuming listSize is a power of 2)

    for (int i = 0; i < bits; i++) {
        // Shift rev left to make space for the next bit
        rev = (rev << 1) | (index & 1); // Add the least significant bit of index to rev
        index >>= 1; // Shift index right to get the next bit in the next iteration
    }

    return rev;
}

complex<double> calculateDFT2D(vector<vector<int>>& matrix, size_t k, size_t l)    {
    complex<double> dftSum = 0;
    for(size_t r = 0; r < matrix.size(); ++r)   {
        for(size_t c = 0; c < matrix[0].size(); ++c)    {
            dftSum += getDftStep(matrix, r, c, k, l);
        }
    }
    return dftSum;
}

complex<double> getDftStep(vector<vector<int>>& matrix, size_t r, size_t c, size_t k, size_t l) {
    double M = static_cast<double>(matrix.size());
    double N = static_cast<double>(matrix[0].size());
    double angle = -2.0 * numbers::pi * ((k * r) / M) + ((l * c) / N);
    return static_cast<double>(matrix[r][c]) * exp(imaginary_i * angle);
}

void performIFFT(const vector<vector<complex<double>>>& fft_output, vector<vector<complex<double>>>& ifft_result) {
    int rows = fft_output.size();
    int cols = fft_output[0].size();
    int N = rows * cols;

    // Allocate memory for FFTW input/output in 1D format
    fftw_complex *in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

    // Convert 2D vector to 1D fftw_complex array
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int idx = r * cols + c;
            in[idx][0] = fft_output[r][c].real();
            in[idx][1] = fft_output[r][c].imag();
        }
    }

    // Create FFTW plan for IFFT
    fftw_plan ifft_plan = fftw_plan_dft_2d(rows, cols, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);

    // Execute IFFT
    fftw_execute(ifft_plan);

    // Convert 1D fftw_complex array back to 2D vector and normalize
    ifft_result.resize(rows, vector<complex<double>>(cols));
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int idx = r * cols + c;
            ifft_result[r][c] = complex<double>(out[idx][0] / N, out[idx][1] / N);  // Normalize
        }
    }

    // Clean up
    fftw_destroy_plan(ifft_plan);
    fftw_free(in);
    fftw_free(out);
}

void printComplexMatrix(const vector<vector<complex<double>>>& matrix) {
    for (const auto& row : matrix) {
        for (const auto& elem : row) {
            cout << "(" << elem.real() << ", " << elem.imag() << "i) ";
        }
        cout << endl;
    }
}
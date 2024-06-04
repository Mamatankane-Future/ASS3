#include "Matrix.h"

Matrix::Matrix(int n, int m){
    if (n < 1 || m < 1){
        throw MathExceptions::InvalidMatrixSize;
    }
    this->n = n;
    this->m = m;
    arr = new double *[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = new double[m];
    }
}

Matrix::Matrix(int n, int m, double ** arr){
    if (n < 1 || m < 1){
        throw MathExceptions::InvalidMatrixSize;
    }
    
    this->n = n;
    this->m = m;
    this->arr = arr;
}

Matrix::Matrix(const Matrix & matrix){
    this->n = matrix.n;
    this->m = matrix.m;
    arr = new double *[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = new double[m];
        for (int j = 0; j < m; j++)
        {
            arr[i][j] = matrix[i][j];
        }
    }
}

Matrix::~Matrix(){
    for (int i = 0; i < n; i++)
    {
        delete[] arr[i];
    }
    delete[] arr;
}

Matrix Matrix::operator*(const Matrix matrix) const{
    if (m != matrix.n){
        matrix.print();
        throw MathExceptions::InvalidMatrixMultiplication;
    }

    double ** total = new double*[n];
    for (int i = 0; i < n; i++)
    {
        total[i] = new double[matrix.m];
        for (int j = 0; j < matrix.m; j++){
            total[i][j] = 0;
            for (int k = 0; k < m; k++){
                total[i][j] += arr[i][k] * matrix[k][j];
            }
        }
    }
    return Matrix(n, matrix.m, total);
}
    
Matrix Matrix::operator*(const double scalar) const{
    double ** total = new double*[n];
    for (int i = 0; i < n; i++){
        total[i] = new double[m];
        for (int j = 0; j < m; j++){
            total[i][j] = scalar * arr[i][j];
        }
    }
    return Matrix(n, m, total);
}

Matrix Matrix::operator+(const Matrix matrix) const{
    if (n != matrix.n || m != matrix.m){
        throw MathExceptions::InvalidMatrixAddition;
    }

    double ** total = new double*[n];
    for (int i = 0; i < n; i++){
        total[i] = new double[m];
        for (int j = 0; j < m; j++){
            total[i][j] = arr[i][j] + matrix[i][j];
        }
    }

    return Matrix(n, m, total);
}
    
Matrix Matrix::operator~() const{
    double ** temp = new double*[m];
    
    for (int i = 0; i < m; i ++){
        temp[i] = new double[n];
    }

    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++){
            temp[j][i] = arr[i][j];
        }
    }

    return Matrix(m, n, temp);
}

int Matrix::getM() const{
    return m;
}
    
int Matrix::getN() const{
    return n;
}

SquareMatrix::SquareMatrix(int n) : Matrix(n, n){

}

SquareMatrix::SquareMatrix(int n, double ** arr) : Matrix(n, n, arr){

}

SquareMatrix::~SquareMatrix(){

}

double calcDeterminate(double **arr, int n){
    if (n < 1){
        return 0;
    }
    if (n < 2){
        return arr[0][0];
    }
    if (n == 2){
        return arr[0][0] * arr[1][1] - arr[0][1] * arr[1][0];
    }
    else {
        double total = 0;
        for (int i = 0; i < n; i++){
            double ** temp = new double*[n-1];
            for (int j = 0; j < n - 1; j++){
                temp[j] = new double[n - 1];
            }
            for (int r = 1; r < n; r++){
                for (int c = 0; c < n; c++){
                    if (c < i){
                        temp[r - 1][c] = arr[r][c];
                    }
                    else if (c > i){
                        temp[r - 1][c - 1] = arr[r][c];
                    }
                }
            }
            total +=  pow(-1, i) * arr[0][i] * calcDeterminate(temp, n - 1);

            // for (int r = 0; r < n-1; r++){
            //     delete [] temp[r];
            // }

            // delete [] temp;
        }
        return total;
    }
}

Vector SquareMatrix::solve(const Vector vector) const{
    if (n != vector.getN()){
        throw MathExceptions::UnsolvableSystemOfLinearEquations;
    }

    if (n == 1){
        double * solutions = new double[n];
        solutions[0] = vector[0] / arr[0][0];
        return Vector(n, solutions);
    
    }

    double * solutions = new double[n];
    double det = determinant();
    double ** temp = new double*[n];
    for (int i = 0; i < n; i++){
        temp[i] = new double[n];
        for (int j = 0; j < n; j++){
            temp[i][j] = arr[i][j];
        }
    }

    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            temp[j][i] = vector[j];
        }

        solutions[i] = calcDeterminate(temp, n) / det;

        for (int j = 0; j < n; j++){
            temp[j][i] = arr[j][i];
        }

    }

    return Vector(n, solutions);
}

double SquareMatrix::determinant() const{
    return calcDeterminate(arr, n);
}

double matrixOfMinors(double **arr, int n){
}
SquareMatrix SquareMatrix::operator!() const{

    double det = determinant();
    if (det == 0){
        throw MathExceptions::UnsolvableSystemOfLinearEquations;
    }

    if (n == 2){
        double ** temp = new double*[n];
        for (int i = 0; i < n; i++){
            temp[i] = new double[n];
        }
        temp[0][0] = (1/det) * arr[1][1];
        temp[0][1] = (1/det) * -arr[0][1];
        temp[1][0] = (1/det) * -arr[1][0];
        temp[1][1] = (1/det) * arr[0][0];
        return SquareMatrix(n, temp);
    }
  
    double minors[n][n];
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            int rows = n - 1;
            int cols = n - 1;
            double ** temp = new double*[rows];
            for (int r = 0; r < n; r++){
                temp[r] = new double[cols];
                for (int c = 0; c < n; c++){                    
                    if (r == i || c == j) continue;
                    if (r > i && c < j){
                        temp[r - 1][c] = arr[r][c];
                    }
                    else if (r < i && c > j){
                        temp[r][c - 1] = arr[r][c];
                    }
                    else if (r > i && c > j){
                        temp[r - 1][c - 1] = arr[r][c];
                    }
                    else {
                        temp[r][c] = arr[r][c];
                    }
                }
            
            }
        
    
            minors[i][j] = pow(-1, i) * pow(-1, j) * calcDeterminate(temp, n - 1);
          
        }
    }

    double ** adjoint = new double*[n];
    for (int i = 0; i < n; i++){
        adjoint[i] = new double[n];
        for (int j = 0; j < n; j++){
            adjoint[i][j] = minors[j][i];
        }
    }

    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++){
            adjoint[i][j] = (1/det) * adjoint[i][j];
        }
    }

    return SquareMatrix(n, adjoint);

}

IdentityMatrix::IdentityMatrix(int n) : SquareMatrix(n){
    arr = new double *[n];
    for (int i = 0; i < n; i++){
        arr[i] = new double[n];
        for (int j =0; j < n; j++) arr[i][j] = 0;
        arr[i][i] = 1;
    }
    
}
    
IdentityMatrix::~IdentityMatrix(){

}
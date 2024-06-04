#include "Vector.h"
#include "Matrix.h"
#include <cmath>


Vector::Vector(int n){
    if (n < 1) {
        throw MathExceptions::InvalidVectorSize;
    }
    this->n = n;
    arr = new double[n];
}

Vector::Vector(int n, double * arr){
    if (n < 1) {
        throw MathExceptions::InvalidVectorSize;
    }
    this->n = n;
    this->arr = arr;
}

Vector::~Vector(){
    delete[] arr;
}
    
Vector::Vector(const Vector & vector){
    n = vector.n;
    this->arr = new double[n];
    for (int i = 0; i < n; i++)
    {
        this->arr[i] = vector[i];
    }
}
Vector Vector::operator+(const Vector vector) const{
    if (n != vector.n) {
        throw MathExceptions::InvalidVectorAddition;
    }
    
    double * total = new double[n];

    for (int i = 0; i < n; i++)
    {
        total[i] = arr[i] + vector[i];
    }
    return Vector(n, total);
};
Vector Vector::operator-(const Vector vector) const{
    if (n != vector.n){
        throw MathExceptions::InvalidVectorAddition;
    }

    double * total = new double[n];
    for (int i = 0; i < n; i++){
        total[i] = arr[i] - vector[i];
    }

    return Vector(n, total);
};
    
Vector Vector::operator*(const double scalar) const{
    double * total = new double[n];
    for (int i = 0; i < n; i++){
        total[i] = scalar * arr[i];
    }
    
    return Vector(n, total);
};

double Vector::operator*(const Vector vector) const{
    if (n != vector.n){
        throw MathExceptions::InvalidDotProduct;
    }
    double total = 0;
    for (int i = 0; i < n; i++){
        total += arr[i] * vector[i];
    }
    return total;
};

double Vector::magnitude() const{
    return sqrt((*this) * (*this));
}

Vector::operator Matrix() const{
    double ** total = new double*[n];
    for (int i = 0; i < n; i++){
        total[i] = new double[1];
        total[i][0] = arr[i];
    }
    return Matrix(n, 1, total);
}
    
Vector Vector::crossProduct(const Vector vector) const{
    if (n != vector.n || n != 3){
        throw MathExceptions::InvalidCrossProduct;
    
    }
    double * total = new double[3];
    total[0] = arr[1] * vector[2] - arr[2] * vector[1];
    total[1] = arr[2] * vector[0] - arr[0] * vector[2];
    total[2] = arr[0] * vector[1] - arr[1] * vector[0];

    return Vector(3, total);
}

Vector Vector::unitVector() const{
    double mag = magnitude();
    if (mag == 0){
        throw MathExceptions::InvalidUnitVector;
    }
    return (*this) * (1/mag);
}

int Vector::getN() const{
    return n;
}
    
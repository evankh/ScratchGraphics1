#ifndef __EKH_ARBITRARY_MATRIX__
#define __EKH_ARBITRARY_MATRIX__

template<unsigned int M>
class Vector {
private:
	float mValues[M];
public:
	float& operator[](unsigned int index);
	Vector<M> operator+(Vector<M> &other);
	Vector<M> operator*(float c);
};

template<unsigned int M, unsigned int N>
class Matrix {
private:
	union Data {
		float mValues[M][N];
		Vector<N> mRows[M];
		Data() {};
	} mData;
public:
	Vector<N>& operator[](unsigned int index);
	Matrix<M, N> operator+(Matrix<M, N> &other);
	Matrix<M, N> operator*(float c);
	Vector<M> operator*(Vector<N> &other);
	template<unsigned int L> Matrix<M, L> operator*(Matrix<N, L> &other);
};

template<unsigned int M>
Matrix<M + 1, M + 1> makeTranslationMatrix(Vector<M> pos);

template<unsigned int M>
Matrix<M, M> makeScaleMatrix(float c);

template<unsigned int M>
Matrix<M + 1, M + 1> makeScaleMatrix(Vector<M> scale);

#endif//__EKH_ARBITRARY_MATRIX__
#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>

/*
    这个文件定义写一些代数运算的操作，里面全是模板
*/

// 前向声明
template<size_t DimCols, size_t DimRows, typename T> class mat;

/* -------------------------------------------- 向量 -------------------------------------*/

// 定义 DIM 维的，T 类型的 向量
template <size_t DIM, typename T> 
struct vec
{
    vec() { for (size_t i = DIM; i--; data_[i] = T()); }

    // 重载
    T& operator[](const size_t i) { assert(i < DIM); return data_[i]; }
    const T& operator[](const size_t i) const { assert(i < DIM); return data_[i]; }

private:
    T data_[DIM];
};

// 定义有2个维度的，T类型的向量（可以用来表示坐标）
template <typename T> 
struct vec<2, T>
{
    // 构造函数
    vec() : x(T()), y(T()) {}
    vec(T X, T Y) : x(X), y(Y) {}

    // 构造函数
    template <class U>
    vec<2, T>(const vec<2, U>& v);

    // 重载
    T& operator[](const size_t i) { assert(i < 2); return i <= 0 ? x : y; }
    const T& operator[](const size_t i) const { assert(i < 2); return i <= 0 ? x : y; }

    T x, y;
};

// 定义有3个维度的，T类型的向量（可以用来表示坐标，向量）
template <typename T> 
struct vec<3, T>
{
    // 构造函数
    vec() : x(T()), y(T()), z(T()) {}
    vec(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
    template <class U>
    vec<3, T>(const vec<3, U>& v);

    // 重载
    T& operator[](const size_t i) { assert(i < 3); return i <= 0 ? x : (1 == i ? y : z); }
    const T& operator[](const size_t i) const { assert(i < 3); return i <= 0 ? x : (1 == i ? y : z); }
    
    // 求向量的模
    float norm() { return std::sqrt(x * x + y * y + z * z); }
    float norm2() { return (x * x + y * y + z * z); }

    // 归一化向量（l 是系数）
    vec<3, T>& normalize(T l = 1) { *this = (*this) * (l / norm()); return *this; }

    T x, y, z;
};

/*-------------------------------------------- 向量操作 -------------------------------------*/

// 向量点积（定义 DIM 维，T 类型 的 向量的点积）
template<size_t DIM, typename T> 
T operator*(const vec<DIM, T>& lhs, const vec<DIM, T>& rhs)
{
    T ret = T();
    for (size_t i = DIM; i--; ret += lhs[i] * rhs[i]);
    return ret;
}

// 向量加法
template<size_t DIM, typename T>
vec<DIM, T> operator+(vec<DIM, T> lhs, const vec<DIM, T>& rhs)
{
    for (size_t i = DIM; i--; lhs[i] += rhs[i]);
    return lhs;
}

// 向量减法
template<size_t DIM, typename T>
vec<DIM, T> operator-(vec<DIM, T> lhs, const vec<DIM, T>& rhs)
{
    for (size_t i = DIM; i--; lhs[i] -= rhs[i]);
    return lhs;
}

// 向量的数乘（DIM维T类型的向量 乘上 U类型的数值）
template<size_t DIM, typename T, typename U> 
vec<DIM, T> operator*(vec<DIM, T> lhs, const U& rhs)
{
    for (size_t i = DIM; i--; lhs[i] *= rhs);
    return lhs;
}

// 向量/数值（DIM维T类型的向量 除以 U类型的数值）
template<size_t DIM, typename T, typename U> 
vec<DIM, T> operator/(vec<DIM, T> lhs, const U& rhs)
{
    for (size_t i = DIM; i--; lhs[i] /= rhs);
    return lhs;
}

// 将一个低维向量嵌入到一个高维向量中（用 fill 填充剩下的元素）
template<size_t LEN, size_t DIM, typename T> 
vec<LEN, T> embed(const vec<DIM, T>& v, T fill = 1)
{
    vec<LEN, T> ret;
    for (size_t i = LEN; i--; ret[i] = (i < DIM ? v[i] : fill));
    return ret;
}

// 从一个高维向量中投影出一个低维向量
template<size_t LEN, size_t DIM, typename T> 
vec<LEN, T> proj(const vec<DIM, T>& v)
{
    vec<LEN, T> ret;
    for (size_t i = LEN; i--; ret[i] = v[i]);
    return ret;
}

// 向量的叉积（3维T类型的向量）
template <typename T> 
vec<3, T> cross(vec<3, T> v1, vec<3, T> v2)
{
    return vec<3, T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

// 输出向量
template <size_t DIM, typename T> 
std::ostream& operator<<(std::ostream& out, vec<DIM, T>& v)
{
    for (unsigned int i = 0; i < DIM; i++) 
    {
        out << v[i] << " ";
    }
    return out;
}

/////////////////////////////////////////////////////////////////////////////////

// 计算矩阵的行列式
template<size_t DIM, typename T> 
struct dt
{
    static T det(const mat<DIM, DIM, T>& src)
    {
        T ret = 0;
        for (size_t i = DIM; i--; ret += src[0][i] * src.cofactor(0, i));
        return ret;
    }
};

// 特化模板
template<typename T> 
struct dt<1, T>
{
    static T det(const mat<1, 1, T>& src)
    {
        return src[0][0];
    }
};

/////////////////////////////////////////////////////////////////////////////////

/*----------------------------------------- 矩阵 ----------------------------------------*/

// 定义矩阵（DimRows 行，DimCols 列，T 类型 的矩阵）
template<size_t DimRows, size_t DimCols, typename T> 
class mat
{
    // 用向量实现
    vec<DimCols, T> rows[DimRows];
public:
    mat() {}

    // 获取矩阵的 idx 行
    vec<DimCols, T>& operator[] (const size_t idx) 
    {
        assert(idx < DimRows);
        return rows[idx];
    }

    const vec<DimCols, T>& operator[] (const size_t idx) const 
    {
        assert(idx < DimRows);
        return rows[idx];
    }

    // 获取矩阵的 idx 列
    vec<DimRows, T> col(const size_t idx) const 
    {
        assert(idx < DimCols);
        vec<DimRows, T> ret;
        for (size_t i = DimRows; i--; ret[i] = rows[i][idx]);
        return ret;
    }

    // 设置矩阵的 idx 列
    void set_col(size_t idx, vec<DimRows, T> v)
    {
        assert(idx < DimCols);
        for (size_t i = DimRows; i--; rows[i][idx] = v[i]);
    }

    // 定义一个单位矩阵
    static mat<DimRows, DimCols, T> identity()
    {
        mat<DimRows, DimCols, T> ret;
        for (size_t i = DimRows; i--; )
            for (size_t j = DimCols; j--; ret[i][j] = (i == j));
        return ret;
    }

    // 求矩阵的行列式
    T det() const 
    {
        return dt<DimCols, T>::det(*this);
    }
    
    // 获取矩阵的子矩阵（Minor Matrix）子矩阵是从原始矩阵中删除特定行和列后得到的矩阵。
    mat<DimRows - 1, DimCols - 1, T> get_minor(size_t row, size_t col) const 
    {
        mat<DimRows - 1, DimCols - 1, T> ret;
        for (size_t i = DimRows - 1; i--; )
            for (size_t j = DimCols - 1; j--; ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1]);
        return ret;
    }

    // 用于计算矩阵的余子式（Cofactor）。余子式是通过将原始矩阵的特定行和列删除后得到的子矩阵的行列式乘以一个符号因子。
    T cofactor(size_t row, size_t col) const 
    {
        return get_minor(row, col).det() * ((row + col) % 2 ? -1 : 1);
    }

    // 这个函数是用于计算矩阵的伴随矩阵（Adjugate Matrix），也称为伴随矩阵或伴随阵。
    // 伴随矩阵是由矩阵的余子式组成的矩阵，其中每个元素对应于原始矩阵的某个位置的余子式。
    mat<DimRows, DimCols, T> adjugate() const
    {
        mat<DimRows, DimCols, T> ret;
        for (size_t i = DimRows; i--; )
            for (size_t j = DimCols; j--; ret[i][j] = cofactor(i, j));
        return ret;
    }

    // 这个函数是用于计算矩阵的转置逆矩阵（Inverse Transpose）。
    // 在这个函数中，首先调用adjugate函数计算原始矩阵的伴随矩阵，然后将伴随矩阵除以伴随矩阵第一行与原始矩阵第一行的点积的结果，得到转置逆矩阵。
    mat<DimRows, DimCols, T> invert_transpose() 
    {
        mat<DimRows, DimCols, T> ret = adjugate();
        T tmp = ret[0] * rows[0];
        return ret / tmp;
    }

    // 求矩阵的逆
    mat<DimRows, DimCols, T> invert() 
    {
        return invert_transpose().transpose();
    }

    // 求矩阵的转置
    mat<DimCols, DimRows, T> transpose() 
    {
        mat<DimCols, DimRows, T> ret;
        for (size_t i = DimCols; i--; ret[i] = this->col(i));
        return ret;
    }
};

/*------------------------------------------- 矩阵操作 --------------------------------------------*/

// 矩阵 向量 乘法
template<size_t DimRows, size_t DimCols, typename T> 
vec<DimRows, T> operator*(const mat<DimRows, DimCols, T>& lhs, const vec<DimCols, T>& rhs)
{
    vec<DimRows, T> ret;
    for (size_t i = DimRows; i--; ret[i] = lhs[i] * rhs);
    return ret;
}

// 矩阵乘法
template<size_t R1, size_t C1, size_t C2, typename T>
mat<R1, C2, T> operator*(const mat<R1, C1, T>& lhs, const mat<C1, C2, T>& rhs)
{
    mat<R1, C2, T> result;
    for (size_t i = R1; i--; )
        for (size_t j = C2; j--; result[i][j] = lhs[i] * rhs.col(j));
    return result;
}

// 矩阵 / 数
template<size_t DimRows, size_t DimCols, typename T>
mat<DimCols, DimRows, T> operator/(mat<DimRows, DimCols, T> lhs, const T& rhs)
{
    for (size_t i = DimRows; i--; lhs[i] = lhs[i] / rhs);
    return lhs;
}

// 输出矩阵
template <size_t DimRows, size_t DimCols, class T> 
std::ostream& operator<<(std::ostream& out, mat<DimRows, DimCols, T>& m)
{
    for (size_t i = 0; i < DimRows; i++) out << m[i] << std::endl;
    return out;
}

// 定义一些类型
typedef vec<2,  float> Vec2f;
typedef vec<2,  int>   Vec2i;
typedef vec<3,  float> Vec3f;
typedef vec<3,  int>   Vec3i;
typedef vec<4,  float> Vec4f;
typedef mat<4, 4, float> Matrix;

#endif //__GEOMETRY_H__


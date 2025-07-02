#pragma once
#pragma once
#include <cmath>
#include <cassert>
#include <iostream>

template<int n> 
struct vec_data {
    double data[n] = { 0 };
    double& operator[](const int i) { assert(i >= 0 && i < n); return data[i]; }
    double  operator[](const int i) const { assert(i >= 0 && i < n); return data[i]; }
};

template<> 
struct vec_data<2> {
    double x = 0, y = 0;
    double& operator[](const int i) { assert(i >= 0 && i < 2); return i ? y : x; }
    double  operator[](const int i) const { assert(i >= 0 && i < 2); return i ? y : x; };
};


template<>
struct vec_data<3> {
    double x = 0, y = 0, z = 0;
    double& operator[](const int i) { assert(i >= 0 && i < 3); return i ? (1 == i ? y : z) : x; }
    double  operator[](const int i) const { assert(i >= 0 && i < 3); return i ? (1 == i ? y : z) : x; }
};

template<int n>
struct vec : public vec_data<n>{
    // ����
    double length() const {
        double sum = 0;
        for (int i = 0; i < n; ++i) sum += (*this)[i] * (*this)[i];
        return std::sqrt(sum);
    }

    // ��һ��
    vec<n>& normalize() {
        double l = length();
        if (l == 0) return *this;
        for (int i = 0; i < n; ++i) (*this)[i] /= l;
        return *this;
    }

    // ���
    double dot(const vec<n>& other) const {
        double sum = 0;
        for (int i = 0; i < n; ++i) sum += (*this)[i] * other[i];
        return sum;
    }

    // �ӷ�
    vec<n> operator+(const vec<n>& other) const {
        vec<n> result;
        for (int i = 0; i < n; ++i) result[i] = (*this)[i] + other[i];
        return result;
    }

    // ����
    vec<n> operator-(const vec<n>& other) const {
        vec<n> result;
        for (int i = 0; i < n; ++i) result[i] = (*this)[i] - other[i];
        return result;
    }

    // ����
    vec<n> operator*(double scalar) const {
        vec<n> result;
        for (int i = 0; i < n; ++i) result[i] = (*this)[i] * scalar;
        return result;
    }
};

template<int n>
std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
    for (int i = 0; i < n; i++) out << v[i] << " ";
    return out;
}

typedef vec<2> vec2;
typedef vec<3> vec3;


inline vec3 cross(const vec3& a, const vec3& b) {
	return vec3{
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}
inline double cross(const vec2& a, const vec2& b) {
    return a.x * b.y - a.y * b.x;
}

#pragma once
#include <cmath>
#include <cassert>
#include <iostream>


//#pragma once
//#include <cmath>
//#include <cassert>
//#include <iostream>
//
//// ��γ�������࣬ ֧��2��3��4 ͨ��.h�����������
//template<int n> 
//struct vec_data {
//    double data[n] = { 0 };
//    vec(std::initializer_list<double> init) {
//        assert(init.size() == n);
//        int i = 0;
//        for (auto val : init) {
//            data[i++] = val;
//        }
//    }
//    double& operator[](const int i) { assert(i >= 0 && i < n); return data[i]; }
//    double  operator[](const int i) const { assert(i >= 0 && i < n); return data[i]; }
//};
//template<> 
//struct vec_data<2> {
//    double x = 0, y = 0;
//    double& operator[](const int i) { assert(i >= 0 && i < 2); return i ? y : x; }
//    double  operator[](const int i) const { assert(i >= 0 && i < 2); return i ? y : x; };
//};
//
//
//template<>
//struct vec_data<3> {
//    double x = 0, y = 0, z = 0;
//    double& operator[](const int i) { assert(i >= 0 && i < 3); return i ? (1 == i ? y : z) : x; }
//    double  operator[](const int i) const { assert(i >= 0 && i < 3); return i ? (1 == i ? y : z) : x; }
//};
//template<>
//struct vec_data<4>{
//    double x = 0, y = 0, z = 0, w = 1;
//    double& operator[](const int i) { assert(i >= 0 && i < 4); return i ? (2 == i ? z : (3 == i ? w : y)) : x; }
//    double  operator[](const int i) const { assert(i >= 0 && i < 4); return i ? (2 == i ? z : (3 == i ? w : y)) : x; }
//};
//template<int n>
//struct vec : public vec_data<n>{
//    // ����
//    double length() const {
//        double sum = 0;
//        for (int i = 0; i < n; ++i) sum += (*this)[i] * (*this)[i];
//        return std::sqrt(sum);
//    }
//
//    // ��һ��
//    vec<n>& normalize() {
//        double l = length();
//        if (l == 0) return *this;
//        for (int i = 0; i < n; ++i) (*this)[i] /= l;
//        return *this;
//    }
//
//    // ���
//    double dot(const vec<n>& other) const {
//        double sum = 0;
//        for (int i = 0; i < n; ++i) sum += (*this)[i] * other[i];
//        return sum;
//    }
//
//    // �ӷ�
//    vec<n> operator+(const vec<n>& other) const {
//        vec<n> result;
//        for (int i = 0; i < n; ++i) result[i] = (*this)[i] + other[i];
//        return result;
//    }
//
//    // ����
//    vec<n> operator-(const vec<n>& other) const {
//        vec<n> result;
//        for (int i = 0; i < n; ++i) result[i] = (*this)[i] - other[i];
//        return result;
//    }
//
//    // ����
//    vec<n> operator*(double scalar) const {
//        vec<n> result;
//        for (int i = 0; i < n; ++i) result[i] = (*this)[i] * scalar;
//        return result;
//    }
//};
//
//template<int n>
//std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
//    for (int i = 0; i < n; i++) out << v[i] << " ";
//    return out;
//}
//
//typedef vec<2> vec2;
//typedef vec<3> vec3;
//typedef vec<4> vec4;
//
//double cross(const vec2& a, const vec2& b) {
//    return      a.x * b.y - a.y * b.x;
//}
//vec3 cross(const vec3& a, const vec3& b) {
//    return vec3(a.y * b.z - a.z * b.y, 
//                a.z * b.x - a.x * b.z, 
//                a.x * b.y - a.y * b.x);
//}
//
//
//
//
//



//
// 1. vec_data �ػ��ṹ��ֻ����洢�� [] ����
//
template<int n>
struct vec_data {
    double data[n] = { 0 };
    double& operator[](int i) { assert(i >= 0 && i < n); return data[i]; }
    double  operator[](int i) const { assert(i >= 0 && i < n); return data[i]; }
};

template<>
struct vec_data<2> {
    double x = 0, y = 0;
    double& operator[](int i) { assert(i >= 0 && i < 2); return i ? y : x; }
    double  operator[](int i) const { assert(i >= 0 && i < 2); return i ? y : x; }
};

template<>
struct vec_data<3> {
    double x = 0, y = 0, z = 0;
    double& operator[](int i) { assert(i >= 0 && i < 3); return i == 0 ? x : (i == 1 ? y : z); }
    double  operator[](int i) const { assert(i >= 0 && i < 3); return i == 0 ? x : (i == 1 ? y : z); }
};

//
// 2. ͨ�ù��ܻ��ࣨʹ�� CRTP ʵ�֣�
//
template<int n, typename Derived>
struct vec_base : public vec_data<n> {
    double length() const {
        double sum = 0;
        for (int i = 0; i < n; ++i) sum += (*this)[i] * (*this)[i];
        return std::sqrt(sum);
    }

    Derived& normalize() {
        double l = length();
        if (l != 0) {
            for (int i = 0; i < n; ++i) (*this)[i] /= l;
        }
        return static_cast<Derived&>(*this);
    }

    double dot(const Derived& other) const {
        double sum = 0;
        for (int i = 0; i < n; ++i) sum += (*this)[i] * other[i];
        return sum;
    }

    Derived operator+(const Derived& other) const {
        Derived res;
        for (int i = 0; i < n; ++i) res[i] = (*this)[i] + other[i];
        return res;
    }

    Derived operator-(const Derived& other) const {
        Derived res;
        for (int i = 0; i < n; ++i) res[i] = (*this)[i] - other[i];
        return res;
    }

    Derived operator*(double s) const {
        Derived res;
        for (int i = 0; i < n; ++i) res[i] = (*this)[i] * s;
        return res;
    }
};


template<int n>
struct vec : public vec_base<n, vec<n>> {

};

// 4. �ػ����캯��ֻΪ vec<2> / vec<3> ���
template<>
struct vec<2> : public vec_base<2, vec<2>> {
    vec() = default;
    vec(double x_, double y_) { x = x_; y = y_; }
};

template<>
struct vec<3> : public vec_base<3, vec<3>> {
    vec() = default;
    vec(double x_, double y_, double z_) { x = x_; y = y_; z = z_; }
};



template<int n>
std::ostream& operator<<(std::ostream& os, const vec<n>& v) {
    for (int i = 0; i < n; ++i) os << v[i] << " ";
    return os;
}
using vec2 = vec<2>;
using vec3 = vec<3>;


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

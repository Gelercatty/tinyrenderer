#pragma once
#include <cmath>
#include <cassert>
#include <iostream>
#include <numbers>
#include <cmath>

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

template<>
struct vec<2> : public vec_base<2, vec<2>> {
    vec() = default;
    vec(double x_, double y_) { x = x_; y = y_; }
	vec(int x_, int y_) {
		this->x = static_cast<double>(x_);
		this->y = static_cast<double>(y_);
	}
};

template<>
struct vec<3> : public vec_base<3, vec<3>> {
    vec() = default;
    vec(double x_, double y_, double z_) { x = x_; y = y_; z = z_; }
	vec(int x_, int y_, int z_) {
		this->x = static_cast<double>(x_);
		this->y = static_cast<double>(y_);
		this->z = static_cast<double>(z_);
	}
};



template<int n>
std::ostream& operator<<(std::ostream& os, const vec<n>& v) {
	os << "[";
    for (int i = 0; i < n; ++i) os << v[i] << " ";
	os << "]";
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

// Matrix
template<int m, int n> 
struct mat
{
	double data[m][n] = { {0} };
	mat() = default;
    // 初始化列表构造函数（必须加 explicit，避免模板歧义）
    mat(std::initializer_list<std::initializer_list<double>> values) {
        int i = 0;
        for (auto row : values) {
            int j = 0;
            for (auto val : row) {
                if (i < m && j < n) {
                    data[i][j] = val;
                }
                ++j;
            }
            ++i;
        }
    }

   double& operator()(int i, int j) {
		assert(i >= 0 && i < m && j >= 0 && j < n);
		return data[i][j];
	}
	const double& operator()(int i, int j) const {
		assert(i >= 0 && i < m && j >= 0 && j < n);
		return data[i][j];
	}
    vec2 shape() {
        return vec2{ static_cast<double>(m), static_cast<double>(n) };
    };
	std::pair<int, int> shape() const {
		return { m, n };
	}
};

template<int m, int k, int n>
mat<m, n> operator*(const mat<m, k>& A, const mat<k, n>& B) {
    mat<m, n> res;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            res(i, j) = 0;
            for (int t = 0; t < k; ++t) {
                res(i, j) += A(i, t) * B(t, j);
            }
        } 
    }
    return res;
}

template<int n>
mat<n, 1> to_col_matrix(const vec<n>& v) {
    mat<n, 1> m;
    for (int i = 0; i < n; ++i) {
        m(i, 0) = v[i];
    }
    return m;
}

template<int n>
mat<1, n> to_row_matrix(const vec<n>& v) {
    mat<1, n> m;
    for (int i = 0; i < n; ++i) {
        m(0, i) = v[i];
    }
    return m;
}

template<int n>
vec<n> to_vec(const mat<n, 1>& m) {
    vec<n> v;
    for (int i = 0; i < n; ++i) {
        v[i] = m(i, 0);
    }
    return v;
}

template<int n>
vec<n> to_vec(const mat<1, n>& m) {
    vec<n> v;
    for (int i = 0; i < n; ++i) {
        v[i] = m(0, i);
    }
    return v;
}

template<int m, int n>
vec<m> operator*(const mat<m, n>& M, const vec<n>& v) {
    vec<m> result;
    for (int i = 0; i < m; ++i) {
        result[i] = 0;
        for (int j = 0; j < n; ++j) {
            result[i] += M(i, j) * v[j];
        }
    }
    return result;
}
template<int m, int n>
vec<n> operator*(const vec<m>& v, const mat<m, n>& M) {
    vec<n> result;
    for (int j = 0; j < n; ++j) {
        result[j] = 0;
        for (int i = 0; i < m; ++i) {
            result[j] += v[i] * M(i, j);
        }
    }
    return result;
}






/// 旋转
/// @param v 输入向量
/// @param angles 沿着x, y, z轴的旋转角度,单位为度
/// @return 旋转后的向量
/// 旋转顺序 x y z
vec3 rotate(const vec3& v, const vec3& angles) {
	// Convert angles from degrees to radians
	double rx = angles.x * std::numbers::pi / 180.0;
	double ry = angles.y * std::numbers::pi / 180.0;
	double rz = angles.z * std::numbers::pi / 180.0;
	// Rotation matrices around x, y, z axes
	mat<3, 3> Rx = {
		{1, 0, 0},
		{0, std::cos(rx), -std::sin(rx)},
		{0, std::sin(rx), std::cos(rx)}
	};
	mat<3, 3> Ry = {
		{std::cos(ry), 0, std::sin(ry)},
		{   0,         1,       0    },
		{-std::sin(ry), 0, std::cos(ry)}
	};
	mat<3, 3> Rz = {
		{std::cos(rz), -std::sin(rz), 0},
		{std::sin(rz), std::cos(rz), 0},
		{0, 0, 1}
	};
	// Combined rotation matrix
	mat<3, 3> R = Rx * Ry * Rz;
	// Rotate the vector
	return R * v;
}


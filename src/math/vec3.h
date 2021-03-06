#ifndef VEC3_H
#define VEC3_H

class Vec3 {
  public:
    Vec3(double x, double y, double z) : m_x(x),m_y(y),m_z(z) {};
    Vec3(Vec3 const &avec);
    double dot(Vec3 v);
    Vec3 operator-(const Vec3& other);
    Vec3 operator+(const Vec3& other);
    Vec3 multiply(const Vec3& other);

    #ifdef DEBUG
    void debug();
    #endif

    void normalize();
    double m_x, m_y, m_z;
};

#endif

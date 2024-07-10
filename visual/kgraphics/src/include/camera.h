#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>

#include <QVector3D>
#include <QMatrix4x4>

class TCamera
{
public:
    static constexpr double RAD = M_PI / 180.;
    static constexpr double DEG = 180. / M_PI;
    static constexpr double BASE_FoV = 26.;

    static double clip( double v )
    {
        return v < 0 ? (v + 360.) : (v > 360. ? v - 360. : v);
    }

    TCamera( const QVector3D &camPos, int width, int height );

    void setPosition( float x, float y, float z );
    void setRotation( float x, float y, float z );

    void normalize();
    void move( float width, float height );
    void rotate( float x_deg, float y_deg );
    void zoom( float factor );

    const QMatrix4x4 view() const
    {
        return view_;
    }

    const QMatrix4x4 projection() const
    {
        return projection_;
    }

    const QVector3D &rotation() const
    {
        return rotation_deg_;
    }

    float fieldOfView() const
    {
        return FoV_;
    }
private:
    QVector3D position_ = QVector3D(0.f, 0.5f,  3.0f);
    QVector3D front_ = QVector3D(0.0f, 0.0f,  0.0f);
    QVector3D up_ = QVector3D(0.0f, 1.0f,  0.0f);
    QMatrix4x4 view_;
    QMatrix4x4 projection_;

    QVector3D rotation_deg_ = QVector3D(0.f, 0.f, 0.f);
    float FoV_ { BASE_FoV };
    float sense_ = 1./100.;
};

#endif // CAMERA_H

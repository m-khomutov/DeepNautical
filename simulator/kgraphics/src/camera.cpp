#include "camera.h"

TCamera::TCamera( const QVector3D &pos, int width, int height )
    : position_( pos )
{
    projection_.setToIdentity();
    projection_.perspective( FoV_, width / height, 0.1f, 10000.0f );

    view_.setToIdentity();
    view_.lookAt( position_, front_, up_ );
}

void TCamera::setPosition( float x, float y, float z )
{
    position_.setX( x );
    position_.setY( y );
    position_.setZ( z );
}

void TCamera::setRotation( float x, float y, float z )
{
    rotation_deg_.setX( x );
    rotation_deg_.setY( y );
    rotation_deg_.setZ( z );
}

void TCamera::normalize()
{
    setRotation( clip(rotation_deg_.x()),
                 clip(rotation_deg_.y()),
                      rotation_deg_.z() );
}

void TCamera::move( float width, float height )
{
    projection_.setToIdentity();
    projection_.perspective( FoV_, width / height, 0.1f, 10000.0f );

    front_.setX( ::cosf( TCamera::RAD * rotation_deg_.x() ) * ::sinf( TCamera::RAD * rotation_deg_.y() ) );
    front_.setY( ::sinf( TCamera::RAD * rotation_deg_.x() ) );
    front_.setZ( ::cosf( TCamera::RAD * rotation_deg_.x() ) * ::cosf( TCamera::RAD * rotation_deg_.y() ) );
    //front_.setY( ::sinf( TCamera::RAD * rotation_deg_.x() ) * ::sinf( TCamera::RAD * rotation_deg_.y() ) );
    //front_.setZ( ::cosf( TCamera::RAD * rotation_deg_.y() ) );

    view_.setToIdentity();
    view_.lookAt( position_, front_, up_ );
}

void TCamera::rotate( float x_deg, float y_deg )
{
    float deg = rotation_deg_.x() + x_deg;
    rotation_deg_.setX( deg - std::ceil(deg / 360.f) * 360.f );

    deg = rotation_deg_.y() + y_deg;
    rotation_deg_.setY( deg - std::ceil(deg / 360.f) * 360.f );
}

void TCamera::zoom( float factor )
{
    FoV_ = std::max( 2., std::min( BASE_FoV / factor, 90. ) );
}

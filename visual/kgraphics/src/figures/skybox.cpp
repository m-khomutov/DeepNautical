#include "figures/skybox.h"

TSkybox::TSkybox( const NJson::TObject &environment,const NJson::TObject &settings )
: TFigure( environment, settings )
{}

void TSkybox::f_check_environment() const
{
    if( ! (NUtils::file_exists( (std::string(NUtils::TConfig()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["shaders"]) + "/frag_" + spec_.shader_name).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + "}"  );
    }
}

char const *TSkybox::f_shader_name() const
{
    return spec_.shader_name.c_str();
}

void TSkybox::f_initialize()
{
    vertex_buffer_object_.allocate( position_, sizeof(position_) );

    shader_program_.enableAttributeArray( "position" );
    shader_program_.setAttributeBuffer( "position", GL_FLOAT, 0, 3, 3 * sizeof(GLfloat) );

    f_create_face( TSpecification::ECubeFaces::RIGHT, GL_TEXTURE_CUBE_MAP_POSITIVE_X );
    f_create_face( TSpecification::ECubeFaces::LEFT, GL_TEXTURE_CUBE_MAP_NEGATIVE_X );
    f_create_face( TSpecification::ECubeFaces::TOP, GL_TEXTURE_CUBE_MAP_POSITIVE_Y );
    f_create_face( TSpecification::ECubeFaces::BOTTOM, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y );
    f_create_face( TSpecification::ECubeFaces::FRONT, GL_TEXTURE_CUBE_MAP_POSITIVE_Z );
    f_create_face( TSpecification::ECubeFaces::BACK, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z );
}

void TSkybox::draw()
{
    skybox_->bind();
    shader_program_.setUniformValue( "skybox", GLuint(0) );

//    glDisable(GL_DEPTH);
    glDepthMask(GL_FALSE);
    shader_program_.setUniformValue( "Offset", offset_ );

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);
}

void TSkybox::f_accept( IVisitor &p, double currentTime )
{
    shader_program_.setUniformValue( "Time", (float)((int64_t)currentTime%1000000/1000000.));
    // передать посетителю указатель на себя
    p.visit( this );
}

void TSkybox::f_create_face( TSpecification::ECubeFaces cube_face, GLenum face )
{
    std::string path = std::string(NUtils::TConfig()["textures"]) + "/" + spec_.cube_faces[cube_face];
    QImage img = QImage(path.c_str()).convertToFormat(QImage::Format_RGB888);

    if( !skybox_ )
    {
        skybox_.reset( new QOpenGLTexture( QOpenGLTexture::TargetCubeMap ) );
        skybox_->create();
        skybox_->bind();
        skybox_->setSize( img.width(), img.height(), img.depth());
        skybox_->setFormat(QOpenGLTexture::RGBFormat);
        skybox_->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
        skybox_->setWrapMode(QOpenGLTexture::ClampToEdge);
        skybox_->allocateStorage(QOpenGLTexture::RGB, QOpenGLTexture::UInt8);
    }

    glTexImage2D( face,
                  0,
                  GL_RGB,
                  img.width(),
                  img.height(),
                  0,
                  GL_RGB,
                  GL_UNSIGNED_BYTE,
                  img.bits() );
}

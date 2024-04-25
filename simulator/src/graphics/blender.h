/* 
 * File:   blenderobj.h
 * Author: mkh
 *
 * Created on 1 марта 2023 г., 9:50
 */

#ifndef BLENDER_H
#define BLENDER_H

#include "texture/jpeg.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace blender
{

class mtlfile {
public:
    using face_t = glm::vec< 3, glm::ivec3 >; 
    struct material {
        std::string name;

        float Ns;
        glm::vec3 Ka;
        glm::vec3 Kd;
        glm::vec3 Ks;
        glm::vec3 Ke;
        float Ni;
        float d;
        int illum;
        std::shared_ptr< TJpegTexture > map_Kd;

        std::vector< face_t > faces;
    };
    using material_t = std::map< std::string, material >;
    
    mtlfile( char const *filename );
    mtlfile(const mtlfile &orig) = delete;
    mtlfile operator =(const mtlfile &orig) = delete;
    ~mtlfile() = default;

    material const &operator []( std::string const &name ) const;
    
private:
    material_t materials_;
};

class object {
public:
    explicit object( char const *fname );
    object(const object& orig) = delete;
    object &operator =(const object& orig) = delete;
    ~object() = default;

    void load_position( std::vector< float > *pos );
    size_t facecount() const
    {
        return facecount_;
    }
    const  std::vector< mtlfile::material > &materials() const
    {
        return materials_;
    }
    
private:
    std::vector< glm::vec3 > vertices_;
    std::vector< glm::vec2 > texels_;
    std::vector< glm::vec3 > normals_;
    std::vector< mtlfile::material > materials_;
    size_t facecount_ { 0 };
};

}  // namespace blender

#endif /* BLENDER_H */

/* 
 * File:   vessel.h
 * Author: mkh
 *
 * Created on 16 февраля 2023 г., 13:11
 */

#ifndef VESSEL_H
#define VESSEL_H

#include "figure.h"

class vessel: public figure {
public:
    static bool environment_valid();

    vessel();
    vessel(const vessel& orig) = delete;
    vessel &operator =(const vessel& orig) = delete;
    ~vessel();

private:
    std::vector< GLfloat > position_;

private:
    char const *f_shader_name() const override;
    void f_initialize() override;
    void f_draw( double currentTime ) override;
    void f_set_model();
};

#endif /* VESSEL_H */

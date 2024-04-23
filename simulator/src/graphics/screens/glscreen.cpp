
/* 
 * File:   openglscreen.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 14:25
 */

#include "glscreen.h"

TGLscreenError::TGLscreenError( const std::string &what )
: std::runtime_error( what )
{}

TGLscreen::TGLscreen( TBaseframe *frame )
: TBasescreen( frame )
{
    // получить список сцен
    NUtils::read_directory( NUtils::TConfig()["scenes"],
                           ".scn",
                           [this]( const std::string &name )
                           {
                               scenes_.insert( name.substr( 0, name.size() - 4 ) );
                           } );
    if( scenes_.empty() )
        throw TGLscreenError( std::string("no scenes found") );

    //настроить итератор на начало списка
    scene_iter_ = scenes_.begin();
}

// сделать список текущих сцен
std::vector< std::string > TGLscreen::current_scenes()
{
    std::vector< std::string > ret( sc_.size() );
    for( size_t i(0); i < sc_.size();++i )
    {
        ret[i] = sc_[i]->name();
    }
    return ret;
}
// запускается при получении команды от абонента из сети
void TGLscreen::set_scene( const std::string &scene_name, size_t position )
{
    // проверить, может хотят уже используемую сцену
    for( const auto &scene : sc_ )
    {
        if( scene->name() == scene_name )
        {
            return;
        }
    }
    // проверить наличие запрвшиваемого места
    if( position >= sc_.size() )
    {
        throw TGLscreenError( std::string( "no such position " ) + std::to_string( position ) );
    }
    // проверить, есть ли сцена с таким именем в наличии
    std::set< std::string>::iterator it = scenes_.find( scene_name );
    if( it == scenes_.end() )
    {
        throw TGLscreenError( scene_name + " not found" );
    }
    // настроить итератор на желаемую сцену
    scene_iter_ = it;

    // передать команду экрану на исполнение через очередь команд
    auto g = commands_.value_guard();
    g->emplace_back( TScreenCommand( TScreenCommand::kSetScene, position ) );
}

 // периодическая проверка наличия команд в очереди
void TGLscreen::f_exec_command()
{
    auto g = commands_.value_guard();
    if( !g->empty() )
    {
        // есть команда, ваыполняем
        TScreenCommand cmd = g->front();
        g->pop_front();
        switch( cmd.type() )
        {
        case TScreenCommand::kSetScene:
            if( cmd.position() < sc_.size() )
            {
                sc_[cmd.position()].reset( new scene( *scene_iter_, std::string(NUtils::TConfig()["scenes"]) + "/" + *scene_iter_ + ".scn" ) );
            }
            break;
        }
    }
}

// сохранить текущие кадры с текущих сцен
void TGLscreen::f_store_scene_frame()
{
    int q;
    glGetIntegerv( GL_READ_BUFFER, &q );
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    for( size_t v(0); v < view_count_; ++v )
    {
        // получить указатель на буфер кадра. Явное нарушение инкапсуляции. На что не пойдешь ради оптимизации
        uint8_t *buffer = frame_->buffer( v, frame_->width(), frame_->height() );
        // прочесть в буфер текущий кадр
        glReadPixels( v * frame_->width(), 0, frame_->width(), frame_->height(), GL_RGB, GL_UNSIGNED_BYTE, buffer );
        // подготовить (сжать) буфер для последующей передачи
        frame_->prepare_buffer( v );
    }
}

// скомандовать объекту протокола передать кадр абонентам
bool TGLscreen::f_send_stored_scene_frame( TBaseprotocol *proto )
{
   return frame_->send_buffer( proto );
}

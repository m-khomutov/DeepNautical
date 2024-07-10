#ifndef SCREEN_COMMAND_H
#define SCREEN_COMMAND_H

#include <type_traits>

/*!
   \class TScreenCommand
   \brief Класс команды, передаваемой экрану
 */
class TScreenCommand {
public:
    //! Типы передаваемых команд
    enum EScreenCommandType { kZoom, kRotate, kChangeCamera, kChangeCameraMode, kChangePanoramaMode, kMouseMoveX, kMouseMoveY };

    /*!
       \brief Конструктор класса. Настраивает тип создаваемой команды
       \param t тип создаваемой команды
       \param ivalue целочисленное значение
     */
    TScreenCommand( EScreenCommandType t, int value )
    : t_( t )
    , ivalue_( value )
    {}

    /*!
       \brief Конструктор класса. Настраивает тип создаваемой команды
       \param t тип создаваемой команды
       \param dvalue вещественное значение
     */
    TScreenCommand( EScreenCommandType t, double value )
    : t_( t )
    , dvalue_( value )
    {}

    /*!
       \brief Возвращает тип команды
       \return тип команды
     */
    EScreenCommandType type() const
    {
        return t_;
    }
    /*!
       \brief Возвращает значение
     */
    template< typename T >
    T value() const
    {
        if( std::is_floating_point<T>::value )
        {
            return dvalue_;
        }
        return ivalue_;
    }

private:
    //! тип команды
    EScreenCommandType t_;
    //! целочисленное значение
    int ivalue_;
    //! вещественное значение
    double dvalue_;
};

#endif // SCREEN_COMMAND_H

#include "labeleventfilter.h"
#include <QMouseEvent>

auto LabelEventFilter::eventFilter(QObject * /*watched*/, QEvent *event) -> bool
{
    if ( event->type() == QEvent::MouseMove ){
        const QMouseEvent* const me = dynamic_cast<const QMouseEvent*>( event );
        const QPoint p = me->pos();
        emit moved(p);
        return true;
    }

    if ( event->type() == QEvent::MouseButtonPress ){
        const QMouseEvent* const me = dynamic_cast<const QMouseEvent*>( event );
        const QPoint p = me->pos();
        emit clicked(p);
        return true;
    }

    return false;
}

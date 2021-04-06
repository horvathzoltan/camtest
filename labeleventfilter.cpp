#include "labeleventfilter.h"
#include <QMouseEvent>

auto LabelEventFilter::eventFilter(QObject * /*watched*/, QEvent *event) -> bool
{
    if ( event->type() != QEvent::MouseButtonPress ) return false;
    const QMouseEvent* const me = dynamic_cast<const QMouseEvent*>( event );
    //might want to check the buttons here
    const QPoint p = me->pos();
    emit clicked(p);
    return false;
}

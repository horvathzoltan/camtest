#ifndef LABELEVENTFILTER_H
#define LABELEVENTFILTER_H

#include <QObject>

class LabelEventFilter : public QObject
{
    Q_OBJECT
public:
    LabelEventFilter() = default;
    bool eventFilter(QObject *watched, QEvent *event);
signals:
    void clicked(const QPoint&);
    void moved(const QPoint&);
};

#endif // LABELEVENTFILTER_H

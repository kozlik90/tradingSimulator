#ifndef CUSTOMSCROLLAREA_H
#define CUSTOMSCROLLAREA_H

#include <QScrollArea>

class CustomScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    CustomScrollArea(QWidget*);
protected:
    void wheelEvent(QWheelEvent* event) override;
};

#endif // CUSTOMSCROLLAREA_H

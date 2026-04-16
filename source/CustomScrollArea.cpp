#include "includes/CustomScrollArea.h"
#include "qevent.h"


CustomScrollArea::CustomScrollArea(QWidget * parrent) : QScrollArea(parrent)
{

}

void CustomScrollArea::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}

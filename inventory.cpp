#include <QLayout>
#include <QDebug>
#include <QHeaderView>
#include <QDragEnterEvent>
#include "inventory.h"

Inventory::Inventory(uint size/* = 3*/, QWidget *parent/* = nullptr*/)
    : m_Size(size), QTableWidget(parent) {
    setColumnCount(m_Size);
    setRowCount(m_Size);
    for (uint i = 0; i < m_Size; ++i) {
        insertRow(i);
    }
    setAcceptDrops(true);

    createFormInterior();
}

void Inventory::createFormInterior() {
    qDebug() << "Inventory";
    horizontalHeader()->setCascadingSectionResizes(false);
    verticalHeader()->setCascadingSectionResizes(false);
    horizontalHeader()->hide();
    verticalHeader()->hide();

    verticalHeader()->setMinimumSectionSize(250);
    verticalHeader()->setMaximumSectionSize(250);
    verticalHeader()->setDefaultSectionSize(250);
    horizontalHeader()->setMinimumSectionSize(250);
    horizontalHeader()->setMaximumSectionSize(250);
    horizontalHeader()->setDefaultSectionSize(250);

    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    verticalHeader()->setSizePolicy(sizePolicy);
    horizontalHeader()->setSizePolicy(sizePolicy);

    setMinimumSize(250 * m_Size, 250 * m_Size);
    setMaximumSize(250 * m_Size, 250 * m_Size);
    setSizePolicy(sizePolicy);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

/*virtual*/ void Inventory::dragEnterEvent(QDragEnterEvent *event) /*override*/ {
    if (event->mimeData()->hasFormat("application/x-item"))
        event->acceptProposedAction();
}

/*virtual*/ void Inventory::dropEvent(QDropEvent *event) /*override*/ {

}

uint Inventory::size() const {
    return m_Size;
}

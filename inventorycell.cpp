#include "database.h"
#include "inventorycell.h"

// конструктор по-умолчанию, вечный генератор яблок
InventoryCell::InventoryCell(bool isSource/* = true*/, QWidget* parent/* = nullptr*/)
	: QWidget(parent), m_Number(1), m_isSource(isSource), m_Content(nullptr) {
	if (!m_isSource) {
		setAcceptDrops(true);
		m_State = State::Empty;
	}
	else {
		m_Content = Database::itemSelect("apple");
		m_State = State::Fill;
	}
}

// конструктор для создания заполненной ячейки, в итоге не успел пригодится,
// но должен был служить для заполнения инвентаря из БД
InventoryCell::InventoryCell(int row, int col, int number,
							 Item* item, bool isSource/* = false*/,
							 QWidget* parent/* = nullptr*/)
	: QWidget(parent), m_Row(row), m_Col(col), m_Number(number),
	  m_isSource(isSource), m_Content(item) {
	if (!m_isSource)
		setAcceptDrops(true);
	view();
	m_State = State::Fill;
}

// для заполнения инвентаря пустыми ячейками, строки и столбцы -
// для внесения в БД местоположения ячейки вместе с её содержимым и его количеством
InventoryCell::InventoryCell(int row, int col, QWidget* parent/* = nullptr*/)
	: QWidget(parent), m_Row(row), m_Col(col), m_Number(0), m_isSource(false), m_Content(nullptr) {
	setAcceptDrops(true);
	m_State = State::Empty;
}

// слот для совершения действия над предметом в ячейке,
// количество предметов при действии уменьшается, если их нет, то ячейка очищается
void InventoryCell::actionWithItem() {
	m_SoundEffect.setSource(QUrl::fromLocalFile(m_Content->soundPath()));
	m_SoundEffect.play();
	--m_Number;
	updateNumberText();
	if (m_Number == 0) {
		clearCell();
	}
}

// на зажатие левой кнопки - вычисление позиции m_DragStart, на правую - действия с предметом (поедание яблок)
/*virtual*/ void InventoryCell::mousePressEvent(QMouseEvent *event) /*override*/ {
	if (m_State != State::Empty) {
		if (event->button() == Qt::LeftButton) {
			m_DragStart = event->pos();
		} else if (!m_isSource && event->button() == Qt::RightButton) {
			actionWithItem();
		}
	}
	QWidget::mousePressEvent(event);
}

// обработка перетаскивания предмета из ячейки, из источника - копируем, из обычной ячейки - переносим
/*virtual*/ void InventoryCell::mouseMoveEvent(QMouseEvent *event) /*override*/ {
	if (m_State != State::Empty && event->buttons() & Qt::LeftButton) {
		int distance = (event->pos() - m_DragStart).manhattanLength();
		if (distance > QApplication::startDragDistance()) {
			QByteArray data;
			QDataStream dataStream(&data, QIODevice::WriteOnly);
			QMimeData* mimeData = new QMimeData;

			dataStream << *this;
			mimeData->setData(m_Content->mimeType(), data);

			QDrag* drag = new QDrag(this);
			drag->setMimeData((mimeData));
			drag->setPixmap(*(m_Content->pixmap()));

			if (m_isSource)
				drag->exec(Qt::CopyAction);
			else {
				drag->exec(Qt::MoveAction);
			}
		}
	}
	QWidget::mouseMoveEvent(event);
}

// При входе в зону сброса предмета, проверяем mime-type, должен соответствовать специальному типу предмета
/*virtual*/ void InventoryCell::dragEnterEvent(QDragEnterEvent* event) /*override*/ {
	if (event->mimeData()->hasFormat(Item::mimeType())) {
		event->acceptProposedAction();
	}
}

// "Принятие" предмета ячейкой и его отображение вместе с количеством
/*virtual*/ void InventoryCell::dropEvent(QDropEvent* event) /*override*/ {
	if (event->mimeData()->hasFormat(Item::mimeType())) {
		InventoryCell* source =
				dynamic_cast<InventoryCell*>(event->source());
		if (source == this) {
			return;
		}
		QByteArray data = event->mimeData()->data(Item::mimeType());
		QDataStream dataStream(&data, QIODevice::ReadOnly);

		InventoryCell tempCell;
		dataStream >> tempCell;


		if (source->m_isSource == false)
			source->clearCell();
		if (m_Number == 0) {
			m_Content = tempCell.m_Content;
			m_Number += tempCell.m_Number;
			view();
			m_State = State::Fill;
		} else {
			m_Number += tempCell.m_Number;
			updateNumberText();
		}
	}
}

// компоновка виджета предмета и виджета отображения количества предметов, для источника нет надписи с количеством
void InventoryCell::view() {
	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(m_Content);
	if (!m_isSource) {
		m_NumberText = new QLabel(QString::number(m_Number));
		m_NumberText->setStyleSheet("color: black");
		layout->addWidget(m_NumberText, 0, Qt::AlignBottom | Qt::AlignRight);
	}
	setLayout(layout);
}

// "Опустошение" ячейки от её содержимого и очистка компоновки
void InventoryCell::clearCell() {
	m_NumberText->clear();
	m_Content->clear();
	delete layout();
	m_Number = 0;
	m_State = State::Empty;
}

// Обновление виджета отображения количества элементов
void InventoryCell::updateNumberText() {
	m_NumberText->setText(QString::number(m_Number));
}

QDataStream& operator<<(QDataStream& stream, const InventoryCell& cell) {
	stream << cell.m_Row << cell.m_Col << cell.m_Number << cell.m_Content << cell.m_isSource;
	return stream;
}

QDataStream& operator>>(QDataStream& stream, InventoryCell& cell) {
	stream >> cell.m_Row >> cell.m_Col >> cell.m_Number >> *(cell.m_Content) >> cell.m_isSource;
	return stream;
}

int InventoryCell::number() const {
	return m_Number;
}

Item* InventoryCell::content() const {
	return m_Content;
}

InventoryCell::State InventoryCell::state() const {
	return m_State;
}

int InventoryCell::col() const {
	return m_Col;
}

int InventoryCell::row() const {
	return m_Row;
}

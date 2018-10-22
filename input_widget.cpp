#include "input_widget.hpp"

InputWidget::InputWidget(QWidget* parent): QPlainTextEdit(parent) {}

#include <QDebug>
void InputWidget::keyPressEvent(QKeyEvent* e) {

	// if the user pressed Shift+Enter, then we need to execute what they typed and clear the screen
	if ((e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) &&
		e->modifiers() & Qt::ShiftModifier) {

		// TODO: Connect the user's input to a signal
		qDebug() << toPlainText();
		clear();
	} else {

		// otherwise, proceed with the normal event handler
		QPlainTextEdit::keyPressEvent(e);
	}
}

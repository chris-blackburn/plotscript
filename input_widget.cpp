#include "input_widget.hpp"

InputWidget::InputWidget(QWidget* parent): QPlainTextEdit(parent) {}

void InputWidget::keyPressEvent(QKeyEvent* e) {

	// if the user pressed Shift+Enter,
	if ((e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) &&
		(e->modifiers() & Qt::ShiftModifier)) {

		// publish the text in the text box and clear it
		publish(toPlainText());
		clear();
	} else {

		// otherwise, proceed with the normal event handler
		QPlainTextEdit::keyPressEvent(e);
	}
}

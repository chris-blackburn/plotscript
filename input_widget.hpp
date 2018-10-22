#ifndef INPUT_WIDGET_HPP
#define INPUT_WIDGET_HPP

#include <QPlainTextEdit>

class InputWidget: public QPlainTextEdit {
Q_OBJECT
public:
	InputWidget(QWidget* parent = nullptr);

	void keyPressEvent(QKeyEvent* e) override;
};

#endif

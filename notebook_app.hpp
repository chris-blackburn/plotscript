#ifndef NOTEBOOK_APP_HPP
#define NOTEBOOK_APP_HPP

#include <QWidget>

#include "input_widget.hpp"
#include "output_widget.hpp"

#include "interpreter.hpp"
#include "semantic_error.hpp"

class NotebookApp: public QWidget {
Q_OBJECT
public:
	NotebookApp(QWidget* parent = nullptr);
private:
	InputWidget* input;
	OutputWidget* output;

	Interpreter interp;

private slots:
	void process(const QString& str);
};

#endif

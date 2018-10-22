#ifndef NOTEBOOK_APP_HPP
#define NOTEBOOK_APP_HPP

#include "input_widget.hpp"
#include "output_widget.hpp"

#include <QWidget>

class NotebookApp: public QWidget {
Q_OBJECT
public:
	NotebookApp(QWidget* parent = nullptr);
private:
	InputWidget* input;
	// OutputWidget* output;
};

#endif

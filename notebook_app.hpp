#ifndef NOTEBOOK_APP_HPP
#define NOTEBOOK_APP_HPP

#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <fstream>

#include "input_widget.hpp"
#include "output_widget.hpp"

#include "threaded_interpreter.hpp"

class NotebookApp: public QWidget {
Q_OBJECT
public:
	NotebookApp(QWidget* parent = nullptr);

	void initButtons();
private:
	InputWidget* input;
	OutputWidget* output;

	QHBoxLayout* kernelControlButtons;
	QPushButton* startBtn;
	QPushButton* stopBtn;
	QPushButton* resetBtn;
	QPushButton* interruptBtn;

	InputQueue iq;
	OutputQueue oq;

	QTimer* outputTimer;

	ThreadedInterpreter interp;
private slots:
	void process(const QString& str);
	void checkOutput();

	void startKernel();
	void stopKernel();
	void resetKernel();
	void interruptKernel();

signals:
	void outputProcessed();
};

#endif

#include <QTest>
#include <QString>
#include <QGraphicsItem>
#include <QList>
#include <QPair>

#include "notebook_app.hpp"

class NotebookTest : public QObject {
	Q_OBJECT

private slots:
	void findInputWidget();
	void findOutputWidget();
	void findOutputView();
	void findOutputScene();
	void simpleExpressions();

	// Tests for point objects
	void testPoint();
	void testPointLists();

	// Tests for line objects
	void testLine();
	void testLineLists();

	// Tests for line objects
	void testText();
	void testTextLists();

	// TODO: implement additional tests here
private:
	NotebookApp app;

	// Helper functions for repeated tests
	void submitInput(QWidget* w);
	QGraphicsScene* getScene(QWidget* output);
	void verifyNumberOfOutputGraphics(QWidget* output, qreal n);
	void testSimpleExpression(const QString& input, const QString& output);
	void testSinglePoint(const QString& input, const QRectF& expected);
	void testSingleLine(const QString& input, const QLineF& expected, const qreal& thickness);
	void testSingleText(const QString& input, const QString& expected, const QPointF& pos);
};

// ************ START: Helper functions for repeated tests ************
void NotebookTest::submitInput(QWidget* w) {
	QTest::keyPress(w, Qt::Key_Return, Qt::ShiftModifier);
	QTest::keyRelease(w, Qt::Key_Return, Qt::ShiftModifier);
}

QGraphicsScene* NotebookTest::getScene(QWidget* output) {
	auto view = output->findChild<QGraphicsView*>();
	return view->scene();
}

void NotebookTest::verifyNumberOfOutputGraphics(QWidget* output, qreal n) {
	auto children = getScene(output)->items();
	QVERIFY2(children.size() == n, "Wrong number of graphics items in the output");
}

void NotebookTest::testSimpleExpression(const QString& input, const QString& output) {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, input);
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 1);

	// Make sure the one graphic is a text object at the center of the scene
	auto scene = getScene(op);
	auto item = scene->itemAt(0, 0, QTransform());

	QGraphicsTextItem* graphic = qgraphicsitem_cast<QGraphicsTextItem*>(item);
	QVERIFY2(graphic != NULL, "Graphics item is not a text item");

	// I use contains here so I can test for errors (i.e. test if starts with "Error:")
	QString msg = "Testing simple text expression: unexpected output for " + input +
		". got \"" + graphic->toPlainText() + "\" expected \"" + output + "\"";
	QVERIFY2(graphic->toPlainText().contains(output), qPrintable(msg));
}

void NotebookTest::testSinglePoint(const QString& input, const QRectF& expected) {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, input);
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 1);

	auto scene = getScene(op);
	auto item = scene->items()[0];

	QGraphicsEllipseItem* graphic = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
	QVERIFY2(graphic != NULL, "Graphics item is not an ellipse item");

	QCOMPARE(graphic->rect(), expected);
}

void NotebookTest::testSingleLine(const QString& input, const QLineF& expected,
	const qreal& thickness) {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, input);
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 1);

	auto scene = getScene(op);
	auto item = scene->items()[0];

	QGraphicsLineItem* graphic = qgraphicsitem_cast<QGraphicsLineItem*>(item);
	QVERIFY2(graphic != NULL, "Graphics item is not an line item");

	QCOMPARE(graphic->line(), expected);
	QCOMPARE(graphic->pen(), QPen(QBrush(Qt::black), thickness));
}

void NotebookTest::testSingleText(const QString& input, const QString& expected,
	const QPointF& pos) {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, input);
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 1);

	auto scene = getScene(op);
	auto item = scene->itemAt(pos, QTransform());

	QGraphicsTextItem* graphic = qgraphicsitem_cast<QGraphicsTextItem*>(item);
	QVERIFY2(graphic != NULL, "Graphics item is not an text item");

	QCOMPARE(graphic->toPlainText(), expected);
}

// ************ END: Helper functions for repeated tests ************
void NotebookTest::findInputWidget() {
	auto ip = app.findChild<QWidget*>("input");

	QVERIFY2(ip, "Could not find widget with name: 'input'");
}

void NotebookTest::findOutputWidget() {
	auto op = app.findChild<QWidget*>("output");

	QVERIFY2(op, "Could not find widget with name: 'output'");
}

void NotebookTest::findOutputView() {
	auto op = app.findChild<QWidget*>("output");
	auto view = op->findChild<QGraphicsView*>();

	QVERIFY2(view, "Could not find a view in the output widget");
}

void NotebookTest::findOutputScene() {
	auto op = app.findChild<QWidget*>("output");
	auto view = op->findChild<QGraphicsView*>();
	auto scene = view->scene();

	QVERIFY2(scene, "Could not find a scene in the output widget");
}

void NotebookTest::simpleExpressions() {

	// List of inputs and outputs for simple expressions
	QList<QPair<QString, QString> > tests({
		qMakePair(QString("(1)"), QString("(1)")),
		qMakePair(QString("(\"test\")"), QString("(\"test\")")),
		qMakePair(QString("(+ 1 2)"), QString("(3)")),
		qMakePair(QString("(cos pi)"), QString("(-1)")),
		qMakePair(QString("(+ 1 (* 3 I))"), QString("(1,3)")),
		qMakePair(QString("(first (list (list 1) 2 3))"), QString("(1)")),
		qMakePair(QString("(begin (define inc (lambda (x) (+ 1 x))) (inc 1))"), QString("(2)")),
		qMakePair(QString("("), QString("Error")),
		qMakePair(QString("(+)"), QString("Error")),
		qMakePair(QString("(first (2))"), QString("Error"))
	});

	for (auto test = tests.cbegin(); test != tests.cend(); test++) {
		testSimpleExpression(test->first, test->second);
	}
}

void NotebookTest::testPoint() {
	testSinglePoint(QString("(make-point 0 0)"), QRectF(0, 0, 0, 0));
	testSinglePoint(QString("(make-point 10 15)"), QRectF(10, 15, 0, 0));
	testSinglePoint(QString("(set-property \"size\" 20 (make-point 10 10))"),
		QRectF(0, 0, 20, 20));

	// Error handling of point objects
	testSimpleExpression(QString("(set-property \"size\" -1 (make-point 0 0))"),
		QString("Error"));
	testSimpleExpression(QString("(make-point I 0)"),
		QString("Error"));
	testSimpleExpression(QString("(make-point 1 2 3)"),
		QString("Error"));
	testSimpleExpression(QString("(set-property \"object-name\" \"point\" 3"),
		QString("Error"));
}

void NotebookTest::testPointLists() {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, "(list "
		"(set-property \"size\" 1 (make-point 0 0)) "
		"(set-property \"size\" 2 (make-point 4 0)) "
		"(set-property \"size\" 4 (make-point 8 0))"
		")");
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 3);

	auto scene = getScene(op);
	auto items = scene->items();
	QList<QGraphicsEllipseItem*> graphics;

	// Make sure the graphics aren't duplicated
	bool first = false,
		second = false,
		third = false;

	// verify the types of each object
	for (auto item = items.cbegin(); item != items.cend(); item++) {
		QGraphicsEllipseItem* graphic = qgraphicsitem_cast<QGraphicsEllipseItem*>(*item);
		QVERIFY2(graphic != NULL, "Graphics item is not an ellipse item");

		// Unsure of the order, so I use a conditional
		if (!first && graphic->rect() == QRectF(-0.5, -0.5, 1, 1)) {
			first = true;
		} else if (!second && graphic->rect() == QRectF(3, -1, 2, 2)) {
			second = true;
		} else if (!third && graphic->rect() == QRectF(6, -2, 4, 4)) {
			third = true;
		} else {
			QString msg = "Unknown point in list: \"" + QString(QTest::toString(graphic->rect())) + "\"";
			QFAIL(qPrintable(msg));
		}
	}
}

void NotebookTest::testLine() {
	testSingleLine(QString("(make-line (make-point 0 0) (make-point 20 20))"),
		QLineF(0, 0, 20, 20), (qreal)1);
	testSingleLine(QString("(set-property \"thickness\" 4 "
		"(make-line (make-point 15 5) (make-point 11 19)))"), QLineF(15, 5, 11, 19), (qreal)4);

	// Error handling of point objects
	testSimpleExpression(QString("(make-line 1 (make-point 20 20))"),
		QString("Error"));
	testSimpleExpression(QString("(set-property \"thickness\" -1 "
		"(make-line (make-point 0 0) (make-point 20 20)))"), QString("Error"));
}

void NotebookTest::testLineLists() {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, "(list "
		"(set-property \"thickness\" 2 (make-line (make-point 0 0) (make-point 0 20))) "
		"(set-property \"thickness\" 4 (make-line (make-point 10 0) (make-point 10 20))) "
		"(set-property \"thickness\" 8 (make-line (make-point 20 0) (make-point 20 20)))"
		")");
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 3);

	auto scene = getScene(op);
	auto items = scene->items();
	QList<QGraphicsLineItem*> graphics;

	// Make sure the graphics aren't duplicated
	bool first = false,
		second = false,
		third = false;

	// verify the types of each object
	for (auto item = items.cbegin(); item != items.cend(); item++) {
		QGraphicsLineItem* graphic = qgraphicsitem_cast<QGraphicsLineItem*>(*item);
		QVERIFY2(graphic != NULL, "Graphics item is not a line item");

		// Unsure of the order, so I use a conditional
		if (!first && graphic->line() == QLineF(0, 0, 0, 20) &&
			graphic->pen() == QPen(QBrush(Qt::black), 2)) {
			first = true;
		} else if (!second && graphic->line() == QLineF(10, 0, 10, 20) &&
			graphic->pen() == QPen(QBrush(Qt::black), 4)) {
			second = true;
		} else if (!third && graphic->line() == QLineF(20, 0, 20, 20) &&
			graphic->pen() == QPen(QBrush(Qt::black), 8)) {
			third = true;
		} else {
			QString msg = "Unknown line in list";
			QFAIL(qPrintable(msg));
		}
	}
}

void NotebookTest::testText() {
	testSingleText(QString("(make-text \"Hi\")"), QString("Hi"), QPointF());
	testSingleText(QString("(set-property \"position\" (make-point 10 20) (make-text \"Hi\"))"),
		QString("Hi"), QPointF(10, 20));

	// Error handling of point objects
	testSimpleExpression(QString("(make-text 7)"), QString("Error"));
	testSimpleExpression(QString("(set-property \"position\" e (make-text \"Hi\"))"),
		QString("Error"));
}

void NotebookTest::testTextLists() {

}

QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"

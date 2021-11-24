

#include <QApplication>
#include <QWidget>
#include <QMessageBox>

int main(int argv, char **args)
{
	QApplication	app(argv, args);
	QMessageBox		msgBox;

	msgBox.setText("Hello World!");
	msgBox.show();
	return app.exec();
}
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void doCom();
    int index=0;
    QString comf,outcomf,decomf,outdecomf;
    void dragEnterEvent(QDragEnterEvent* e);	//声明
    void dropEvent(QDropEvent* e);

private slots:
    void on_com_clicked();

    void on_decom_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H

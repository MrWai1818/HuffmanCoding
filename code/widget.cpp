#include "widget.h"
#include"HuffmanCompress.h"
#include "ui_widget.h"
#include<QInputDialog>
#include<QLineEdit>
#include<QTextEdit>
#include<QMessageBox>
#include<QPushButton>
#include<QPushButton>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setAcceptDrops(true);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_com_clicked()
{
        comf=ui->INPUT->text();
        outcomf=ui->OUTPUT->text();
        doCom();
        if(index==0){
            QMessageBox message(QMessageBox::NoIcon, " ", "打开文件"+comf+"失败");
            message.exec();
        }else{
            QMessageBox message(QMessageBox::NoIcon, " ", "压缩成功");
            message.exec();
            ui->textEdit->append("读取将要压缩的文件:"+comf);
            QString temp0=QString::number(lencom,10);
            ui->textEdit->append("当前文件有:"+temp0+"字节");
            ui->textEdit->append("压缩后文件为:"+outcomf);
            QString temp1=QString::number(outlencom,10);
            ui->textEdit->append("压缩后文件有:"+temp1+"字节"+"\n");
        }
}


void Widget::on_decom_clicked()
{
        decomf=ui->INPUT->text();
        outdecomf=ui->OUTPUT->text();
        doCom();
        if(index==0){
            QMessageBox message(QMessageBox::NoIcon, " ", "打开文件"+decomf+"失败");
            message.exec();
        }else{
            QMessageBox message(QMessageBox::NoIcon, " ", "解压成功");
            message.exec();
            ui->textEdit->append("将要读取解压的文件:"+decomf);
            QString temp0=QString::number(lende,10);
            ui->textEdit->append("当前文件有:"+temp0+"字节");
            ui->textEdit->append("解压后文件为:"+outdecomf);
            QString temp1=QString::number(outlende,10);
            ui->textEdit->append("解压后文件有:"+temp1+"字节"+"\n");
        }
}
void Widget::doCom(){
    memset(&header, 0, sizeof(header));
    memset(&tmp, 0, sizeof(tmp));
    string filenameToCompress;   //需要压缩的文件名
    string compressFilename;     //压缩文件命名
    string filenameToUncompress; //需要解压的文件名
    string uncompressFilename;   //解压文件命名
    filenameToCompress=comf.toStdString();
    compressFilename=outcomf.toStdString();
    filenameToUncompress=decomf.toStdString();
    uncompressFilename=outdecomf.toStdString();
    if(!filenameToCompress.empty()){
         index=compress(filenameToCompress, compressFilename);
    }
    if(!filenameToUncompress.empty()){
        index=uncompress(filenameToUncompress, uncompressFilename);
    }
}

void Widget::dragEnterEvent(QDragEnterEvent* e)
{
    //对拖放事件进行筛选
    if (true)
    {
        e->acceptProposedAction();	//放行，否则不会执行dropEvent()函数
    }
}
void Widget::dropEvent(QDropEvent* e)
{
    //获取文件路径 (QString)
    QList<QUrl> urls = e->mimeData()->urls();
    if (urls.isEmpty()) return;
    QString qStr = urls.first().toLocalFile();
    ui->INPUT->setText(qStr);
}

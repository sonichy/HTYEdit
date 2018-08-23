#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mdichild.h"
#include <QDesktopWidget>
#include <QMessageBox>
#include <QTextEdit>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QTextDocumentWriter>
#include <QMdiSubWindow>
#include <QLabel>
#include <QDesktopServices>
#include <QPushButton>
#include <QFontDialog>
#include <QMimeData>
#include <QTextBrowser>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPainter>
//#include <QRegExp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    move((QApplication::desktop()->width() - width())/2, (QApplication::desktop()->height() - height())/2);

    lineEdit_command = new QLineEdit;
    lineEdit_command->setFixedWidth(300);
    lineEdit_command->setPlaceholderText("命令");
    ui->mainToolBar->addWidget(lineEdit_command);
    ui->textBrowser->hide();

    LS1 = new QLabel("欢迎使用海天鹰编辑器！");
    LS1->setMinimumSize(500,20);
    LS1->setStyleSheet("padding:0px 3px;");
    //LS1->setFrameShape(QFrame::WinPanel);
    //LS1->setFrameShadow(QFrame::Sunken);
    LS2 = new QLabel("行,列:");
    LS2->setMinimumSize(20,20);
    LS2->setStyleSheet("padding:0px 3px;");
    LS3 = new QLabel("编码");
    LS3->setMinimumSize(20,20);
    LS3->setStyleSheet("padding:0px 3px;");
    ui->statusBar->addWidget(LS1);
    ui->statusBar->addWidget(LS2);
    ui->statusBar->addWidget(LS3);
    connect(ui->action_quit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(subWindowActivate(QMdiSubWindow*)));

    dialogFind = new DialogFind(this);
    connect(dialogFind->ui->pushButton_find_next,SIGNAL(clicked(bool)),this,SLOT(find()));
    connect(dialogFind->ui->pushButton_replace,SIGNAL(clicked(bool)),this,SLOT(replace()));
    connect(dialogFind->ui->pushButton_replaceAll,SIGNAL(clicked(bool)),this,SLOT(replaceAll()));

    QStringList Largs = QApplication::arguments();
    qDebug() << Largs;
    if(Largs.length()>1){
        QUrl url(Largs.at(1));
        open(url.toLocalFile());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_aboutQt_triggered()
{
    QMessageBox::aboutQt(NULL, "关于 Qt");
}

void MainWindow::on_action_about_triggered()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "海天鹰编辑器 1.5\n\n一款基于 Qt 的文本编辑程序。\n作者：黄颖\nE-mail: sonichy@163.com\n主页：https://github.com/sonichy\n参考文献：\n多文档编辑器：http://www.qter.org/?page_id=161\nQMdiArea基本用法：http://www.mamicode.com/info-detail-1607476.html\n保存文本：http://blog.csdn.net/neicole/article/details/7330234\n语法高亮：http://www.cnblogs.com/lenxvp/p/5475931.html\n拖放打开文件：http://blog.csdn.net/rl529014/article/details/53057577\n行号：http://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html");
    aboutMB.setIconPixmap(QPixmap(":/icon.png"));
    aboutMB.setWindowIcon(QIcon(":/icon.png"));
    aboutMB.exec();
}

void MainWindow::on_action_changelog_triggered()
{
    QString s = "1.5\n2018-08\n增加行号\n增加java文件编译命令\n\n1.4\n2018-07\n设置QTextEdit的Tab跳过的空格数为4个空格\n\n1.3\n2018-06\n增加调试窗口。\n\n1.2\n2018-05\n增加c、cpp的OpenGL编译命令。\n2018-05\n解决右键打开方式无法打开文件问题。\n增加运行python。\n2018-04\n增加打印功能。\n\n1.1\n2017-10\n增加获取文本编码(使用 file --mime-encoding 命令返回)，但是没有解决乱码问题。\n排版实验。\n用文本框代替消息框显示更新日志。\n2017-07\n增加拖放打开文件。\n2017-06\n增加语法高亮。\n提取打开文件的相对路径，使Markdown预览能够载入相对路径图片。\n\n1.0\n2017-03\n支持命令行打开文件和打开方式打开文件。\n查找窗口填入选中文本。\n2017-02\n根据文件扩展名选择语法高亮方案。\nJS语法高亮实验成功！\nHTML语法高亮实验成功！\n增加设置字体。\n设置状态栏左右边距。\n2017-01\n实现全部替换。\n设置循环查找。\n增加查找替换窗体和功能。\n根据文件扩展名决定是否使用默认程序打开，如htm。\n优化保存、另存为和文本修动标题标记逻辑。\n增加撤销，重做，子窗标题文本改动标识。\n增加子窗体类，实现Ctrl+滚轮缩放和保存打开文件的路径。\n增加使用默认程序预览文件。\n把上一个打开或保存的路径设置为打开或保存对话框的默认路径和文件名。\n增加放大、缩小。\n增加文本光标变化信号，光标所在行列显示在状态栏第二栏。\n状态栏分为2栏\n修复没有子窗口时预览引起的崩溃。\n增加预览功能。\n保存成功。\n修改字体颜色，背景色成功。\n新建文件成功，打开文件载入成功。\n选用QMdiArea作为主控件，增加窗口标签、平铺、层叠菜单。 \n制作主要菜单。";
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("更新历史");
    dialog->setFixedSize(400,300);
    QVBoxLayout *vbox = new QVBoxLayout;
    QTextBrowser *textBrowser = new QTextBrowser;
    textBrowser->setText(s);
    textBrowser->zoomIn();
    vbox->addWidget(textBrowser);
    QHBoxLayout *hbox = new QHBoxLayout;
    QPushButton *pushbutton_confirm = new QPushButton("确定");
    hbox->addStretch();
    hbox->addWidget(pushbutton_confirm);
    hbox->addStretch();
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);
    dialog->show();
    connect(pushbutton_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    if(dialog->exec() == QDialog::Accepted){
        dialog->close();
    }
}

void MainWindow::on_action_subWindowView_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
}

void MainWindow::on_action_tabbedView_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::TabbedView);
}

void MainWindow::on_action_cascade_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    ui->mdiArea->cascadeSubWindows();   //重叠窗体
}

void MainWindow::on_action_tile_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    ui->mdiArea->tileSubWindows();   //平铺窗体
}

void MainWindow::on_action_new_triggered()
{
    MdiChild *child = new MdiChild(this);
    ui->mdiArea->addSubWindow(child);
    QPalette plt = palette();
    plt.setColor(QPalette::Text,QColor(Qt::white));
    plt.setBrush(QPalette::Base,QBrush(Qt::black));
    child->setPalette(plt);
    //child->selectAll();
    //child->setFontPointSize(13);
    child->show();
    child->setWindowTitle("未命名[*]");
    setWindowModified(false);
    connect(child, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChange()));
    LS2->setText("行,列：1,0");
}

void MainWindow::on_action_open_triggered()
{
    if (filename == "") {
        filename = QFileDialog::getOpenFileName(this, "打开文本", ".");
    } else {
        filename = QFileDialog::getOpenFileName(this, "打开文本", path);
    }
    if (!filename.isEmpty()) {
        open(filename);
    }
}

void MainWindow::open(QString fileName)
{
    MdiChild *child = new MdiChild(this);
    connect(child, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChange()));
    ui->mdiArea->addSubWindow(child);
    if (child->loadFile(fileName)) {
        path = fileName;
        LS1->setText("打开 " + fileName);
        LS2->setText("行,列：1,0");
        //SyntaxHighlight();
        updateCommand();
    }
}

void MainWindow::on_action_close_triggered()
{
    ui->mdiArea->closeActiveSubWindow();
}

void MainWindow::on_action_save_triggered()
{
    if (ui->mdiArea->currentSubWindow() != 0) {
        path = ((MdiChild*)(ui->mdiArea->currentSubWindow()->widget()))->path;
        if (path == "") {
            on_action_saveas_triggered();
        } else {
            MdiChild *child = (MdiChild*)(ui->mdiArea->currentSubWindow()->widget());
            if (child->save()) {
                LS1->setText("保存 " + child->path);
            }
        }
    }
}

void MainWindow::on_action_saveas_triggered()
{
    if (ui->mdiArea->currentSubWindow() != 0) {
        MdiChild *child = (MdiChild*)(ui->mdiArea->currentSubWindow()->widget());
        path = child->path;
        if (path == "") {
            filename = QFileDialog::getSaveFileName(this, "保存文本", "./未命名");
        } else {
            filename = QFileDialog::getSaveFileName(this, "保存文本", path);
        }
        if (!filename.isEmpty()) {
            child->path = filename;
            on_action_save_triggered();
        }
    }
}

void MainWindow::on_action_run_triggered()
{
    if (ui->mdiArea->currentSubWindow() != 0) {
        QString filename1 = QFileInfo(((MdiChild*)(ui->mdiArea->currentSubWindow()->widget()))->path).fileName();
        QString suffix = QFileInfo(((MdiChild*)(ui->mdiArea->currentSubWindow()->widget()))->path).suffix().toLower();
        QString filepath = QFileInfo(((MdiChild*)(ui->mdiArea->currentSubWindow()->widget()))->path).absolutePath() + "/";

        if (suffix == "md") {
            QString s = ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->toPlainText();
            s.replace("#","<h1>");
            s.replace("\n","</h1>");
            s.replace("  ","<br>");
            //s.replace(QRegExp("!\[(.*)]\((.*)"),"<img src=\\2>");
            s.replace("](", "<img src=" + filepath);
            s.replace(")", ">");
            qDebug() << s;
            MdiChild *child = new MdiChild(this);
            ui->mdiArea->addSubWindow(child);
            child->show();
            child->setWindowTitle("预览 " + filename1);
            //child->setHtml(s);
            child->setPlainText(s);
            connect(child, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChange()));
            LS2->setText("行,列：1,0 ");
        } else if (suffix == "htm" || suffix == "html") {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        } else if (suffix == "py") {
            on_action_save_triggered();
            //LS1->setText("python " + path);
            QProcess *process = new QProcess;
            process->setWorkingDirectory(filepath);
            //QString command = "python " + path;
            QString command = lineEdit_command->text().arg("./" + QFileInfo(path).fileName());
            qDebug() << command;
            ui->textBrowser->setText("");
            ui->textBrowser->append(command);
            connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput()));
            connect(process, SIGNAL(readyReadStandardError()), this, SLOT(printError()));
            process->start(command);
        } else if (suffix == "c" || suffix == "cpp") {
            if (((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->document()->isModified()) on_action_save_triggered();
            QString command = lineEdit_command->text().arg(filename1).arg(QFileInfo(path).baseName());
            LS1->setText(command);
            ui->textBrowser->setText("");
            ui->textBrowser->append(command);
            QProcess *process_compile = new QProcess;
            process_compile->setWorkingDirectory(filepath);
            connect(process_compile, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput()));
            connect(process_compile, SIGNAL(readyReadStandardError()), this, SLOT(printError()));
            connect(process_compile, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus exitStatus){
                Q_UNUSED(exitCode);
                Q_UNUSED(exitStatus);
                qDebug() << exitCode << exitStatus;
                QProcess *process_run = new QProcess;
                process_run->setWorkingDirectory(filepath);
                connect(process_run, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput()));
                connect(process_run, SIGNAL(readyReadStandardError()), this, SLOT(printError()));
                QString command2 = "./" + QFileInfo(path).baseName();
                ui->textBrowser->append(command2);
                process_run->start(command2);
            });
            qDebug() << command;
            process_compile->start(command);
        } else if (suffix == "java") {
            on_action_save_triggered();
            QString command = lineEdit_command->text().arg(filename1).arg(QFileInfo(path).baseName());
            LS1->setText(command);
            ui->textBrowser->setText("");
            ui->textBrowser->append(command);
            QProcess *process_compile = new QProcess;
            process_compile->setWorkingDirectory(filepath);
            connect(process_compile, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput()));
            connect(process_compile, SIGNAL(readyReadStandardError()), this, SLOT(printError()));
            connect(process_compile, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus exitStatus){
                Q_UNUSED(exitCode);
                Q_UNUSED(exitStatus);
                qDebug() << exitCode << exitStatus;
                QProcess *process_run = new QProcess;
                process_run->setWorkingDirectory(filepath);
                connect(process_run, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput()));
                connect(process_run, SIGNAL(readyReadStandardError()), this, SLOT(printError()));
                QString command2 = "java " + QFileInfo(path).baseName();
                ui->textBrowser->append(command2);
                process_run->start(command2);
            });
            qDebug() << command;
            process_compile->start(command);
        }
    }
}

void MainWindow::cursorPositionChange()
{
    if(ui->mdiArea->currentSubWindow() != 0){
        QTextCursor cursor = ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->textCursor();
        LS2->setText(QString("行,列: %1,%2").arg(cursor.blockNumber()+1).arg(cursor.columnNumber()));
    }
}

//菜单
void MainWindow::on_action_zoomin_triggered()
{
    if(ui->mdiArea->currentSubWindow() != 0){
        ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->zoomIn();
    }
}

void MainWindow::on_action_zoomout_triggered()
{
    if(ui->mdiArea->currentSubWindow() != 0){
        ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->zoomOut();
    }
}

void MainWindow::on_action_undo_triggered()
{
    if(ui->mdiArea->currentSubWindow() != 0){
        ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->undo();
    }
}

void MainWindow::on_action_redo_triggered()
{
    if(ui->mdiArea->currentSubWindow() != 0){
        ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->redo();
    }
}

void MainWindow::on_action_find_triggered()
{
    if(ui->mdiArea->currentSubWindow() != 0){
        dialogFind->show();
        dialogFind->ui->lineEdit_find->setFocus();
        dialogFind->ui->lineEdit_find->setText(((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->textCursor().selectedText());
    }
}

void MainWindow::find()
{
    if(ui->mdiArea->currentSubWindow() != 0){
        QString sfind = dialogFind->ui->lineEdit_find->text();
        if(!((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->find(sfind)){
            QMessageBox MB(QMessageBox::Question, "提示", QString("找不到\"%1\",是否从头查起。").arg(sfind));
            MB.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            MB.setButtonText(QMessageBox::Yes,QString("是"));
            MB.setButtonText(QMessageBox::No,QString("否"));
            if(MB.exec() == QMessageBox::Yes){
                QTextCursor cursor = ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->textCursor();
                cursor.setPosition(0,QTextCursor::MoveAnchor);
                ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextCursor(cursor);
                find();
            }
        }
    }
}

void MainWindow::replace()
{
    QString sfind = dialogFind->ui->lineEdit_find->text();
    QString sreplace = dialogFind->ui->lineEdit_replace->text();
    QTextCursor cursor = ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->textCursor();
    if(cursor.selectedText() == sfind){
        cursor.insertText(sreplace);
        find();
    }
}

void MainWindow::replaceAll()
{
    QString sfind = dialogFind->ui->lineEdit_find->text();
    QString sreplace = dialogFind->ui->lineEdit_replace->text();
    QTextCursor cursor = ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->textCursor();
    cursor.setPosition(0,QTextCursor::MoveAnchor);
    ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextCursor(cursor);
    while(((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->find(sfind)){
        cursor = ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->textCursor();
        if(cursor.selectedText() == sfind){
            cursor.insertText(sreplace);
        }
    }
}

void MainWindow::on_action_indent_triggered()
{
    QTextCursor cursor = ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->textCursor();
    QString s = cursor.selectedText().replace("{","{\n");
    s = s.replace("}","}\n");
    s = s.replace("[","[\n");
    s = s.replace("]","]\n");
    s = s.replace(",",",\n");
    //qDebug() << s;
    cursor.insertText(s);
}

void MainWindow::on_action_font_triggered()
{
    if(ui->mdiArea->currentSubWindow() != 0){
        bool ok;
        qDebug() << ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->currentFont();
        QFont font = QFontDialog::getFont(&ok, ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->currentFont(), this, "选择字体");
        if(ok){
            ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setCurrentFont(font);
        }
    }
}

//自己写的高亮效率低，打开文件会卡死，弃用，引入QSyntaxHighlighter类
void MainWindow::SyntaxHighlight()
{
    QTextCursor cursor = ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->textCursor();
    QString suffix = QFileInfo(((MdiChild*)(ui->mdiArea->currentSubWindow()->widget()))->path).suffix().toLower();
    if (suffix == "htm" || suffix == "html") {
        //HTML
        QString str="!DOCTYPE,html,link,head,meta,body,title,style,script,p,br,pre,table,tr,td,input,div,img,a,h1,h2,h3,h4,h6,select,option,ul,ol,li,canvas,fieldset,legend,input,button";
        QStringList list=str.split(",");
        for(int i=0;i<list.size();i++){
            //cursor=((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->textCursor();
            cursor.setPosition(0,QTextCursor::MoveAnchor);
            ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextCursor(cursor);
            while(((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->find("<" + list[i] + ">")){
                ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextColor(QColor(255,0,0));
            }
            //cursor=((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->textCursor();
            cursor.setPosition(0,QTextCursor::MoveAnchor);
            ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextCursor(cursor);
            while(((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->find("<" + list[i])){
                ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextColor(QColor(255,0,0));
            }
            //cursor=((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->textCursor();
            cursor.setPosition(0,QTextCursor::MoveAnchor);
            ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextCursor(cursor);
            while(((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->find("</" + list[i] + ">")){
                ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextColor(QColor(255,0,0));
            }
            //cursor=((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->textCursor();
            cursor.setPosition(0,QTextCursor::MoveAnchor);
            ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextCursor(cursor);
            while(((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->find(">")){
                ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextColor(QColor(255,0,0));
            }
        }

        //HTML属性
        str = "id=,name=,http-equiv=,content=,width=,height=,align=,onchange=,value=,type=";
        list = str.split(",");
        for(int i=0; i<list.size(); i++){
            //cursor=((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->textCursor();
            cursor.setPosition(0,QTextCursor::MoveAnchor);
            ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextCursor(cursor);
            while(((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->find(list[i])){
                ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextColor(QColor(255,255,0));
            }
        }
    } else if (suffix=="js") {
        //JS
        QString str = "window,event,var,new,Array(),Image(),push,document,getElementById,createElement,appendChild,console,.log,.style,backgroundColor,for,if,textContent,innerHTML,function,.src,.load,.complete,.onload,.width,.height,.value,beginPath(),lineTo,moveTo,stroke(),strokeStyle,getContext,eval,translate,textAlign,Math,.cos,.sin,.pow,.random,fillText,addEventListener,length,drawImage,.top,.bottom,.left,.right,onmousemove,onmouseup,offsetLeft,offsetRight,offsetTop,offsetBottom,offsetWidth,offsetHeight,this,options,selectedIndex,.text,setInterval,clearInterval";
        QStringList list = str.split(",");
        for(int i=0; i<list.size(); i++){
            //cursor=((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->textCursor();
            cursor.setPosition(0,QTextCursor::MoveAnchor);
            ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextCursor(cursor);
            while(((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->find(list[i],QTextDocument::FindCaseSensitively)){
                ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextColor(QColor(0,255,0));
            }
        }
    }

    cursor.setPosition(0,QTextCursor::MoveAnchor);
    ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setTextCursor(cursor);
    ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->setWindowModified(false);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    qDebug() << e->mimeData()->formats().at(0);
    //if(e->mimeData()->hasFormat("text/uri-list")) //只能打开文本文件
        e->acceptProposedAction(); //可以在这个窗口部件上拖放对象
}

void MainWindow::dropEvent(QDropEvent *e) //释放对方时，执行的操作
{
    QList<QUrl> urls = e->mimeData()->urls();
    if(urls.isEmpty())
        return ;

    QString fileName = urls.first().toLocalFile();

    foreach (QUrl u, urls) {
        qDebug() << u.toString();
    }
    qDebug() << urls.size();

    if(fileName.isEmpty())
        return;

    open(fileName);
}

void MainWindow::subWindowActivate(QMdiSubWindow *window)
{
    if (window) {
        MdiChild *child = (MdiChild*)(window->widget());
        path = child->path;
        LS1->setText(path);
        LS3->setText(child->scodec);
        updateCommand();
    } else {
        LS1->setText("");
        LS2->setText("");
        LS3->setText("");
    }
}


void MainWindow::on_action_print_triggered()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted){
        printDocument(&printer);
    }
}

void MainWindow::on_action_printPreview_triggered()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printDocument(QPrinter*)));
    preview.exec();
}

void MainWindow::printDocument(QPrinter *printer)
{
    if (ui->mdiArea->currentSubWindow() != 0) {
        MdiChild *child = (MdiChild*)(ui->mdiArea->currentSubWindow()->widget());
        child->print(printer);
    }
}

void MainWindow::updateCommand()
{
    if (ui->mdiArea->currentSubWindow() != 0) {
        QString suffix = QFileInfo(((MdiChild*)(ui->mdiArea->currentSubWindow()->widget()))->path).suffix().toLower();
        if (suffix == "c" || suffix == "cpp") {
            ui->textBrowser->show();
            QString s = ((QTextEdit*)(ui->mdiArea->currentSubWindow()->widget()))->toPlainText();
            QStringList SL = s.split("\n");
            QStringList SLI = SL.filter(QRegExp("^#include"));;
            QString command = "";
            for(int i=0; i<SLI.length(); i++){
                //qDebug() << SLI.at(i);
                if (SLI.at(i).contains("#include <GL/")) {
                    command = "g++ %1 -o %2 -l GL -l GLU -l glut";
                    break;
                } else {
                    command = "g++ %1 -o %2";
                }
            }
            lineEdit_command->setText(command);
        } else if (suffix == "py") {
            ui->textBrowser->show();
            lineEdit_command->setText("python %1");
        } else if (suffix == "java") {
            ui->textBrowser->show();
            lineEdit_command->setText("javac %1");
        } else {
            lineEdit_command->setText("");
            ui->textBrowser->hide();
        }
    }
}

void MainWindow::printOutput()
{
    QProcess *process = qobject_cast<QProcess*>(sender());
    QString s = QString(process->readAllStandardOutput());
    qDebug() << s;
    ui->textBrowser->append(s);
}

void MainWindow::printError()
{
    QProcess *process = qobject_cast<QProcess*>(sender());
    QString s = QString(process->readAllStandardError());
    qDebug() << s;
    ui->textBrowser->append(s);
}
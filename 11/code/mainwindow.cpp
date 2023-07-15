//#pragma once
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "subway_map.cpp"
#include <QMessageBox>
#include <QDebug>
#include <qstring.h>
#include <QTextCodec>
#define ButtonWidth 10
#define MOVESTANDARD 10
#define ZOOMSTANDARD 1
#define ZOOMMAX 10
#define ZOOMMIN 0

DWORD WINAPI Thread(LPVOID lpParam)
{
    MainWindow *n = (MainWindow*)lpParam;
    n->printPath();
    return 0;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    if(!buttonimage.load(":/button/button.jpg"))
        cout<<"image fail"<<endl;
    ui->setupUi(this);
    area=new PaintWidget(this);
    zoom=zoom_last=1;
    ui->verticalLayout->addWidget(area);
    QString imagePath(":/button/search.png");
    QPixmap backgroundImage(imagePath);
    backgroundImage = backgroundImage.scaled(ui->ordest->size(),
                                             Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPalette palette;
    palette.setBrush(ui->ordest->backgroundRole(), QBrush(backgroundImage));
    ui->ordest->setPalette(palette);
    ui->ordest->setAutoFillBackground(true);


    QPixmap butImage;
    butImage.load(":/button/add_button.png");
    ui->butZoomIn->setText("");
    ui->butZoomIn->setStyleSheet("QPushButton{border:0px;}");
    ui->butZoomIn->setIcon(butImage);
    ui->butZoomIn->setIconSize(butImage.size());
    ui->butZoomIn->resize(butImage.size());

    butImage.load(":/button/sub_button.png");
    ui->butZoomOut->setText("");
    ui->butZoomOut->setStyleSheet("QPushButton{border:0px;}");
    ui->butZoomOut->setIcon(butImage);
    ui->butZoomOut->setIconSize(butImage.size());
    ui->butZoomOut->resize(butImage.size());


    butImage.load(":/button/lens.png");
    ui->butFindWay->setText("");
    ui->butFindWay->setStyleSheet("QPushButton{border:0px;}");
    ui->butFindWay->setIcon(butImage);
    ui->butFindWay->setIconSize(butImage.size());
    ui->butFindWay->resize(butImage.size());
    ui->inputTime->setVisible(false);
    ui->inputTime_2->setVisible(false);
    ui->textBrowser->setVisible(false);
    ui->hour->setVisible(false);
    ui->minute->setVisible(false);
    connect(ui->radioButton_2, &QRadioButton::toggled, this, &MainWindow::on_radioButton_2_toggled);
    ui->groupBox->setStyleSheet("QGroupBox { border: none; }");
    //input line edit
    ui->stationOrigin->setStyleSheet("QLineEdit { border: 0; background-color: transparent; }");
    ui->stationDestination->setStyleSheet("QLineEdit { border: 0; background-color: transparent; }");


    QMenu *file = menuBar()->addMenu(tr("Open"));
    QMenu *action=menuBar()->addMenu(tr("Function"));
    QMenu *help=menuBar()->addMenu(tr("Help"));
    QAction *openAction= new QAction(tr("&Import"), this);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Import config"));
    connect(openAction,SIGNAL(triggered()),this,SLOT(Import_clicked()));
    file->addAction(openAction);

    QAction *helpMenu=new QAction(tr("About us"),this);
    helpMenu->setStatusTip(tr("About this program."));
    help->addAction(helpMenu);
    connect(helpMenu, &QAction::triggered, this, [&]() {
        QMessageBox::about(this, tr("About us"),
                           tr("<p>软件工程结对项目：地铁漫游</p>"
                              "<p>By: 朱子林 罗家安</p>"));
    });

    QAction *zoom_in=new QAction(tr("Zoom in"),this);
    zoom_in->setStatusTip(tr("Enlarge"));
    connect(zoom_in,SIGNAL(triggered()),this,SLOT(Zoom_in()));
    action->addAction(zoom_in);
    QAction *zoom_out=new QAction(tr("Zoom out"),this);
    zoom_out->setStatusTip(tr("Reduce"));
    connect(zoom_out,SIGNAL(triggered()),this,SLOT(Zoom_out()));
    action->addAction(zoom_out);
    graph=NULL;
    start=end=-1;

    QAction *shortest=new QAction("Normal Mode",this);
    shortest->setStatusTip("Print the shortest path between selected stations.");
    connect(shortest,SIGNAL(triggered()),this,SLOT(ShortestPath()));
    action->addAction(shortest);

    QAction *change=new QAction("Accurate Mode",this);
    change->setStatusTip("Print the shortest path considering iteration2&3.");
    connect(change,SIGNAL(triggered()),this,SLOT(ShortestPathWithTime()));
    action->addAction(change);

    connect(this,SIGNAL(send()),this,SLOT(got()));
    nowbutton=NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    if(event->button()==Qt::LeftButton){
        lastpoint=event->pos();
        endpoint=event->pos();
    }
    moveflag=true;
}

void MainWindow::wheelEvent(QWheelEvent * event){
    if(event->delta()>0)
        Zoom_in();
    else
        Zoom_out();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    if(event->buttons()&Qt::LeftButton){
        endpoint=event->pos();
        positionx=positionx+endpoint.rx()-lastpoint.rx();
        positiony=positiony+endpoint.ry()-lastpoint.ry();    
        lastpoint=endpoint;
        repaint();
    }
}

//重新画图
void MainWindow::repaint(){
    if(graph==NULL)
        return;
    int i,k,len;
    len=list.size();
    QPushButton *button;
    QLabel *label;
    if(zoom!=zoom_last){
        cout<<list.size()<<" "<<labellist.size()<<endl;
        for(i=0;i<len;i++){
            button=list.back();
            button->deleteLater();
            label=labellist.back();
            label->deleteLater();
            list.pop_back();
            nolist.pop_back();
            labellist.pop_back();
        }
    }

    Point p,q;
    if(zoom==zoom_last){
        for(i=0;i<len;i++){
            button=list.at(i);
            label=labellist.at(i);
            k=nolist.at(i);
            p=Point(graph->id2Nodes[k]->x,graph->id2Nodes[k]->y);
            button->move(zoom*p.x+positionx-ButtonWidth*zoom/2,zoom*p.y+positiony-ButtonWidth*zoom/2);
            label->move(zoom*p.x+positionx-ButtonWidth*zoom/2 + ButtonWidth*zoom + zoom*3,
                        zoom*p.y+positiony-ButtonWidth*zoom/2 + zoom*8);
        }
    }

    area->clear();
    QString name;
    buttonimage=buttonimage.scaled(zoom*ButtonWidth,zoom*ButtonWidth);
    Line sta;
    // int add;
    int sublinenum;
    printlen = pathResults.first.size();
    sublinenum=config->linesNum;
    i=0;

    for(i=0;i<sublinenum;i++){
        sta=graph->lines[i];
        for(int j=0,add=1;j<(int)sta.nodes.size();j++){
            p=Point(sta.nodes[j]->x,sta.nodes[j]->y);
            if(zoom!=zoom_last){
                button=new QPushButton("",area);
                if(add==0 && ((zoom*p.x+positionx+ButtonWidth*zoom/2)<0 ||
                        (positiony+zoom*p.y+ButtonWidth*zoom/2)<20))
                       continue;
                string name=sta.nodes[j]->getName();
                QByteArray encodedTextArray(name.c_str(), static_cast<int>(name.size()));
                QString qname = codec->toUnicode(encodedTextArray);
                QLabel *label = new QLabel(area);
                label->setText(qname);
                // Position the label next to the button
                label->move(zoom*p.x+positionx-ButtonWidth*zoom/2 + ButtonWidth*zoom + zoom*3,
                            zoom*p.y+positiony-ButtonWidth*zoom/2 + zoom*8);
                QFont font = label->font();
                font.setPointSize(zoom*4);  // Set the font size here
                label->setFont(font);
                label->show();
                labellist.push_back(label);

                button->setGeometry(zoom*p.x+positionx-ButtonWidth*zoom/2,
                                    zoom*p.y+positiony-ButtonWidth*zoom/2,ButtonWidth*zoom,ButtonWidth*zoom);
                button->clearMask();
                button->setBackgroundRole( QPalette::Base);
                button->setMask(buttonimage.createHeuristicMask());
                button->setIcon(buttonimage);
                button->setIconSize(buttonimage.size());
                button->show();
                connect(button,SIGNAL(released()),this,SLOT(Button_clicked()));
                list.push_back(button);
                nolist.push_back(((Node*)graph->name2subwayStructs[sta.nodes[j]->getName()])->getId());
            }
            if(j>0){
                Point q=Point(sta.nodes[j-1]->x,sta.nodes[j-1]->y);
                if(stressLine==-1 && onshow==0){
                    area->paintpath(zoom*q.x+positionx,zoom*q.y+positiony,zoom*p.x+positionx,zoom*p.y+positiony,zoom,add);
                }
                else if(stressLine!=i || onshow>0){
                    area->paintpath(zoom*q.x+positionx,zoom*q.y+positiony,zoom*p.x+positionx,zoom*p.y+positiony,zoom,100+add);
                }else{
                    area->paintpath(zoom*q.x+positionx,zoom*q.y+positiony,zoom*p.x+positionx,zoom*p.y+positiony,zoom,add);
                }
                add=0;
            }
        }

        if(i==1||i==8){
            p = Point(sta.nodes[0]->x,sta.nodes[0]->y);
            q = Point(sta.nodes[sta.nodes.size()-1]->x,sta.nodes[sta.nodes.size()-1]->y);
            if(stressLine==-1){
                area->paintpath(zoom*q.x+positionx,zoom*q.y+positiony,zoom*p.x+positionx,zoom*p.y+positiony,zoom,0);
            }
            else if(stressLine!=i){
                area->paintpath(zoom*q.x+positionx,zoom*q.y+positiony,zoom*p.x+positionx,zoom*p.y+positiony,zoom,100);
            }else{
                area->paintpath(zoom*q.x+positionx,zoom*q.y+positiony,zoom*p.x+positionx,zoom*p.y+positiony,zoom,0);
            }
        }
    }

    lastprintlen = printlen;
    //paint the original station and destination circle
    Point point;

    if(start != -1){
        point=Point(graph->id2Nodes[start]->x, graph->id2Nodes[start]->y);
        area->paintcircle(point.x*zoom+positionx,point.y*zoom+positiony,zoom, true);
    }
    if(end != -1){
        point=Point(graph->id2Nodes[end]->x, graph->id2Nodes[end]->y);
        area->paintcircle(point.x*zoom+positionx,point.y*zoom+positiony,zoom, false);
    }

    //画出规划路径
    len=pathResults.first.size();
    if(len==0)
        return;

    p = Point(((Node*)graph->name2subwayStructs[pathResults.first[0]])->x,
            ((Node*)graph->name2subwayStructs[pathResults.first[0]])->y);

    for(i=1;i<len;i++){
        q=Point(((Node*)graph->name2subwayStructs[pathResults.first[i]])->x,
                ((Node*)graph->name2subwayStructs[pathResults.first[i]])->y);
        if(i<=nowpos){
            area->paintpath(zoom*q.x+positionx,zoom*q.y+positiony,zoom*p.x+positionx,zoom*p.y+positiony,zoom,-2);
        }else{
            area->paintpath(zoom*q.x+positionx,zoom*q.y+positiony,zoom*p.x+positionx,zoom*p.y+positiony,zoom,-1);
        }
        p=q;
    }

    if(nowpos<len){
         p = Point(((Node*)graph->name2subwayStructs[pathResults.first[nowpos]])->x,
                 ((Node*)graph->name2subwayStructs[pathResults.first[nowpos]])->y);
    }

    if(nowpos<len){
        if(nowpos==1){
            std::string encodedText = ((Node*)graph->name2subwayStructs[pathResults.first[0]])->getName();
            encodedText.append(" ").append(pathResults.second[0]);
            if(config->version=="2.0"){
                encodedText.append(" ").append(pathResultsWithTimes.second[0]);
            }
            QByteArray encodedTextArray(encodedText.c_str(), static_cast<int>(encodedText.size()));
            QString decodedText = codec->toUnicode(encodedTextArray);
            ui->textBrowser->append(decodedText);
        }
        std::string encodedText = ((Node*)graph->name2subwayStructs[pathResults.first[nowpos]])->getName();
        encodedText.append(" ").append(pathResults.second[nowpos]);
        std::cout<<config->version<<std::endl;
        if(config->version=="2.0"){
            if(pathResults.first[nowpos]==pathResults.first[nowpos-1]){
                std::stringstream t1(pathResultsWithTimes.second[nowpos-1]);
                std::string t1hourString, t1minuteString;
                std::getline(t1, t1hourString, ':');
                std::getline(t1, t1minuteString, ':');
                int t1hour = std::stoi(t1hourString);
                int t1minute = std::stoi(t1minuteString);
                int t1tol = t1hour*60+t1minute;

                std::stringstream t2(pathResultsWithTimes.second[nowpos]);
                std::string t2hourString, t2minuteString;
                std::getline(t2, t2hourString, ':');
                std::getline(t2, t2minuteString, ':');
                int t2hour = std::stoi(t2hourString);
                int t2minute = std::stoi(t2minuteString);
                int t2tol = t2hour*60+t2minute;
                int del = t2tol-t1tol;
                encodedText.clear();

                QByteArray gbk1 = codec->fromUnicode("换成用时 ");
                std::string str1 = gbk1.toStdString();
                QByteArray gbk2 = codec->fromUnicode("分");
                std::string str2 = gbk2.toStdString();
                encodedText.append(gbk1).append(std::to_string(del)).append(str2);
            }
            else{
                encodedText.append(" ").append(pathResultsWithTimes.second[nowpos]);
            }
        }
        QByteArray encodedTextArray(encodedText.c_str(), static_cast<int>(encodedText.size()));
        QString decodedText = codec->toUnicode(encodedTextArray);
        ui->textBrowser->append(decodedText);
    }

    if(nowpos<len){

        buttonimage.load(":/button/button.jpg");
        buttonimage=buttonimage.scaled(zoom*ButtonWidth*2,zoom*ButtonWidth*2);
        if(nowbutton!=NULL)
            nowbutton->deleteLater();
        nowbutton=new QPushButton("",area);
        nowbutton->setGeometry(zoom*p.x+positionx-ButtonWidth*zoom,zoom*p.y+positiony-ButtonWidth*zoom,ButtonWidth*zoom*2,ButtonWidth*zoom*2);
        nowbutton->clearMask();
        nowbutton->setBackgroundRole(QPalette::Base);
        nowbutton->setMask(buttonimage.createHeuristicMask());
        nowbutton->setIcon(buttonimage);
        nowbutton->setIconSize(buttonimage.size());
        nowbutton->show();
    }
    else if(nowpos==len){
        if(nowbutton!=NULL)
             nowbutton->deleteLater();
        nowbutton=NULL;
    }
}

// 调用后端最短路函数
void MainWindow::ShortestPath(){
    if(config->version=="2.0"){
        config->version = "1.0";
        config = new Config();
        graph = new Graph(config);
        graph->build(config);
    }

    cout<<graph->id2Nodes[start]->getName()<<" "<<graph->id2Nodes[end]->getName()<<endl;
    pathResults = graph->run_query_from_a_to_b(graph->id2Nodes[start]->getName(), graph->id2Nodes[end]->getName());
//      for(int i=0;i<pathResults.first.size();i++){
//          cout<<pathResults.first[i]<<" "<<pathResults.second[i]<<endl;
//      }
    cout<<graph->id2Nodes[start]->getName()<<" "<<graph->id2Nodes[end]->getName()<<endl;
    ui->textBrowser->clear();
    if(start<0||end<0||start==end)
        return;

    Point p, q;
    p = Point(((Node*)graph->name2subwayStructs[pathResults.first[0]])->x,
            ((Node*)graph->name2subwayStructs[pathResults.first[0]])->y);
    for(int i=1;i<(int)pathResults.first.size();i++){
        q=Point(((Node*)graph->name2subwayStructs[pathResults.first[i]])->x,
                ((Node*)graph->name2subwayStructs[pathResults.first[i]])->y);
        area->paintpath(zoom*q.x+positionx,zoom*q.y+positiony,zoom*p.x+positionx,zoom*p.y+positiony,zoom,-1);
        p=q;
    }
    {
        CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Thread,(LPVOID)this,0,&ThreadID);
    }

}

// 调用后端带有车次时间、大站快车的函数
void MainWindow::ShortestPathWithTime(){
    if(config->version=="1.0"){
        config->version = "2.0";
        graph->buildHierarchical();
    }
    QString hourStr = ui->inputTime->text();
    QString minStr = ui->inputTime_2->text();

    // 将文本内容转换为整数
    bool okHour, okMin;
    int hour = hourStr.toInt(&okHour);
    int minute = minStr.toDouble(&okMin);

    int times=60*hour+minute;
    pathResultsWithTimes = graph->run_query_from_a_to_b_v2(graph->id2Nodes[start]->getName(), graph->id2Nodes[end]->getName(),times);
    std::map<std::string, int> used;
    vector<string> v1,v2,v3;
    for (int i = 0; i < (int)pathResultsWithTimes.first.first.size(); i++) {
        string name = pathResultsWithTimes.first.first[i];
        string line = pathResultsWithTimes.first.second[i];
        string time = pathResultsWithTimes.second[i];
        if (used[name]==0) {
        }else{
            if (used[name] == 1)
                while (v1.back()!=name) {
                    used[v1.back()]--;
                    v1.pop_back();
                    v2.pop_back();
                    v3.pop_back();
                }
            else{
                while (v1.back()!=name) {
                    used[v1.back()]--;
                    v1.pop_back();
                    v2.pop_back();
                    v3.pop_back();
                }
                used[v1.back()]--;
                v1.pop_back();
                v2.pop_back();
                v3.pop_back();
            }

        }
        v1.push_back(name);
        v2.push_back(line);
        v3.push_back(time);
        used[name]++;
    }
    pathResultsWithTimes = {{v1,v2},v3};
    ui->textBrowser->clear();
    if(start<0||end<0||start==end)
      return;

    pathResults=pathResultsWithTimes.first;
    Point p, q;
    p = Point(((Node*)graph->name2subwayStructs[pathResults.first[0]])->x,
          ((Node*)graph->name2subwayStructs[pathResults.first[0]])->y);
    for(int i=1;i<(int)pathResults.first.size();i++){
      q=Point(((Node*)graph->name2subwayStructs[pathResults.first[i]])->x,
              ((Node*)graph->name2subwayStructs[pathResults.first[i]])->y);
      area->paintpath(zoom*q.x+positionx,zoom*q.y+positiony,zoom*p.x+positionx,zoom*p.y+positiony,zoom,-1);
      p=q;
    }
    {
      CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Thread,(LPVOID)this,0,&ThreadID);
    }

}

// 画出路径
void MainWindow::printPath(){
    int i;
    nowpos=0;
    emit send();

    for(i=0;i<=(int)pathResults.first.size();i++){
        QThread::msleep(500);
        nowpos++;
        emit send();
//        cout<<"hereA"<<endl;
    }
    nowpos=0;
    onshow=0;
    pathResults.first.clear();
    pathResults.second.clear();
    pathResultsWithTimes.first.first.clear();
    pathResultsWithTimes.first.second.clear();
    pathResultsWithTimes.second.clear();
}

// 到达站点
void MainWindow::got(){
        if(nowpos==lastpos)
        {
            return;
        }
        lastpos=nowpos;
//        cout<<"gotA"<<endl;
        repaint();
}

// 导入config
void MainWindow::Import_clicked(){

    QString filePath = QFileDialog::getOpenFileName(this,tr("Select config"),"/home/foxman", tr("Text Files (*.txt)"));
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();
    QString absoluteFilePath = fileInfo.absoluteFilePath();
    absoluteFilePath.replace("/", "\\\\");
    cout<<absoluteFilePath.toStdString()<<endl;

    config = new Config(absoluteFilePath.toStdString());
    graph = new Graph(config);
    graph->build(config);

    QPixmap buttonimage;
    if(!buttonimage.load(":/button/button.jpg"))
        cout<<"image fail"<<endl;
    buttonimage=buttonimage.scaled(zoom*ButtonWidth,zoom*ButtonWidth);

    QPushButton * button;
    Line sta;
    // int add;
    int length=list.size();

    for(int i=0;i<length;i++){
        button=list.back();
        button->deleteLater();
        list.pop_back();
        nolist.pop_back();
    }
    area->clear();


    positionx = positiony = 0;
    zoom = 1;
    int sublinenum = graph->lines.size();

    Point p, q;
    for(int i=0;i<sublinenum;i++){
        sta=graph->lines[i];
        int add = 1;
        for(int j=0;j<(int)sta.nodes.size();j++){
            p=Point(sta.nodes[j]->x,sta.nodes[j]->y);

            string name=sta.nodes[j]->getName();
            QByteArray encodedTextArray(name.c_str(), static_cast<int>(name.size()));
            QString qname = codec->toUnicode(encodedTextArray);
            QLabel *label = new QLabel(area);
            label->setText(qname);
            // Position the label next to the button
            label->move(zoom*p.x-ButtonWidth*zoom/2 + ButtonWidth*zoom + 3, zoom*p.y-ButtonWidth*zoom/2 + 8);
            QFont font = label->font();
            font.setPointSize(4);  // Set the font size here
            label->setFont(font);
            label->show();
            labellist.push_back(label);

            button=new QPushButton("",area);
            button->setGeometry(zoom*p.x-ButtonWidth*zoom/2,zoom*p.y-ButtonWidth*zoom/2,
                                ButtonWidth*zoom,ButtonWidth*zoom);
            button->clearMask();
            //button->setFixedSize(buttonimage.size());
            button->setBackgroundRole(QPalette::Base);
            button->setMask(buttonimage.createHeuristicMask());
            button->setIcon(buttonimage);
            button->setIconSize(buttonimage.size());
            button->show();
            connect(button,SIGNAL(released()),this,SLOT(Button_clicked()));
            list.push_back(button);
            nolist.push_back(((Node*)graph->name2subwayStructs[sta.nodes[j]->getName()])->getId());
            if(j>0){
               q=Point(sta.nodes[j-1]->x,sta.nodes[j-1]->y);
               area->paintpath(zoom*q.x+positionx,zoom*q.y+positiony,zoom*p.x+positionx,zoom*p.y+positiony,zoom,add);
               add = 0;
            }
        }
        // 环路首尾相连
        if(i==1||i==8){
            p = Point(sta.nodes[0]->x,sta.nodes[0]->y);
            q = Point(sta.nodes[sta.nodes.size()-1]->x,sta.nodes[sta.nodes.size()-1]->y);
            area->paintpath(zoom*q.x+positionx,zoom*q.y+positiony,zoom*p.x+positionx,zoom*p.y+positiony,zoom,0);
        }
    }

    // int xylen = sublinenum;
    ui->LineName->clear();
    ui->LineName->addItem("查看线路");
    for(int i = 0; i < sublinenum; i++){
        std::string encodedText = graph->lines[i].getName();
        QByteArray encodedTextArray(encodedText.c_str(), static_cast<int>(encodedText.size()));
        QString decodedText = codec->toUnicode(encodedTextArray);
        ui->LineName->addItem(decodedText);
    }


}

// 放大
void MainWindow::Zoom_in(){
    if(zoom + ZOOMSTANDARD <ZOOMMAX){
        zoom_last=zoom;
        zoom+=ZOOMSTANDARD;
        QSize p=this->size();
        positionx=p.width()/2-((double)zoom/(double)(zoom-ZOOMSTANDARD))*(p.width()/2-positionx);
        positiony=p.height()/2-((double)zoom/(double)(zoom-ZOOMSTANDARD))*(p.height()/2-positiony);
        repaint();
    }
}


// 缩小
void MainWindow::Zoom_out(){
    if(zoom - ZOOMSTANDARD > ZOOMMIN){
        zoom_last=zoom;
        zoom-=ZOOMSTANDARD;
        QSize p=this->size();
        positionx=p.width()/2-((double)zoom/((zoom+ZOOMSTANDARD)))*(p.width()/2-positionx);
        positiony=p.height()/2-((double)zoom/(zoom+ZOOMSTANDARD))*(p.height()/2-positiony);
        repaint();
    }
}

void MainWindow::resizeEvent(QResizeEvent *size){
    //this->close();
    ui->verticalLayoutWidget->resize(size->size());
    repaint();
}

void MainWindow::Button_clicked(){
    cout<<"in clicked";
    int i,no,len;
    QObject *p;
    p=sender();

    len=list.size();
    for(i=0;i<len;i++){
        if(p==list.at(i))
            break;
    }

    no=nolist.at(i);
    if(start == -1){
        start = no;
    }else if(no == start || no == end){
        start = end = -1;
    }else if(end == -1){
        end = no;
    }else{
        start = end;
        end = no;
    }
    if(start != -1){
        std::string encodedText = graph->id2Nodes[start]->getName();
        QByteArray encodedTextArray(encodedText.c_str(), static_cast<int>(encodedText.size()));
        QString decodedText = codec->toUnicode(encodedTextArray);
        ui->stationOrigin->setText(decodedText);
    }else{
        ui->stationOrigin->setText("");
    }
    if(end != -1){
        std::string encodedText = graph->id2Nodes[end]->getName();
        QByteArray encodedTextArray(encodedText.c_str(), static_cast<int>(encodedText.size()));
        QString decodedText = codec->toUnicode(encodedTextArray);
        ui->stationDestination->setText(decodedText);
    }else{
        ui->stationDestination->setText("");
    }
    int length;
        length=pointlist.size();
        for(i=0;i<length;i++)
            pointlist.pop_back();;
        length=path.size();
        for(i=0;i<length;i++)
             path.pop_back();
        if(nowbutton!=NULL)
            nowbutton->deleteLater();
        nowbutton=NULL;
    repaint();

}

void MainWindow::on_butZoomIn_clicked()
{
    Zoom_in();
}

void MainWindow::on_butZoomOut_clicked()
{
    Zoom_out();
}

void MainWindow::on_stationOrigin_textChanged(const QString &arg1)
{
    qDebug() << arg1 << "\n";
    QByteArray ba = ui->stationOrigin->text().toLocal8Bit();
    char * msg = ba.data();

    int id = graph->name2subwayStructs.count(msg);
    if(id == 0){
        ui->originNote->setText("×");
    }else{
        ui->originNote->setText("√");
    }
    if(id!=0){
        start = ((Node*)graph->name2subwayStructs[msg])->getId();
        repaint();
    }
}

void MainWindow::on_stationDestination_textChanged(const QString &arg1)
{   
    qDebug() << arg1 << "\n";
    QByteArray ba = ui->stationDestination->text().toLocal8Bit();
    char * msg = ba.data();

    int id = graph->name2subwayStructs.count(msg);
    if(id == 0){
        ui->destinationNote->setText("×");
    }else{
        ui->destinationNote->setText("√");
    }
    if(id!=0){
        end = ((Node*)graph->name2subwayStructs[msg])->getId();
        repaint();
    }
}

void MainWindow::on_radioButton_clicked()
{
    LenthOrTransfer = true;
    ui->textBrowser->setVisible(false);
}

void MainWindow::on_radioButton_2_clicked()
{
    LenthOrTransfer = false;
    ui->textBrowser->setVisible(false);
}

// 选中查询按钮
void MainWindow::on_butFindWay_clicked()
{
    onshow=1;
    ui->textBrowser->setVisible(true);
    if(LenthOrTransfer){
        ShortestPath();
    }else{
        ShortestPathWithTime();
    }
}

void MainWindow::on_radioButton_2_toggled(bool checked)
{
    ui->inputTime->setVisible(checked);
    ui->inputTime_2->setVisible(checked);
    ui->hour->setVisible(checked);
    ui->minute->setVisible(checked);
}

// 突出显示选中线路
void MainWindow::on_LineName_activated(const QString &arg1)
{
    QByteArray ba = arg1.toLocal8Bit();
    char * msg = ba.data();
    for(int i=0;i<config->linesNum;i++){
        if(strcmp(msg, graph->lines[i].getName().c_str()) == 0){
            stressLine = i;
            repaint();
            return;
        }
    }
    stressLine = -1;
    repaint();
}


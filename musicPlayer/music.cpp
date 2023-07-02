
#include "music.h"
#include "ui_music.h"
#include "myslider.h"
#include "playslider.h"



music::music(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::music)
{
    ui->setupUi(this);
    //窗口隐藏和图标设置
    this->setWindowIcon(QIcon(":/33.png"));
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowMinimizeButtonHint);
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    //变量初始化
    currentIndex = 0;
    playMode = 0;
    pixChange = 7;
    clickSwitchFlag = false;
    m_bDragging = false;
    sliderDragging = false;
    searchListFlag = false;
//    audioOutPut = new QAudioOutput(this);
    player = new QMediaPlayer(this);
    player1 = new QMediaPlayer;
    manager = new QNetworkAccessManager(this);
//    player->setAudioOutput(audioOutPut);
    //声音初始化
    ui->voiceSlider->setMaximum(100);
    ui->voiceSlider->setMinimum(0);
    ui->voiceSlider->setValue(50);
//    audioOutPut->setVolume(0.5);
    player->setVolume(50);
    //进度条初始化
    ui->playSlider->setMaximum(1500);
    ui->playSlider->setMinimum(0);
    //信号连接槽函数
    connect(ui->voiceSlider,&mySlider::valueChanged,this,&music::volumeSet);
//    connect(ui->playSlider,&playslider::mouseClick,this,&music::playProgress);
    connect(player,&QMediaPlayer::positionChanged,this,&music::playSliderChange);
    connect(player,&QMediaPlayer::stateChanged,this,&music::playButtonChange);
    connect(player,&QMediaPlayer::stateChanged,this,&music::autoPlay);
    connect(player,&QMediaPlayer::mediaStatusChanged,this,&music::showDuration);
    connect(player, &QMediaPlayer::durationChanged, this, [=](qint64 duration)
            {
                ui->playSlider->setMaximum(static_cast<int>(duration));
            });
//    connect(ui->playSlider,&QSlider);
    connect(player1,&QMediaPlayer::metaDataAvailableChanged,this,&music::OnMetaDataAvailableChanged);
    connect(manager,SIGNAL(finished(QNetworkReply *)),this,SLOT(databack(QNetworkReply *)));
    //背景透明效果
    opacityEffect = new QGraphicsOpacityEffect;
    opacityEffect->setOpacity(0.8);
    ui->backgroundPic->setGraphicsEffect(opacityEffect);
    //部分按钮隐藏
    ui->voiceSlider->hide();
    ui->addWidget->hide();
    ui->lineEdit->setFocusPolicy(Qt::ClickFocus);

    qApp->installEventFilter(this);  //为所有控件添加事件过滤器
}
//析构函数
music::~music()
{
    delete ui;
}

//搜索请求
void music::on_lineEdit_returnPressed()
{
    ui->stackedWidget_2->setCurrentWidget(ui->page_4);
    ui->musicList2->clear();
    searchList.clear();
    QString s = ui->lineEdit->text();
//    qDebug() << s;
    QString str = QString("http://iwxyi.com:3000/search?keywords=%0&limit=80").arg(ui->lineEdit->text());
    QNetworkRequest request;
    request.setUrl(str);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.111 Safari/537.36");
    manager->get(request);
}

//处理返回数据成表
void music::databack(QNetworkReply *reply)
{
    qDebug()<<"databack";
    searchInfo = reply->readAll();
    QJsonParseError err;
    QJsonDocument json_recv = QJsonDocument::fromJson(searchInfo,&err);
    if(err.error != QJsonParseError::NoError)
    {
        qDebug() <<"搜索Json获取格式错误"<<err.errorString();
            return;
    }
    QJsonObject totalObject = json_recv.object();
    QStringList keys = totalObject.keys();
    if(keys.contains("result"))
    {
        QJsonObject resultObject = totalObject["result"].toObject();
        QStringList resultKeys = resultObject.keys();
        if(resultKeys.contains("songs"))
        {
            QJsonArray array = resultObject["songs"].toArray();
            for(auto i : array)
            {
                QString s;
                QJsonObject object = i.toObject();
                musicId  = object["id"].toInt();
                s = QString("https://music.163.com/song/media/outer/url?id=%0.mp3").arg(QString::number(musicId));
                //                s = QString::fromLatin1(s.toUtf8());
                QUrl url = QUrl(s);
                searchList.append(url);
                musicDuration = object["duration"].toInt();
                musicName = object["name"].toString();
                mvId = object["mvid"].toInt();
                QStringList artistsKey = object.keys();
                if(artistsKey.contains("artists"))
                {
                    QJsonArray artistsArray = object["artists"].toArray();
                    for(auto j : artistsArray)
                    {
                        QJsonObject object2 = j.toObject();
                        singerName = object2["name"].toString();
                    }
                }
                if(artistsKey.contains("album"))
                {
                    QJsonObject albumObject = object["album"].toObject();
                    albumName = albumObject["name"].toString();
                }
//                qDebug()<< musicName + " " + singerName;
                QWidget *widget = new QWidget(ui->musicList2);
                QLabel *titleLabel = new QLabel(ui->musicList2);
                QLabel *artistLabel = new QLabel(ui->musicList2);
                QGridLayout *layout = new QGridLayout(ui->musicList2);
                QListWidgetItem * item = new QListWidgetItem(ui->musicList2);
                layout->addWidget(titleLabel,0,0);
                layout->addWidget(artistLabel,0,1);
                titleLabel->setText(musicName);
                titleLabel->setStyleSheet("font: 12pt 'Microsoft YaHei UI';");
                artistLabel->setText(singerName);
                artistLabel->setStyleSheet("font: 12pt 'Microsoft YaHei UI';");
                widget->setLayout(layout);
                ui->musicList2->setItemWidget(item,widget);
            }
        }
    }
}

//双击播放搜索列表的音乐
void music::on_musicList2_itemDoubleClicked(QListWidgetItem *item)
{
//    qDebug()<<"clicked";
    clickSwitchFlag = true;
    currentIndex = ui->musicList2->currentRow();
    player->setMedia(searchList[currentIndex]);
//    qDebug()<<searchList[currentIndex].isValid();
//    qDebug()<<searchList[currentIndex];
//    qDebug()<<player->errorString();
//    qDebug()<<player->error();
    player->play();
    searchListFlag = true;
    clickSwitchFlag = false;
}

//声音大小设置
void music::volumeSet()
{
    int value = ui->voiceSlider->value();
//    audioOutPut->setVolume((float)value / 100);
    player->setVolume(value);
}
//播放进度设置
//void music::playProgress()
//{
//    int value = ui->playSlider->value();
//    int time = player->metaData(QMediaMetaData::Duration).toInt();
//    player->setPosition((float)value / ui->playSlider->maximum() * time);
//}
//进度条随播放进度同步
void music::playSliderChange()
{
    if(sliderDragging == true)
    {
        return;
    }
    qint64 position = player->position();
//    int time = player->metaData(QMediaMetaData::Duration).toInt();
//    qDebug()<<time;
    ui->playSlider->setValue(position);
}
//页面切换
void music::on_download_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->downloadPage);
}


void music::on_loveMusic_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->lovePage);
}


void music::on_recentPlay_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->rencentpage);
}


//void music::on_pushButton_4_clicked()
//{
//    ui->stackedWidget->setCurrentWidget(ui->musicHomePage);
//}


//声音滑块的显示与隐藏
void music::on_voice_clicked(bool checked)
{
    if(checked == true)
    {
        ui->voiceSlider->show();
    }
    else
    {
        ui->voiceSlider->hide();
    }
}


//首次添加音乐文件
void music::on_addButton_clicked()
{
    addFile();
    if(localList.size() == 0)
    {
        return;
    }
    player->setMedia(localList[currentIndex]);
    ui->stackedWidget_2->setCurrentWidget(ui->page_2);
    ui->addWidget->show();
    ui->localSings->setStyleSheet("QPushButton{"
                                  "background-color: rgba(177, 177, 177, 100);"
                                  "border-radius:0px;"
                                  "font: 290 12pt '微软雅黑 Light';"
                                  "}"
                                  "QPushButton:hover{background-color: rgba(177, 177, 177, 100);}");
}
//播放按钮设置
void music::on_playButton_4_clicked()
{
    if(player->mediaStatus() == QMediaPlayer::NoMedia)
    {
        return;
    }
    if(player->state() == QMediaPlayer::PlayingState)
    {
        player->pause();
    }
    else if(player->state() == QMediaPlayer::PausedState)
    {
        player->play();
    }
    else
    {
        if(playMode == 2)
        {
            if(searchListFlag == false)
            {
                currentIndex = QRandomGenerator::global()->bounded(playList.size());
                player->setMedia(playList[currentIndex]);
                player->play();
            }
            else
            {
                currentIndex = QRandomGenerator::global()->bounded(searchList.size());
                player->setMedia(searchList[currentIndex]);
                player->play();
            }

        }
        else
        {
            player->play();
        }
    }
}
//自动播放下一首
void music::autoPlay()
{
    if(player->state() == QMediaPlayer::StoppedState && clickSwitchFlag == false)
    {
        ui->playSlider->setValue(0);
        if(searchListFlag == false)
        {
            playList = localList;
        }
        else
        {
            playList = searchList;
        }
        if(playMode == 0)
        {
            currentIndex = (currentIndex + 1) % playList.size();
            player->setMedia(playList[currentIndex]);
            player->play();
        }
        else if(playMode == 1)
        {
            player->play();
        }
        else if(playMode == 2)
        {
            currentIndex = QRandomGenerator::global()->bounded(playList.size());
            player->setMedia(playList[currentIndex]);
            player->play();
        }
    }
}
//播放按钮图标切换
void music::playButtonChange()
{
    if(player->state() == QMediaPlayer::PlayingState)
    {
        ui->playButton_4->setStyleSheet("QPushButton{image: url(:/11.png);background-color: rgba(255, 255, 255, 0);}"
                                        );
    }
    else
    {
        ui->playButton_4->setStyleSheet("QPushButton{image: url(:/1.png);background-color: rgba(255, 255, 255, 0);}"
                                        "QPushButton:hover{border-image: url(:/13.png);background-color: rgba(255, 255, 255, 0);}");
    }
}

//切换到下一首
void music::on_nextButton_3_clicked()
{
    if(player->mediaStatus() == QMediaPlayer::NoMedia)
    {
        return;
    }
    clickSwitchFlag = true;
    if(searchListFlag == false)
    {
        playList = localList;
    }
    else
    {
        playList = searchList;
    }
    ui->playSlider->setValue(0);
    currentIndex = (currentIndex + 1) % playList.size();
    player->setMedia(playList[currentIndex]);
    player->play();
    clickSwitchFlag = false;
}

//切换到上一首
void music::on_preButton_clicked()
{
    if(player->mediaStatus() == QMediaPlayer::NoMedia)
    {
        return;
    }
    clickSwitchFlag = true;
    if(searchListFlag == false)
    {
        playList = localList;
    }
    else
    {
        playList = searchList;
    }
    int absolute = 0;
    ui->playSlider->setValue(0);
    if(currentIndex - 1 < 0)
    {
        absolute = currentIndex + playList.size();
    }
    else
    {
        absolute = currentIndex;
    }
    currentIndex = (absolute - 1) % playList.size();
    player->setMedia(playList[currentIndex]);
    player->play();
    clickSwitchFlag = false;
}
//播放模式切换
void music::on_modeButton_2_clicked()
{
    playMode = (playMode + 1) % 3;
    if(playMode == 0)
    {
        ui->modeButton_2->setStyleSheet("QPushButton{border-image: url(:/22.png);}");
    }
    else if(playMode == 1)
    {
        ui->modeButton_2->setStyleSheet("QPushButton{border-image: url(:/6.png);}");
    }
    else if(playMode == 2)
    {
        ui->modeButton_2->setStyleSheet("QPushButton{border-image: url(:/10.png);}");
    }
}

//列表点击播放
void music::on_musicList_itemDoubleClicked(QListWidgetItem *item)
{
    clickSwitchFlag = true;
    currentIndex = ui->musicList->currentRow();
    player->setMedia(localList[currentIndex]);
    player->play();
    searchListFlag = false;
    clickSwitchFlag = false;
}
//显示总时长
void music::showDuration()
{
    if(player->state() == QMediaPlayer::PlayingState)
    {
        int duration;
//        int duration = player->metaData(QMediaMetaData::Duration).toInt();
//        qDebug()<<duration;
//        if(searchListFlag == false)
//        {
//           duration = player->metaData(QMediaMetaData::Duration).toInt();
//        }
//        else
//        {
           duration = player->duration();
//        }
        int min = duration / 1000 / 60;
        int s = duration / 1000 % 60;
        if(searchListFlag == false)
        {
            ui->musicList->setCurrentRow(currentIndex);
        }
        else
        {
            ui->musicList2->setCurrentRow(currentIndex);
        }
        ui->durationLabel->setText(QString("%2 : %1").arg(s).arg(min));
        ui->durationLabel->setStyleSheet("font: 290 12pt '微软雅黑 Light';color: rgb(255, 255, 255);");
    }
}
//页面切换到下载页面
void music::on_downloadedSings_clicked()
{
    ui->downloadedSings->setStyleSheet("QPushButton{"
                                        "background-color: rgba(177, 177, 177, 100);"
                                       "border-radius:0px;"
                                       "font: 290 12pt '微软雅黑 Light';"
                                        "}"
                                        "QPushButton:hover{background-color: rgba(177, 177, 177, 100);}");
    ui->localSings->setStyleSheet("QPushButton{"
                                  "background-color: rgba(255, 255, 255, 50);"
                                  "border-radius:0px;"
                                  "font: 290 12pt '微软雅黑 Light';"
                                  "}"
                                  "QPushButton:hover{background-color: rgba(177, 177, 177, 100);}");
    ui->stackedWidget_2->setCurrentWidget(ui->page_3);
}

//页面切换到本地页面
void music::on_localSings_clicked()
{
    if(localList.size() == 0)
    {
        ui->stackedWidget_2->setCurrentWidget(ui->page);
    }
    else
    {
        ui->stackedWidget_2->setCurrentWidget(ui->page_2);
    }
    ui->localSings->setStyleSheet("QPushButton{"
                                       "background-color: rgba(177, 177, 177, 100);"
                                       "border-radius:0px;"
                                       "font: 290 12pt '微软雅黑 Light';"
                                       "}"
                                       "QPushButton:hover{background-color: rgba(177, 177, 177, 100);}");
    ui->downloadedSings->setStyleSheet("QPushButton{"
                                  "background-color: rgba(255, 255, 255, 50);"
                                  "border-radius:0px;"
                                  "font: 290 12pt '微软雅黑 Light';"
                                  "}"
                                  "QPushButton:hover{background-color: rgba(177, 177, 177, 100);}");
}
//添加音频文件（代码复用）
void music::addFile()
{
    int currentNum = localList.size();
    //打开选择文件窗口，将选中的文件的QUrl保存道playList中
    QStringList strPathList = QFileDialog::getOpenFileNames(this, "选择要添加的音乐文件", "/","(*.mp3);;""(*.wav);;");
    pathSize = strPathList.size();
    if(strPathList.size() == 0)
    {
        return;
    }
    for(int i = 0;i < strPathList.size();i++)
    {
        QString strPath = strPathList[i];
//        QString path = QString::fromLatin1(strPath.toUtf8());//编码转换，不进行此步无法播放路径含有中文的歌曲（qt6.5）
        localList.append(QUrl::fromLocalFile(strPath));
    }
    //获取音乐信息须在音乐播放状态，因此通过创建另一个播放器来专门获取音乐的信息，
        //避免对主播放器造成影响

    //遍历playList中的歌曲信息，输出到列表中
//    for(int i = 0;i < strPathList.size();i++)
//    {
//        clickSwitchFlag = true;
        count = 0;
        player1->setMedia(localList[currentNum + count]);
//        player1->play();

//        player1->stop();
//        clickSwitchFlag = false;
//    }
}
void music::OnMetaDataAvailableChanged(bool available)
{
    if(available)
    {
        QString contributingArtist = player1->metaData(QMediaMetaData::ContributingArtist).toString();
//        qDebug()<<player1->metaData(QMediaMetaData::ContributingArtist).toString();
//        qDebug()<<player1->metaData(QMediaMetaData::Duration).toInt();
        QString title = player1->metaData(QMediaMetaData::Title).toString();
        //信息载入列表并设置布局
        QWidget *widget = new QWidget(ui->musicList);
        QLabel *titleLabel = new QLabel(ui->musicList);
        QLabel *artistLabel = new QLabel(ui->musicList);
        QGridLayout *layout = new QGridLayout(ui->musicList);
        QListWidgetItem * item = new QListWidgetItem(ui->musicList);
        layout->addWidget(titleLabel,0,0);
        layout->addWidget(artistLabel,0,1);
        titleLabel->setText(title);
        titleLabel->setStyleSheet("font: 12pt 'Microsoft YaHei UI';");
        artistLabel->setText(contributingArtist);
        artistLabel->setStyleSheet("font: 12pt 'Microsoft YaHei UI';");
        widget->setLayout(layout);
        ui->musicList->setItemWidget(item,widget);
        if(count < pathSize - 1)
        {
            count++;
            player1->setMedia(localList[count]);
        }
    }
}
//再次添加音频文件
void music::on_addAgainButton_clicked()
{
    addFile();
}
//关闭进程
void music::on_closeButton_clicked()
{
    this->close();
}

//最小化进程
void music::on_minButton_clicked()
{
    this->showMinimized();
}

//窗口拖动
//点击
void music::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QRect rect = this->rect();  //rect是鼠标实现可拖动的区域
        rect.setBottom(rect.top() + 41);
        rect.setRight(rect.right() + 1000);
        if (rect.contains(event->pos()))
        {
            m_bDragging = true;
            m_poStartPosition = event->globalPos();
            m_poFramePosition = frameGeometry().topLeft();
        }
    }
    QWidget::mousePressEvent(event);
}
//移动
void music::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)	//只响应鼠标所见
    {
        if (m_bDragging)
        {
            // delta 相对偏移量,
            QPointF delta = event->globalPos() - m_poStartPosition;
            // 新位置：窗体原始位置  + 偏移量
            move((m_poFramePosition + delta).toPoint());
        }
    }
    QWidget::mouseMoveEvent(event);
}
//释放
void music::mouseReleaseEvent(QMouseEvent* event)
{
    m_bDragging = false;
    QWidget::mouseReleaseEvent(event);
}
//页面切换到音乐馆
void music::on_musicHome_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->musicHomePage);
}

//通过拖动或点击进度条来改变播放进度
//释放
//void music::on_playSlider_sliderReleased()
//{
//    if(player->mediaStatus() == QMediaPlayer::NoMedia)
//    {
//        return;
//    }
////    sliderDragging = true;
//    int value = ui->playSlider->value();
//    int time = player->metaData(QMediaMetaData::Duration).toInt();
//    qDebug()<<value;
//    qDebug()<<ui->playSlider->maximum();
//    qDebug()<<time;
//    qDebug()<<(float)value / ui->playSlider->maximum() * time;
//    qDebug()<<player->duration();
//    qDebug()<<player->position();
//    player->setPosition((int)((float)value / ui->playSlider->maximum() * time));
//    qDebug()<<player->position();
//    sliderDragging = false;
//}

////点击
//void music::on_playSlider_sliderPressed()
//{
//    if(player->mediaStatus() == QMediaPlayer::NoMedia)
//    {
//        return;
//    }
//    sliderDragging = true;
//}

//根据进度条显示当前播放时长
void music::on_playSlider_valueChanged(int value)
{
//    int time = 0;
//    if(searchListFlag == false)
//    {
//        time = player->metaData(QMediaMetaData::Duration).toInt();
//    }
//    else
//    {
//        time = player->duration();
//    }
//    int currentPosition = (float)value / ui->playSlider->maximum() *time;
    int currentPosition = player->position();
    int min = currentPosition / 1000 / 60;
    int s = currentPosition / 1000 % 60;
    ui->positionLabel->setText(QString("%1 : %2").arg(min).arg(s));
    ui->positionLabel->setStyleSheet("font: 290 12pt '微软雅黑 Light';color: rgb(255, 255, 255);");
}

//背景切换
void music::on_backgroundChangeButton_clicked()
{
    pixChange = (pixChange + 1) % 12;
    switch (pixChange) {
    case 0:
        ui->backgroundPic->setStyleSheet("background-image: url(:/19.png);");
        opacityEffect->setOpacity(0.9);
        ui->backgroundPic->setGraphicsEffect(opacityEffect);
        break;
    case 1:
        ui->backgroundPic->setStyleSheet("border-image: url(:/34.png);");
        opacityEffect->setOpacity(0.7);
        ui->backgroundPic->setGraphicsEffect(opacityEffect);
        break;
    case 2:
        ui->backgroundPic->setStyleSheet("border-image: url(:/35.png);");
        opacityEffect->setOpacity(0.7);
        ui->backgroundPic->setGraphicsEffect(opacityEffect);
        break;
    case 3:
        ui->backgroundPic->setStyleSheet("border-image: url(:/36.png);");
        opacityEffect->setOpacity(0.6);
        ui->backgroundPic->setGraphicsEffect(opacityEffect);
        break;
    case 4:
        ui->backgroundPic->setStyleSheet("border-image: url(:/37.png);");
        opacityEffect->setOpacity(0.7);
        ui->backgroundPic->setGraphicsEffect(opacityEffect);
        break;
    case 5:
        ui->backgroundPic->setStyleSheet("border-image: url(:/38.png);");
        opacityEffect->setOpacity(0.7);
        ui->backgroundPic->setGraphicsEffect(opacityEffect);
        break;
    case 6:
        ui->backgroundPic->setStyleSheet("border-image: url(:/39.png);");
        opacityEffect->setOpacity(0.7);
        ui->backgroundPic->setGraphicsEffect(opacityEffect);
        break;
    case 7:
        ui->backgroundPic->setStyleSheet("border-image: url(:/49.png);");
        opacityEffect->setOpacity(0.8);
        ui->backgroundPic->setGraphicsEffect(opacityEffect);
        break;
    case 8:
        ui->backgroundPic->setStyleSheet("border-image: url(:/50.png);");
        opacityEffect->setOpacity(0.8);
        ui->backgroundPic->setGraphicsEffect(opacityEffect);
        break;
    case 9:
        ui->backgroundPic->setStyleSheet("border-image: url(:/47.png);");
        opacityEffect->setOpacity(0.8);
        ui->backgroundPic->setGraphicsEffect(opacityEffect);
        break;
    case 10:
        ui->backgroundPic->setStyleSheet("border-image: url(:/48.png);");
        opacityEffect->setOpacity(0.8);
        ui->backgroundPic->setGraphicsEffect(opacityEffect);
        break;
    case 11:
        ui->backgroundPic->setStyleSheet("border-image: url(:/51.png);");
        opacityEffect->setOpacity(0.6);
        ui->backgroundPic->setGraphicsEffect(opacityEffect);
        break;
    default:
        break;
    }
}


void music::on_playSlider_sliderReleased()
{
    if(player->mediaStatus() == QMediaPlayer::NoMedia)
        {
            return;
        }
    //    sliderDragging = true;
        int value = ui->playSlider->value();
//        int time = player->metaData(QMediaMetaData::Duration).toInt();
//        qDebug()<<value;
//        qDebug()<<ui->playSlider->maximum();
//        qDebug()<<time;
//        qDebug()<<(float)value / ui->playSlider->maximum() * time;
//        qDebug()<<player->duration();
//        qDebug()<<player->position();
        player->setPosition(value);
//        connect(player,&QMediaPlayer::positionChanged,this,[=](){
//            qDebug()<<player->position();
//        });
//        qDebug()<<player->position();
}


void music::on_playSlider_sliderPressed()
{

}

bool music::eventFilter(QObject *watched, QEvent *event) {
        if(event->type() == QEvent::MouseButtonPress && watched !=  ui->lineEdit)
        {
            ui->lineEdit->clearFocus();
            this->setFocus();
        }
        return QObject::eventFilter(watched,event);
}




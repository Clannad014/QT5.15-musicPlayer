
#ifndef MUSIC_H
#define MUSIC_H

#include <QWidget>
#include <QGraphicsOpacityEffect>
#include <QFileDialog>
#include <QList>
#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QAudioOutput>
#include <QSoundEffect>
#include <QTextCodec>
#include <QRandomGenerator>
#include <QPropertyAnimation>
#include <QListWidgetItem>
#include <QDateTime>
#include <QLayout>
#include <QMouseEvent>
#include <QPointF>
#include <QRect>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QByteArray>
#include <QLabel>
#include <QGridLayout>
//#include <QMediaCaptureSession>
//#include <QMediaDevices>
#include <QAudioDecoder>
#include <QAudioFormat>
#include <QBuffer>
#include <QEventLoop>
//#include <QAudioSink>
QT_BEGIN_NAMESPACE
namespace Ui { class music; }
QT_END_NAMESPACE

class music : public QWidget

{
    Q_OBJECT

public:
    music(QWidget *parent = 0);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    bool eventFilter(QObject *watched, QEvent *event);
    void addFile();
    ~music();
private slots:
    void on_download_clicked();

    void on_loveMusic_clicked();

    void on_recentPlay_clicked();

//    void on_pushButton_4_clicked();

    void on_voice_clicked(bool checked);


    void on_addButton_clicked();

    void on_playButton_4_clicked();

    void on_nextButton_3_clicked();

    void on_preButton_clicked();

    void volumeSet();
    void playButtonChange();

    void on_modeButton_2_clicked();
    void autoPlay();
//    void playProgress();
    void playSliderChange();

    void on_musicList_itemDoubleClicked(QListWidgetItem *item);
    void showDuration();

    void on_downloadedSings_clicked();

    void on_localSings_clicked();


    void on_addAgainButton_clicked();



    void on_closeButton_clicked();

    void on_minButton_clicked();


    void on_musicHome_clicked();

    void on_playSlider_sliderReleased();

    void on_playSlider_sliderPressed();

    void on_playSlider_valueChanged(int value);

    void on_backgroundChangeButton_clicked();
    void OnMetaDataAvailableChanged(bool available);

    void on_lineEdit_returnPressed();
    void databack(QNetworkReply *reply);

    void on_musicList2_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::music *ui;
    QGraphicsOpacityEffect *opacityEffect;
    QMediaPlayer *player;
    QMediaPlayer * player1;
    QAudioOutput *audioOutPut;
    QSoundEffect * startSound;
    QNetworkAccessManager *manager;
    QByteArray searchInfo;
    int musicId,musicDuration,mvId;
    QString musicName,singerName,albumName;
    QList<QUrl> searchList;
    QList<QUrl> localList;
    QList<QUrl> playList;
    int count;
    int currentIndex;
    int playMode;
    int pixChange;
    int pathSize;
    bool clickSwitchFlag;
    bool sliderDragging;
    bool m_bDragging;			// 是否正在拖动
    bool searchListFlag;
    QPointF m_poStartPosition;	// 拖动开始前的鼠标位置
    QPointF m_poFramePosition;	// 窗体的原始位置
};

#endif // MUSIC_H

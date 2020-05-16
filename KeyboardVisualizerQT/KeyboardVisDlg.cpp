#include "KeyboardVisDlg.h"
#include "KeyboardVisualizerCommon/Visualizer.h"
#include "KeyboardVisualizerCommon/VisualizerDefines.h"
#include "ui_keyboardvisualizer.h"

#include <QTreeWidgetItem>
#include <QSignalMapper>

Visualizer* vis_ptr;
boolean startminimized;
boolean firstrun;

using namespace Ui;

static void UpdateOpenRGBClientListCallback(void * this_ptr)
{
    KeyboardVisDlg * this_obj = (KeyboardVisDlg *)this_ptr;

    QMetaObject::invokeMethod(this_obj, "UpdateOpenRGBClientList", Qt::QueuedConnection);
}

KeyboardVisDlg::KeyboardVisDlg(QWidget *parent) : QMainWindow(parent), ui(new KeyboardVisualizerDlg)
{
    startminimized = false;
    firstrun = true;

    ui->setupUi(this);

    std::string titleString = "Keyboard Visualizer ";
    titleString.append(VERSION_STRING);
    setWindowTitle(titleString.c_str());

    QIcon icon(":Icon.png");
    setWindowIcon(icon);

    QAction* actionExit = new QAction( "Show/Hide", this );
    connect( actionExit, SIGNAL( triggered() ), this, SLOT( show_hide() ));

    QMenu* myTrayIconMenu = new QMenu( this );
    myTrayIconMenu->addAction( actionExit );

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("Keyboard Visualizer");
    trayIcon->setContextMenu(myTrayIconMenu);
    trayIcon->show();

    ui->lineEdit_IP->setText("127.0.0.1");
    ui->lineEdit_Port->setText("1337");
}

void KeyboardVisDlg::show()
{
    QMainWindow::show();
}

KeyboardVisDlg::~KeyboardVisDlg()
{
    delete ui;
}

void KeyboardVisDlg::show_hide()
{
    if(isHidden())
    {
        timer->start(15);
        show();
    }
    else
    {
        hide();
        timer->stop();
    }
}

void KeyboardVisDlg::update()
{
    if (startminimized && firstrun)
    {
        firstrun = false;
        hide();
        timer->stop();
    }

    for(int x = 0; x < 256; x++)
    {
        for(int y = 0; y < 64; y++)
        {
            COLORREF input = vis_ptr->pixels_out->pixels[y][x];
            COLORREF bgr = RGB(GetBValue(input), GetGValue(input), GetRValue(input));
            image->setPixel(x, y, bgr);
        }
    }

    pixmap.convertFromImage(*image);
    scene->clear();
    scene->addPixmap(pixmap);

    ui->graphicsView_Visualization_Preview->show();

    if(vis_ptr->update_ui)
    {
        vis_ptr->update_ui = false;

        ui->lineEdit_Normalization_Offset->setText(QString::number(vis_ptr->nrml_ofst));
        ui->lineEdit_Normalization_Scale->setText(QString::number(vis_ptr->nrml_scl));
        ui->lineEdit_Animation_Speed->setText(QString::number(vis_ptr->anim_speed));
        ui->lineEdit_Filter_Constant->setText(QString::number(vis_ptr->filter_constant));

        ui->lineEdit_Amplitude->setText(QString::number(vis_ptr->amplitude));
        ui->lineEdit_Background_Brightness->setText(QString::number(vis_ptr->bkgd_bright));
        ui->lineEdit_Average_Size->setText(QString::number(vis_ptr->avg_size));
        ui->lineEdit_Decay->setText(QString::number(vis_ptr->decay));
        ui->lineEdit_Delay->setText(QString::number(vis_ptr->delay));
        ui->lineEdit_Background_Timeout->setText(QString::number(vis_ptr->background_timeout));

        ui->comboBox_Average_Mode->blockSignals(true);
        ui->comboBox_Average_Mode->setCurrentIndex(vis_ptr->avg_mode);
        ui->comboBox_Average_Mode->blockSignals(false);

        ui->comboBox_Background_Mode->blockSignals(true);
        ui->comboBox_Background_Mode->setCurrentIndex(vis_ptr->bkgd_mode);
        ui->comboBox_Background_Mode->blockSignals(false);

        ui->comboBox_FFT_Window_Mode->blockSignals(true);
        ui->comboBox_FFT_Window_Mode->setCurrentIndex(vis_ptr->window_mode);
        ui->comboBox_FFT_Window_Mode->blockSignals(false);

        ui->comboBox_Foreground_Mode->blockSignals(true);
        ui->comboBox_Foreground_Mode->setCurrentIndex(vis_ptr->frgd_mode);
        ui->comboBox_Foreground_Mode->blockSignals(false);

        ui->comboBox_Single_Color_Mode->blockSignals(true);
        ui->comboBox_Single_Color_Mode->setCurrentIndex(vis_ptr->single_color_mode);
        ui->comboBox_Single_Color_Mode->blockSignals(false);

        ui->checkBox_Reactive_Background->setChecked(vis_ptr->reactive_bkgd);
        ui->checkBox_Silent_Background->setChecked(vis_ptr->silent_bkgd);
    }
}

void KeyboardVisDlg::SetVisualizer(Visualizer* v)
{
    vis_ptr = v;

    ui->lineEdit_Normalization_Offset->setText(QString::number(vis_ptr->nrml_ofst));
    ui->lineEdit_Normalization_Scale->setText(QString::number(vis_ptr->nrml_scl));
    ui->lineEdit_Animation_Speed->setText(QString::number(vis_ptr->anim_speed));
    ui->lineEdit_Filter_Constant->setText(QString::number(vis_ptr->filter_constant));

    ui->lineEdit_Amplitude->setText(QString::number(vis_ptr->amplitude));
    ui->lineEdit_Background_Brightness->setText(QString::number(vis_ptr->bkgd_bright));
    ui->lineEdit_Average_Size->setText(QString::number(vis_ptr->avg_size));
    ui->lineEdit_Decay->setText(QString::number(vis_ptr->decay));
    ui->lineEdit_Delay->setText(QString::number(vis_ptr->delay));
    ui->lineEdit_Background_Timeout->setText(QString::number(vis_ptr->background_timeout));

    ui->comboBox_FFT_Window_Mode->blockSignals(true);
    ui->comboBox_FFT_Window_Mode->addItem("None");
    ui->comboBox_FFT_Window_Mode->addItem("Hanning");
    ui->comboBox_FFT_Window_Mode->addItem("Hamming");
    ui->comboBox_FFT_Window_Mode->addItem("Blackman");
    ui->comboBox_FFT_Window_Mode->setCurrentIndex(vis_ptr->window_mode);
    ui->comboBox_FFT_Window_Mode->blockSignals(false);

    ui->comboBox_Background_Mode->blockSignals(true);
    for(int i = 0; i < VISUALIZER_NUM_PATTERNS; i++)
    {
        ui->comboBox_Background_Mode->addItem(visualizer_pattern_labels[i]);
    }
    ui->comboBox_Background_Mode->setCurrentIndex(vis_ptr->bkgd_mode);
    ui->comboBox_Background_Mode->blockSignals(false);

    ui->comboBox_Foreground_Mode->blockSignals(true);
    for(int i = 0; i < VISUALIZER_NUM_PATTERNS; i++)
    {
        ui->comboBox_Foreground_Mode->addItem(visualizer_pattern_labels[i]);
    }
    ui->comboBox_Foreground_Mode->setCurrentIndex(vis_ptr->frgd_mode);
    ui->comboBox_Foreground_Mode->blockSignals(false);

    ui->comboBox_Single_Color_Mode->blockSignals(true);
    for(int i = 0; i < VISUALIZER_NUM_SINGLE_COLOR; i++)
    {
        ui->comboBox_Single_Color_Mode->addItem(visualizer_single_color_labels[i]);
    }
    ui->comboBox_Single_Color_Mode->setCurrentIndex(vis_ptr->single_color_mode);
    ui->comboBox_Single_Color_Mode->blockSignals(false);

    ui->comboBox_Average_Mode->blockSignals(true);
    ui->comboBox_Average_Mode->addItem("Binning");
    ui->comboBox_Average_Mode->addItem("Low Pass");
    ui->comboBox_Average_Mode->setCurrentIndex(vis_ptr->avg_mode);
    ui->comboBox_Average_Mode->blockSignals(false);

    ui->comboBox_Audio_Device->blockSignals(true);
    for(unsigned int i = 0; i < vis_ptr->audio_devices.size(); i++)
    {
        ui->comboBox_Audio_Device->addItem(vis_ptr->audio_devices[i]);
    }
    ui->comboBox_Audio_Device->setCurrentIndex(vis_ptr->audio_device_idx);
    ui->comboBox_Audio_Device->blockSignals(false);

    ui->checkBox_Reactive_Background->setChecked(vis_ptr->reactive_bkgd);
    ui->checkBox_Silent_Background->setChecked(vis_ptr->silent_bkgd);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(15);

    image = new QImage(256, 64, QImage::Format_RGB32);
    scene = new QGraphicsScene(this);
    ui->graphicsView_Visualization_Preview->setScene(scene);
}

void KeyboardVisDlg::StartMinimized(boolean startmin)
{
    startminimized = startmin;
}

void KeyboardVisDlg::on_lineEdit_Amplitude_textChanged(const QString &arg1)
{
    vis_ptr->amplitude = arg1.toInt();
    vis_ptr->OnSettingsChanged();
}

void KeyboardVisDlg::on_lineEdit_Background_Brightness_textChanged(const QString &arg1)
{
    vis_ptr->bkgd_bright = arg1.toInt();
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_lineEdit_Average_Size_textChanged(const QString &arg1)
{
    vis_ptr->avg_size = arg1.toInt();
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_lineEdit_Decay_textChanged(const QString &arg1)
{
    vis_ptr->decay = arg1.toInt();
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_lineEdit_Delay_textChanged(const QString &arg1)
{
    vis_ptr->delay = arg1.toInt();
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_lineEdit_Normalization_Offset_textChanged(const QString &arg1)
{
    vis_ptr->nrml_ofst = arg1.toFloat();
    vis_ptr->SetNormalization(vis_ptr->nrml_ofst, vis_ptr->nrml_scl);
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_lineEdit_Normalization_Scale_textChanged(const QString &arg1)
{
    vis_ptr->nrml_scl = arg1.toFloat();
    vis_ptr->SetNormalization(vis_ptr->nrml_ofst, vis_ptr->nrml_scl);
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_lineEdit_Animation_Speed_textChanged(const QString &arg1)
{
    vis_ptr->anim_speed = arg1.toFloat();
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_comboBox_FFT_Window_Mode_currentIndexChanged(int index)
{
    vis_ptr->window_mode = index;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_comboBox_Background_Mode_currentIndexChanged(int index)
{
    vis_ptr->bkgd_mode = index;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_comboBox_Foreground_Mode_currentIndexChanged(int index)
{
    vis_ptr->frgd_mode = index;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_comboBox_Single_Color_Mode_currentIndexChanged(int index)
{
    vis_ptr->single_color_mode = index;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_comboBox_Average_Mode_currentIndexChanged(int index)
{
    vis_ptr->avg_mode = index;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_pushButton_Save_Settings_clicked()
{
    vis_ptr->SaveSettings();
}

void Ui::KeyboardVisDlg::on_checkBox_Reactive_Background_clicked(bool checked)
{
    vis_ptr->reactive_bkgd = checked;

    if (vis_ptr->reactive_bkgd == true)
    {
        vis_ptr->silent_bkgd = false;
        ui->checkBox_Silent_Background->setChecked(false);
    }
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_comboBox_Audio_Device_currentIndexChanged(int index)
{
    if((unsigned int)index != vis_ptr->audio_device_idx)
    {
        vis_ptr->audio_device_idx = index;
        vis_ptr->ChangeAudioDevice();
    }
}

void Ui::KeyboardVisDlg::on_lineEdit_Filter_Constant_textChanged(const QString &arg1)
{
    vis_ptr->filter_constant = arg1.toFloat();
    if(vis_ptr->filter_constant > 1.0f)
    {
        vis_ptr->filter_constant = 1.0f;
    }
    else if(vis_ptr->filter_constant < 0.0f)
    {
        vis_ptr->filter_constant = 0.0f;
    }
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_checkBox_Silent_Background_clicked(bool checked)
{
    vis_ptr->silent_bkgd = checked;

    if (vis_ptr->silent_bkgd == true)
    {
        vis_ptr->reactive_bkgd = false;
        ui->checkBox_Reactive_Background->setChecked(false);
    }

    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_lineEdit_Background_Timeout_textChanged(const QString &arg1)
{
    vis_ptr->background_timeout = arg1.toInt();

    if (vis_ptr->update_ui == false)
    {
        vis_ptr->background_timer = 0;
    }

    vis_ptr->OnSettingsChanged();
}

class NetworkClientPointer : public QObject
{
public:
    NetworkClient * net_client;
};

void Ui::KeyboardVisDlg::UpdateOpenRGBClientList()
{
    ui->tree_Devices->clear();

    //OpenRGB device list
    ui->tree_Devices->setColumnCount(2);
    ui->tree_Devices->header()->setStretchLastSection(false);
    ui->tree_Devices->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tree_Devices->setColumnWidth(1, 100);
    ui->tree_Devices->setHeaderLabels(QStringList() << "Connected Clients" << "");

    QSignalMapper* signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(QObject *)), this, SLOT(on_button_Disconnect_clicked(QObject *)));

    for(int client_idx = 0; client_idx < vis_ptr->rgb_clients.size(); client_idx++)
    {
        QTreeWidgetItem* new_top_item = new QTreeWidgetItem(ui->tree_Devices);
        new_top_item->setText(0, QString::fromStdString(vis_ptr->rgb_clients[client_idx]->GetIP()));

        QPushButton* new_button = new QPushButton( "Disconnect" );
        ui->tree_Devices->setItemWidget(new_top_item, 1, new_button);

        connect(new_button, SIGNAL(clicked()), signalMapper, SLOT(map()));

        NetworkClientPointer * new_arg = new NetworkClientPointer();
        new_arg->net_client = vis_ptr->rgb_clients[client_idx];

        signalMapper->setMapping(new_button, new_arg);

        for(int dev_idx = 0; dev_idx < vis_ptr->rgb_clients[client_idx]->server_controllers.size(); dev_idx++)
        {
            QTreeWidgetItem* new_item = new QTreeWidgetItem(new_top_item);
            new_item->setText(0, QString::fromStdString(vis_ptr->rgb_clients[client_idx]->server_controllers[dev_idx]->name));

            for(int zone_idx = 0; zone_idx < vis_ptr->rgb_clients[client_idx]->server_controllers[dev_idx]->zones.size(); zone_idx++)
            {
                QTreeWidgetItem* new_child = new QTreeWidgetItem();

                std::string zone_str = vis_ptr->rgb_clients[client_idx]->server_controllers[dev_idx]->zones[zone_idx].name + ", ";
                zone_str.append(std::to_string(vis_ptr->rgb_clients[client_idx]->server_controllers[dev_idx]->zones[zone_idx].leds_count));
                zone_str.append(" LEDs, ");

                switch(vis_ptr->rgb_clients[client_idx]->server_controllers[dev_idx]->zones[zone_idx].type)
                {
                    case ZONE_TYPE_SINGLE:
                        zone_str.append("Single");
                    break;

                    case ZONE_TYPE_LINEAR:
                        zone_str.append("Linear");
                        break;

                    case ZONE_TYPE_MATRIX:
                        zone_str.append("Matrix");
                        break;
                }

                new_child->setText(0, QString::fromStdString(zone_str));


                new_item->addChild(new_child);
            }
        }
    }
}

void Ui::KeyboardVisDlg::on_button_Disconnect_clicked(QObject * arg)
{
    NetworkClient * disconnect_client = ((NetworkClientPointer *)arg)->net_client;

    vis_ptr->OpenRGBDisconnect(disconnect_client);
}

void Ui::KeyboardVisDlg::on_button_Connect_clicked()
{
    unsigned short  port = std::stoi(ui->lineEdit_Port->text().toStdString());
    std::string     ip   = ui->lineEdit_IP->text().toStdString();

    NetworkClient * new_client = vis_ptr->OpenRGBConnect(ip.c_str(), port);

    new_client->RegisterClientInfoChangeCallback(UpdateOpenRGBClientListCallback, this);
}

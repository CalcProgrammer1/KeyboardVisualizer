#include "KeyboardVisDlg.h"
#include "Visualizer.h"
#include "VisualizerDefines.h"
#include "ui_keyboardvisualizer.h"

Visualizer* vis_ptr;
boolean startminimized;
boolean firstrun;

using namespace Ui;

KeyboardVisDlg::KeyboardVisDlg(QWidget *parent) : QMainWindow(parent), ui(new KeyboardVisualizerDlg)
{
    ui->setupUi(this);
}

void KeyboardVisDlg::show()
{
    QIcon icon(":Icon.png");
    setWindowIcon(icon);

    ui->lineEdit_Normalization_Offset->setText(QString::number(vis_ptr->nrml_ofst));
    ui->lineEdit_Normalization_Scale->setText(QString::number(vis_ptr->nrml_scl));
    ui->lineEdit_Animation_Speed->setText(QString::number(vis_ptr->anim_speed));

    ui->lineEdit_Amplitude->setText(QString::number(vis_ptr->amplitude));
    ui->lineEdit_Background_Brightness->setText(QString::number(vis_ptr->bkgd_bright));
    ui->lineEdit_Average_Size->setText(QString::number(vis_ptr->avg_size));
    ui->lineEdit_Decay->setText(QString::number(vis_ptr->decay));
    ui->lineEdit_Delay->setText(QString::number(vis_ptr->delay));

    ui->comboBox_FFT_Window_Mode->blockSignals(TRUE);
    ui->comboBox_FFT_Window_Mode->addItem("None");
    ui->comboBox_FFT_Window_Mode->addItem("Hanning");
    ui->comboBox_FFT_Window_Mode->addItem("Hamming");
    ui->comboBox_FFT_Window_Mode->addItem("Blackman");
    ui->comboBox_FFT_Window_Mode->setCurrentIndex(vis_ptr->window_mode);
    ui->comboBox_FFT_Window_Mode->blockSignals(FALSE);

    ui->comboBox_Background_Mode->blockSignals(TRUE);
    for(int i = 0; i < VISUALIZER_NUM_PATTERNS; i++)
    {
        ui->comboBox_Background_Mode->addItem(visualizer_pattern_labels[i]);
    }
    ui->comboBox_Background_Mode->setCurrentIndex(vis_ptr->bkgd_mode);
    ui->comboBox_Background_Mode->blockSignals(FALSE);

    ui->comboBox_Foreground_Mode->blockSignals(TRUE);
    for(int i = 0; i < VISUALIZER_NUM_PATTERNS; i++)
    {
        ui->comboBox_Foreground_Mode->addItem(visualizer_pattern_labels[i]);
    }
    ui->comboBox_Foreground_Mode->setCurrentIndex(vis_ptr->frgd_mode);
    ui->comboBox_Foreground_Mode->blockSignals(FALSE);

    ui->comboBox_Single_Color_Mode->blockSignals(TRUE);
    for(int i = 0; i < VISUALIZER_NUM_SINGLE_COLOR; i++)
    {
        ui->comboBox_Single_Color_Mode->addItem(visualizer_single_color_labels[i]);
    }
    ui->comboBox_Single_Color_Mode->setCurrentIndex(vis_ptr->single_color_mode);
    ui->comboBox_Single_Color_Mode->blockSignals(FALSE);

    ui->comboBox_Average_Mode->blockSignals(TRUE);
    ui->comboBox_Average_Mode->addItem("Binning");
    ui->comboBox_Average_Mode->addItem("Low Pass");
    ui->comboBox_Average_Mode->setCurrentIndex(vis_ptr->avg_mode);
    ui->comboBox_Average_Mode->blockSignals(FALSE);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(15);

    image = new QImage(256, 64, QImage::Format_RGB32);
    scene = new QGraphicsScene(this);
    ui->graphicsView_Visualization_Preview->setScene(scene);

    QMainWindow::show();
}

KeyboardVisDlg::~KeyboardVisDlg()
{
    delete ui;
}

void KeyboardVisDlg::update()
{
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
}

void KeyboardVisDlg::SetVisualizer(Visualizer* v)
{
    vis_ptr = v;
}

void KeyboardVisDlg::StartMinimized(boolean startmin)
{
    startminimized = startmin;
}

void KeyboardVisDlg::on_lineEdit_Amplitude_textChanged(const QString &arg1)
{
    vis_ptr->amplitude = arg1.toInt();
}

void KeyboardVisDlg::on_lineEdit_Background_Brightness_textChanged(const QString &arg1)
{
    vis_ptr->bkgd_bright = arg1.toInt();
}

void Ui::KeyboardVisDlg::on_lineEdit_Average_Size_textChanged(const QString &arg1)
{
    vis_ptr->avg_size = arg1.toInt();
}

void Ui::KeyboardVisDlg::on_lineEdit_Decay_textChanged(const QString &arg1)
{
    vis_ptr->decay = arg1.toInt();
}

void Ui::KeyboardVisDlg::on_lineEdit_Delay_textChanged(const QString &arg1)
{
    vis_ptr->delay = arg1.toInt();
}

void Ui::KeyboardVisDlg::on_lineEdit_Normalization_Offset_textChanged(const QString &arg1)
{
    vis_ptr->nrml_ofst = arg1.toFloat();
}

void Ui::KeyboardVisDlg::on_lineEdit_Normalization_Scale_textChanged(const QString &arg1)
{
    vis_ptr->nrml_scl = arg1.toFloat();
}

void Ui::KeyboardVisDlg::on_lineEdit_Animation_Speed_textChanged(const QString &arg1)
{
    vis_ptr->anim_speed = arg1.toFloat();
}

void Ui::KeyboardVisDlg::on_comboBox_FFT_Window_Mode_currentIndexChanged(int index)
{
    vis_ptr->window_mode = index;
}

void Ui::KeyboardVisDlg::on_comboBox_Background_Mode_currentIndexChanged(int index)
{
    vis_ptr->bkgd_mode = index;
}

void Ui::KeyboardVisDlg::on_comboBox_Foreground_Mode_currentIndexChanged(int index)
{
    vis_ptr->frgd_mode = index;
}

void Ui::KeyboardVisDlg::on_comboBox_Single_Color_Mode_currentIndexChanged(int index)
{
    vis_ptr->single_color_mode = index;
}

void Ui::KeyboardVisDlg::on_comboBox_Average_Mode_currentIndexChanged(int index)
{
    vis_ptr->avg_mode = index;
}

void Ui::KeyboardVisDlg::on_pushButton_Save_Settings_clicked()
{
    vis_ptr->SaveSettings();
}

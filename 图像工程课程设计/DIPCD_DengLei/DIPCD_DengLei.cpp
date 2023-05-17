#include "stdafx.h"
#include "DIPCD_DengLei.h"
#include <QMessageBox>

using namespace std;

DIPCD_DengLei::DIPCD_DengLei(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::DIPCD_DengLeiClass)
{
    ui->setupUi(this);
    ui->action_openFile->setEnabled(true);
    ui->action_savaFile->setEnabled(false);
    ui->action_quitFile->setEnabled(false);
    ui->action_grayscale->setEnabled(false);
    ui->action_binarization->setEnabled(false);
    ui->action_3x3meanFiltering->setEnabled(false);
    ui->action_3x3medianFiltering->setEnabled(false);
    ui->action_sharpeningOfLaplace4->setEnabled(false);
    ui->action_edgaDetection->setEnabled(false);
    ui->action_histogramCalculationAndDisplay->setEnabled(false);
    ui->action_undoProcessing->setEnabled(false);

    // 设置窗口标题
    setWindowTitle("图像处理工具 作者：20101330邓磊");

    // 给显示的窗口设置图标
    setWindowIcon(QIcon(".\\Resource Files\\图像处理.ico"));
    //setWindowIcon(QIcon(".\\Resource Files\\大户爱.ico"));

    // 退出
    connect(ui->action_quit, &QAction::triggered, this, &DIPCD_DengLei::close);

    //说明文档
    connect(ui->action_documentation, &QAction::triggered, this, [=]()
        {
            QIcon* icon = new QIcon(".\\Resource Files\\图像处理.ico");
            QMessageBox message(QMessageBox::Information, tr("说明文档"), tr("此软件的具体帮助请查阅：图像处理工具说明文档.md\n此软件的项目部署运行说明请查阅：ReadMe.md"));
            message.setWindowIcon(*icon);
            QPushButton* okbutton = (message.addButton(tr("确定"), QMessageBox::AcceptRole));
            message.exec();
        });

    //联系作者
    connect(ui->action_contactAuthor, &QAction::triggered, this, [=]()
        {
            QIcon* icon = new QIcon(".\\Resource Files\\图像处理.ico");
            QMessageBox message(QMessageBox::Information, tr("联系作者"), tr("学号：20101330\n姓名：邓磊\n昵称：末影小黑xh\n\nQQ：1735350920\n开源中国：https://my.oschina.net/MYXHcode\n稀土掘金：https://juejin.cn/user/2907545284124903/posts\nCSDN：https://blog.csdn.net/qq_40734758?type=blog\n哔哩哔哩：https://b23.tv/foSt7Zw"));
            message.setWindowIcon(*icon);
            QPushButton* okbutton = (message.addButton(tr("确定"), QMessageBox::AcceptRole));
            message.exec();
        });
}

DIPCD_DengLei::~DIPCD_DengLei()
{
    delete ui;
}

// MAT类型 转为 QImage类型
QImage DIPCD_DengLei::MatToQImage(const cv::Mat& mat)
{
    // 8 位无符号，信道数 = 1
    if (mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);

        // 设置颜色表（用于将颜色索引转换为 q Rgb值）
        image.setColorCount(256);
        for (int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }

        // 复制输入 Mat
        uchar* pSrc = mat.data;
        for (int row = 0; row < mat.rows; row++)
        {
            uchar* pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }

    // 8 位无符号，信道数 = 3
    else if (mat.type() == CV_8UC3)
    {
        // 复制输入 Mat
        const uchar* pSrc = (const uchar*)mat.data;

        // 创建与输入 Mat 尺寸相同的 QImage
        QImage image(pSrc, mat.cols, mat.rows, (int)mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if (mat.type() == CV_8UC4)
    {
        //qDebug() << "CV_8UC4";
        // 复制输入 Mat
        const uchar* pSrc = (const uchar*)mat.data;

        // 创建与输入 Mat 尺寸相同的 QImage
        QImage image(pSrc, mat.cols, mat.rows, (int)mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        //qDebug() << "错误：Mat 无法转换为 QImage.";
        return QImage();
    }
}

// MAT 对象用 QT显示
void DIPCD_DengLei::display_MatInQT(QLabel* label, Mat mat)
{
    label->setPixmap(QPixmap::fromImage(MatToQImage(mat)).scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

// 打开文件
void DIPCD_DengLei::on_action_openFile_triggered()
{    // 调用窗口打开文件
    ui->label_showOriginalImage->clear();
    ui->label_showProcessImage->clear();
    QString filename =
        QFileDialog::getOpenFileName(this, tr("打开图像"),
            ".",
            tr("Image file(*.png *.jpg *.bmp)"));

    image = imread(filename.toLocal8Bit().data());
    if (image.data)
    {
        ui->action_savaFile->setEnabled(true);
        ui->action_quitFile->setEnabled(true);
        ui->action_grayscale->setEnabled(true);
        ui->action_binarization->setEnabled(true);
        ui->action_3x3meanFiltering->setEnabled(true);
        ui->action_3x3medianFiltering->setEnabled(true);
        ui->action_sharpeningOfLaplace4->setEnabled(true);
        ui->action_edgaDetection->setEnabled(true);
        ui->action_histogramCalculationAndDisplay->setEnabled(true);
        ui->action_undoProcessing->setEnabled(true);

        // 通过 label 方式显示图片
        display_MatInQT(ui->label_showOriginalImage, image);
    }
}

// 保存文件
void DIPCD_DengLei::on_action_savaFile_triggered()
{
    if (image.data)
    {
        QString savePath;
        savePath = QFileDialog::getSaveFileName(0, "请选择图片保存路径", ".\\Image Files", "jpg(*.jpg);;png(*.png);;bmp(*.bmp);;所有文件(*.*)");

        if ((!savePath.isNull()) || (!savePath.isEmpty()))
        {
            const QPixmap saveImgage = ui->label_showProcessImage->pixmap();
            saveImgage.save(savePath);

            QIcon* icon = new QIcon(".\\Resource Files\\图像处理.ico");
            QMessageBox message(QMessageBox::Information, tr("提示"), tr("保存图片成功！"));
            message.setWindowIcon(*icon);
            QPushButton* okbutton = (message.addButton(tr("确定"), QMessageBox::AcceptRole));
            message.exec();
        }
    }
    else
    {
        QIcon* icon = new QIcon(".\\Resource Files\\图像处理.ico");
        QMessageBox message(QMessageBox::Information, tr("提示"), tr("保存图片失败！"));
        message.setWindowIcon(*icon);
        QPushButton* okbutton = (message.addButton(tr("确定"), QMessageBox::AcceptRole));
        message.exec();
    }
}

// 关闭文件
void DIPCD_DengLei::on_action_quitFile_triggered()
{
    if (image.data)
    {
        ui->action_savaFile->setEnabled(false);
        ui->action_grayscale->setEnabled(false);
        ui->action_binarization->setEnabled(false);
        ui->action_3x3meanFiltering->setEnabled(false);
        ui->action_3x3medianFiltering->setEnabled(false);
        ui->action_sharpeningOfLaplace4->setEnabled(false);
        ui->action_edgaDetection->setEnabled(false);
        ui->action_histogramCalculationAndDisplay->setEnabled(false);
        ui->action_undoProcessing->setEnabled(false);

        ui->label_showOriginalImage->clear();
        ui->label_showProcessImage->clear();

        image.release();
        gray.release();
        binary.release();
        meanFiltering3x3.release();
        medianFiltering3x3.release();
        sharpeningOfLaplace4.release();
        edgeDetection.release();
    }
}

// 灰度化
void DIPCD_DengLei::on_action_grayscale_triggered()
{
    ui->label_showProcessImage->clear();

    if (image.data)
    {
        // 灰度化
        cvtColor(image, gray, COLOR_BGR2GRAY);
        // 通过 label 方式显示图片
        display_MatInQT(ui->label_showOriginalImage, image);
        display_MatInQT(ui->label_showProcessImage, gray);
    }
    else
    {
        QIcon* icon = new QIcon(".\\Resource Files\\图像处理.ico");
        QMessageBox message(QMessageBox::Information, tr("提示"), tr("处理图片失败！"));
        message.setWindowIcon(*icon);
        QPushButton* okbutton = (message.addButton(tr("确定"), QMessageBox::AcceptRole));
        message.exec();
    }
}

// 二值化
void DIPCD_DengLei::on_action_binarization_triggered()
{
    ui->label_showProcessImage->clear();

    if (image.data)
    {
        // 获取 int 类型 阈值数据
        int thresholdValue = 0;

        QInputDialog* inputDialog = new QInputDialog(this);
        inputDialog->setOkButtonText(tr("确定"));
        inputDialog->setCancelButtonText(tr("取消"));

        thresholdValue = inputDialog->getInt(this, "提示", "请输入图片二值化的阈值：（范围为 0 ~ 255 之间）", 155, 0, 255, 1);

        // 二值化
        threshold(image, binary, thresholdValue, 255, THRESH_BINARY_INV);

        // 通过 label 方式显示图片
        display_MatInQT(ui->label_showOriginalImage, image);
        display_MatInQT(ui->label_showProcessImage, binary);
    }
    else
    {
        QIcon* icon = new QIcon(".\\Resource Files\\图像处理.ico");
        QMessageBox message(QMessageBox::Information, tr("提示"), tr("处理图片失败！"));
        message.setWindowIcon(*icon);
        QPushButton* okbutton = (message.addButton(tr("确定"), QMessageBox::AcceptRole));
        message.exec();
    }
}

// 3×3 均值滤波
void DIPCD_DengLei::on_action_3x3meanFiltering_triggered()
{
    ui->label_showProcessImage->clear();

    if (image.data)
    {
        // 3×3 均值滤波
        blur(image, meanFiltering3x3, Size(3, 3));
        // 通过 label 方式显示图片
        display_MatInQT(ui->label_showOriginalImage, image);
        display_MatInQT(ui->label_showProcessImage, meanFiltering3x3);
    }
    else
    {
        QIcon* icon = new QIcon(".\\Resource Files\\图像处理.ico");
        QMessageBox message(QMessageBox::Information, tr("提示"), tr("处理图片失败！"));
        message.setWindowIcon(*icon);
        QPushButton* okbutton = (message.addButton(tr("确定"), QMessageBox::AcceptRole));
        message.exec();
    }
}

// 3×3 中值滤波
void DIPCD_DengLei::on_action_3x3medianFiltering_triggered()
{
    ui->label_showProcessImage->clear();

    if (image.data)
    {
        // 3×3 中值滤波
        medianBlur(image, medianFiltering3x3, 3);
        // 通过 label 方式显示图片
        display_MatInQT(ui->label_showOriginalImage, image);
        display_MatInQT(ui->label_showProcessImage, medianFiltering3x3);
    }
    else
    {
        QIcon* icon = new QIcon(".\\Resource Files\\图像处理.ico");
        QMessageBox message(QMessageBox::Information, tr("提示"), tr("处理图片失败！"));
        message.setWindowIcon(*icon);
        QPushButton* okbutton = (message.addButton(tr("确定"), QMessageBox::AcceptRole));
        message.exec();
    }
}

// 拉普拉斯 4 领域锐化
void DIPCD_DengLei::on_action_sharpeningOfLaplace4_triggered()
{
    ui->label_showProcessImage->clear();

    if (image.data)
    {
        // 拉普拉斯 4 领域锐化
        Laplacian(image, sharpeningOfLaplace4, CV_16S, 1, 1, 0, BORDER_DEFAULT);
        // 通过 label 方式显示图片
        display_MatInQT(ui->label_showOriginalImage, image);
        display_MatInQT(ui->label_showProcessImage, sharpeningOfLaplace4);
    }
    else
    {
        QIcon* icon = new QIcon(".\\Resource Files\\图像处理.ico");
        QMessageBox message(QMessageBox::Information, tr("提示"), tr("处理图片失败！"));
        message.setWindowIcon(*icon);
        QPushButton* okbutton = (message.addButton(tr("确定"), QMessageBox::AcceptRole));
        message.exec();
    }
}

// 边缘检测
void DIPCD_DengLei::on_action_edgaDetection_triggered()
{
    ui->label_showProcessImage->clear();

    if (image.data)
    {
        // 边缘检测
        Canny(image, edgeDetection, 150, 100, 3);
        // 通过 label 方式显示图片
        display_MatInQT(ui->label_showOriginalImage, image);
        display_MatInQT(ui->label_showProcessImage, edgeDetection);
    }
    else
    {
        QIcon* icon = new QIcon(".\\Resource Files\\图像处理.ico");
        QMessageBox message(QMessageBox::Information, tr("提示"), tr("处理图片失败！"));
        message.setWindowIcon(*icon);
        QPushButton* okbutton = (message.addButton(tr("确定"), QMessageBox::AcceptRole));
        message.exec();
    }
}

// 直方图计算与显示
void DIPCD_DengLei::on_action_histogramCalculationAndDisplay_triggered()
{
    Mat src, dst;
    src = image;

    if (image.data)
    {
        // 将多通道图像分为单通道图像
        // 单通道图像 vector
        std::vector<Mat> bgr_planes;
        split(src, bgr_planes);

        // 直方图参数
        int histSize = 256;
        float range[] = {0, 256};
        const float* histRanges = {range};
        Mat b_hist, g_hist, r_hist;

        // 求出直方图的数据
        calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize,
            &histRanges, true, false);
        calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize,
            &histRanges, true, false);
        calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize,
            &histRanges, true, false);

        // 画出直方图
        int hist_h = 400;
        int hist_w = 512;
        int bin_w = hist_w / histSize;    // 直方图的步数

        Mat histImage(hist_w, hist_h, CV_8UC3, Scalar(0, 0, 0));

        // 将数据归一化到直方图的图像尺寸中来
        normalize(b_hist, b_hist, 0, hist_h, NORM_MINMAX, -1, Mat());
        normalize(g_hist, g_hist, 0, hist_h, NORM_MINMAX, -1, Mat());
        normalize(r_hist, r_hist, 0, hist_h, NORM_MINMAX, -1, Mat());

        // 0-255 的像素值，画出每个像素值的连线
        //注意：图像中的坐标是以左上角为原点向右下方延伸
        for (int i = 1; i < histSize; ++i)
        {
            line(histImage, Point((i - 1) * bin_w, hist_h - cvRound(b_hist.at<float>(i - 1))),
                Point(i * bin_w, hist_h - cvRound(b_hist.at<float>(i))), Scalar(0, 0, 255), 2, LINE_AA);
            line(histImage, Point((i - 1) * bin_w, hist_h - cvRound(g_hist.at<float>(i - 1))),
                Point(i * bin_w, hist_h - cvRound(g_hist.at<float>(i))), Scalar(0, 255, 0), 2, LINE_AA);
            line(histImage, Point((i - 1) * bin_w, hist_h - cvRound(r_hist.at<float>(i - 1))),
                Point(i * bin_w, hist_h - cvRound(r_hist.at<float>(i))), Scalar(255, 0, 0), 2, LINE_AA);
        }

        // 通过 label 方式显示图片
        display_MatInQT(ui->label_showOriginalImage, image);
        display_MatInQT(ui->label_showProcessImage, histImage);
    }
    else
    {
        QIcon* icon = new QIcon(".\\Resource Files\\图像处理.ico");
        QMessageBox message(QMessageBox::Information, tr("提示"), tr("处理图片失败！"));
        message.setWindowIcon(*icon);
        QPushButton* okbutton = (message.addButton(tr("确定"), QMessageBox::AcceptRole));
        message.exec();
    }
}

// 撤销处理
void DIPCD_DengLei::on_action_undoProcessing_triggered()
{
    if (image.data)
    {
        ui->label_showProcessImage->clear();
    }
}

// 视频灰度化
void DIPCD_DengLei::on_action_grayscaleVideo_triggered()
{
    QString filename =
        QFileDialog::getOpenFileName(this, tr("打开视频"),
            ".",
            tr("Video file(*.mp4 *.avi)"));

    String fileName = filename.toStdString();

    QString savePath;
    String fileOut;

    if ((!filename.isNull()) || (!filename.isEmpty()))
    {
        savePath = QFileDialog::getSaveFileName(0, "请选择视频保存路径", ".\\Video Files\\recorded.mp4", "mp4(*.mp4);;avi(*.avi);;所有文件(*.*)");
    }

    if ((!savePath.isNull()) || (!savePath.isEmpty()))
    {
        fileOut = savePath.toStdString();
    }
    else
    {
        return;
    }

    VideoCapture inVid(fileName);

    Mat inFrame, outFrame;
    const char winIn[] = "Grabbing...", winOut[] = "Recording...";
    double fps = 30;    // 每秒的帧数

    // 打开摄像头，具体设备文件需先查看，参数为 0，则为默认摄像头
    if (!inVid.isOpened())
    {    // 检查错误
        cout << "错误相机未就绪！\n";
        return;
    }

    // 获取输入视频的宽度和高度
    int width = (int)inVid.get(CAP_PROP_FRAME_WIDTH);
    int height = (int)inVid.get(CAP_PROP_FRAME_HEIGHT);
    cout << "width = " << width << ", height = " << height << endl;

    VideoWriter recVid(fileOut, VideoWriter::fourcc('X', 'V', 'I', 'D'), fps, Size(width, height), 0);

    if (!recVid.isOpened())
    {
        cout << "错误视频文件未打开！\n";
        return;
    }

    // 为原始视频和最终视频创建两个窗口
    namedWindow(winIn);
    namedWindow(winOut);

    while (true)
    {
        // 从摄像机读取帧（抓取并解码）以流的形式进行
        inVid >> inFrame;

        if (inFrame.empty())
        {
            inVid.release();
            break;
        }
        // 将帧转换为灰度
        cvtColor(inFrame, outFrame, COLOR_BGR2GRAY);

        // 将帧写入视频文件（编码并保存）以流的形式进行
        recVid << outFrame;

        imshow(winIn, inFrame);    // 在窗口中显示帧
        imshow(winOut, outFrame);    // 在窗口中显示帧

        if (waitKey(1000 / fps) >= 0)
        {
            break;
        }
    }

    inVid.release();    // 关闭摄像机
    QIcon* icon = new QIcon(".\\Resource Files\\图像处理.ico");
    QMessageBox message(QMessageBox::Information, tr("提示"), tr("保存视频成功！\n保存路径：.\\Video Files\\recorded.mp4"));
    message.setWindowIcon(*icon);
    QPushButton* okbutton = (message.addButton(tr("确定"), QMessageBox::AcceptRole));
    message.exec();
}

// 人脸识别
void DIPCD_DengLei::on_action_faceRecognition_triggered()
{
    String faceCascadeName = ".\\Resource Files\\haarcascade_frontalface_alt.xml";
    String eyesAscadeName = ".\\Resource Files\\haarcascade_eye.xml";
    String mouthCascadeName = ".\\Resource Files\\haarcascade_mcs_mouth.xml";

    void faceRecongize(cv::CascadeClassifier faceCascade, cv::CascadeClassifier eyesCascade, cv::CascadeClassifier mouthCascade, cv::Mat frame);

    cv::VideoCapture* videoCap = new cv::VideoCapture;

    cv::CascadeClassifier faceCascade;
    cv::CascadeClassifier eyesCascade;
    cv::CascadeClassifier mouthCascade;

    // 加载脸部分类器文件
    if (!faceCascade.load(faceCascadeName))
    {
        std::cout << "加载 faceCascadeName 失败。" << std::endl;
        return;
    }

    // 加载眼睛部分分类器文件
    if (!eyesCascade.load(eyesAscadeName))
    {
        std::cout << "加载 eyesAscadeName 失败。" << std::endl;
        return;
    }

    // 加载嘴部分类器文件
    if (!mouthCascade.load(mouthCascadeName))
    {
        std::cout << "加载 mouthCascadeName 失败。" << std::endl;
        return;
    }

    // 打开摄像机
    videoCap->open(0);

    if (!videoCap->isOpened())
    {
        videoCap->release();
        std::cout << "打开摄像头失败。" << std::endl;
        return;
    }

    std::cout << "开放式摄像机成功。" << std::endl;

    while (true)
    {
        cv::Mat frame;

        // 读取视频帧
        videoCap->read(frame);

        if (frame.empty())
        {
            videoCap->release();
            return;
        }

        // 进行人脸识别
        faceRecongize(faceCascade, eyesCascade, mouthCascade, frame);

        // 窗口进行展示
        imshow("Face recognition (Press Enter to exit)", frame);

        //等待回车键按下退出程序
        if (cv::waitKey(30) == 13)
        {
            cv::destroyAllWindows();
            return;
        }
    }

    system("pause");
    return;
}

// 面部协调
void faceRecongize(cv::CascadeClassifier faceCascade, cv::CascadeClassifier eyesCascade, cv::CascadeClassifier mouthCascade, cv::Mat frame)
{
    std::vector<cv::Rect> faces;

    // 检测人脸
    faceCascade.detectMultiScale(frame, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

    for (int i = 0; i < faces.size(); i++)
    {
        // 用椭圆画出人脸部分
        cv::Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        ellipse(frame, center, cv::Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, cv::Scalar(255, 0, 255), 4, 8, 0);

        cv::Mat faceROI = frame(faces[i]);
        std::vector<cv::Rect> eyes;

        // 检测眼睛
        eyesCascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
        for (int j = 0; j < eyes.size(); j++)
        {
            // 用圆画出眼睛部分
            cv::Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
            int radius = cvRound((eyes[j].width + eyes[j].height) * 0.25);
            circle(frame, eye_center, radius, cv::Scalar(255, 0, 0), 4, 8, 0);
        }

        cv::Mat mouthROI = frame(faces[i]);
        std::vector<cv::Rect> mouth;

        // 检测嘴部
        mouthCascade.detectMultiScale(mouthROI, mouth, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
        for (int k = 0; k < mouth.size(); k++)
        {
            //用长方形画出嘴部
            cv::Rect rect(faces[i].x + mouth[k].x, faces[i].y + mouth[k].y, mouth[k].width, mouth[k].height);
            rectangle(frame, rect, cv::Scalar(0, 255, 0), 2, 8, 0);
        }

        // 检测到两个眼睛和一个嘴巴, 可认为检测到有效人脸
        if (eyes.size() >= 2 && mouth.size() >= 1)
        {
            // 人脸上方区域写字进行标识
            cv::Point centerText(faces[i].x + faces[i].width / 2 - 40, faces[i].y - 20);
            cv::putText(frame, "face", centerText, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
        }
    }
}
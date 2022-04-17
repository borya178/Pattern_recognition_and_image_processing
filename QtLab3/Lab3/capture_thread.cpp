#include <QTime>
#include <QtConcurrent>
#include <QDebug>

#include "utilities.h"
#include "capture_thread.h"

CaptureThread::CaptureThread(int camera, QMutex *lock):
    running(false), cameraID(camera), videoPath(""), data_lock(lock)
{
    fps_calculating = false;
    fps = 0.0;

    frame_width = frame_height = 0;
    video_saving_status = STOPPED;
    saved_video_name = "";
    video_writer = nullptr;

    motion_detecting_status = false;
}

CaptureThread::CaptureThread(QString videoPath, QMutex *lock):
    running(false), cameraID(-1), videoPath(videoPath), data_lock(lock)
{
    fps_calculating = false;
    fps = 0.0;

    frame_width = frame_height = 0;
    video_saving_status = STOPPED;
    saved_video_name = "";
    video_writer = nullptr;

    motion_detecting_status = false;
}

CaptureThread::~CaptureThread() {
}

void CaptureThread::run() {
    running = true;
    cv::VideoCapture cap(cameraID);
    cv::Mat tmp_frame;
    int frames = 0;
    QTime now_time = QTime::currentTime();
    frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    segmentor = cv::createBackgroundSubtractorMOG2(500, 16, true);

    while(running) {
        cap >> tmp_frame;
        if (tmp_frame.empty()) {
            break;
        }
        if(motion_detecting_status) {
            motionDetect(tmp_frame);
        }
        if(video_saving_status == STARTING) {
            startSavingVideo(tmp_frame);
        }
        if(video_saving_status == STARTED) {
            video_writer->write(tmp_frame);
        }
        if(video_saving_status == STOPPING) {
            stopSavingVideo();
        }

        cvtColor(tmp_frame, tmp_frame, cv::COLOR_BGR2RGB);
        data_lock->lock();
        frame = tmp_frame;
        data_lock->unlock();
        emit frameCaptured(&frame);
        if(fps_calculating) {
            calculateFPS(cap);
        }

        int interval = now_time.secsTo(QTime::currentTime());
        if (interval >= 1) {
            fps = frames / interval;
            now_time = QTime::currentTime();
            frames = 0;

            getMeanAndStdDev(tmp_frame);

            cv::Scalar cv_arr_mean, cv_arr_std_dev;
            cv::meanStdDev(tmp_frame, cv_arr_mean, cv_arr_std_dev);
            qRegisterMetaType <cv::Scalar>("cv::Scalar");
            emit updateStatus(fps,
                cv::Vec3f(cv_arr_mean[0], cv_arr_mean[1], cv_arr_mean[2]),
                cv::Vec3f(arr_mean[0], arr_mean[1], arr_mean[2]),
                cv::Vec3f(cv_arr_std_dev[0], cv_arr_std_dev[1], cv_arr_std_dev[2]),
                cv::Vec3f(arr_std_dev[0], arr_std_dev[1], arr_std_dev[2]));
        } else {
            frames++;
        }
    }
    cap.release();
    running = false;
}

void CaptureThread::calculateFPS(cv::VideoCapture &cap)
{
    const int count_to_read = 100;
    cv::Mat tmp_frame;
    QTime timer;
    timer.start();
    for(int i = 0; i < count_to_read; i++) {
            cap >> tmp_frame;
    }
    int elapsed_ms = timer.elapsed();
    fps = count_to_read / (elapsed_ms / 1000.0);
    fps_calculating = false;
    emit fpsChanged(fps);
}


void CaptureThread::startSavingVideo(cv::Mat &firstFrame)
{
    saved_video_name = Utilities::newSavedVideoName();

    QString cover = Utilities::getSavedVideoPath(saved_video_name, "jpg");
    cv::imwrite(cover.toStdString(), firstFrame);

    video_writer = new cv::VideoWriter(
        Utilities::getSavedVideoPath(saved_video_name, "avi").toStdString(),
        cv::VideoWriter::fourcc('M','J','P','G'),
        fps? fps: 30,
        cv::Size(frame_width,frame_height));
    video_saving_status = STARTED;
}


void CaptureThread::stopSavingVideo()
{
    video_saving_status = STOPPED;
    video_writer->release();
    delete video_writer;
    video_writer = nullptr;
    emit videoSaved(saved_video_name);
}

void CaptureThread::motionDetect(cv::Mat &frame)
{
    cv::Mat fgmask;
    segmentor->apply(frame, fgmask);
    if (fgmask.empty()) {
            return;
    }

    cv::threshold(fgmask, fgmask, 25, 255, cv::THRESH_BINARY);

    int noise_size = 9;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(noise_size, noise_size));
    cv::erode(fgmask, fgmask, kernel);
    kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(noise_size, noise_size));
    cv::dilate(fgmask, fgmask, kernel, cv::Point(-1,-1), 3);

    vector<vector<cv::Point> > contours;
    cv::findContours(fgmask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    bool has_motion = contours.size() > 0;
    if(!motion_detected && has_motion) {
        motion_detected = true;
        setVideoSavingStatus(STARTING);
        qDebug() << "new motion detected, should send a notification.";
        QtConcurrent::run(Utilities::notifyMobile, cameraID);
    } else if (motion_detected && !has_motion) {
        motion_detected = false;
        setVideoSavingStatus(STOPPING);
        qDebug() << "detected motion disappeared.";
    }

    cv::Scalar color = cv::Scalar(0, 0, 255);
    for(size_t i = 0; i < contours.size(); i++) {
        cv::Rect rect = cv::boundingRect(contours[i]);
        cv::rectangle(frame, rect, color, 1);
    }
}

void::CaptureThread::getMeanAndStdDev(cv::Mat &frame){
    cv::Scalar temp_arr_mean = cv::Scalar(0, 0, 0);
    cv::Scalar temp_arr_std_dev= cv::Scalar(0, 0, 0);

    for (int row = 0; row < frame.rows; row++){
        for(int col = 0; col < frame.cols; col++){
            for (int i = 0; i < 3; i++){
                temp_arr_mean[i] += frame.at<cv::Vec3b>(row, col)[i];
                temp_arr_std_dev[i] += std::pow(frame.at<cv::Vec3b>(row, col)[i], 2);
            }
        }
    }

    for (int i = 0; i < 3; i++){
        arr_mean[i] = temp_arr_mean[i] / (frame.rows * frame.cols);
        temp_arr_std_dev[i] = temp_arr_std_dev[i] / (frame.rows * frame.cols);
        arr_std_dev[i] = std::sqrt(temp_arr_std_dev[i] - arr_mean[i] * arr_mean[i]);
    }
}

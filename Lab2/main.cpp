
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    VideoCapture camera(0);

    if (!camera.isOpened())
    {
        cout << "Camera error!" << endl;
        return -1;
    }

    // Загружаем классификатор лиц
    CascadeClassifier faceCascade;

    string cascadePath =
        "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";

    if (!faceCascade.load(cascadePath))
    {
        cout << "Face cascade not found!" << endl;
        return -1;
    }

    namedWindow("Camera");
    namedWindow("Mask");

    int mode = 1;
    bool recording = false;

    VideoWriter video;

    Mat frame, hsv, mask, gray;

    cout << "1 - Red color detection" << endl;
    cout << "2 - Face detection" << endl;
    cout << "R - Start/stop recording" << endl;
    cout << "Q - Exit" << endl;

    while (true)
    {
        camera >> frame;

        if (frame.empty())
            break;

        // Режим 1: поиск красной коробочки
        if (mode == 1)
        {
            cvtColor(frame, hsv, COLOR_BGR2HSV);

            Mat mask1, mask2;

            // Первый диапазон красного
            inRange(hsv,
                    Scalar(0, 100, 100),
                    Scalar(10, 255, 255),
                    mask1);

            // Второй диапазон красного
            inRange(hsv,
                    Scalar(160, 100, 100),
                    Scalar(179, 255, 255),
                    mask2);

            // Объединяем маски
            mask = mask1 | mask2;

            vector<vector<Point>> contours;

            findContours(mask, contours,
                         RETR_EXTERNAL,
                         CHAIN_APPROX_SIMPLE);

            // Находим самый большой объект
            if (!contours.empty())
            {
                int largest = 0;

                for (int i = 1; i < contours.size(); i++)
                {
                    if (contourArea(contours[i]) >
                        contourArea(contours[largest]))
                    {
                        largest = i;
                    }
                }

                if (contourArea(contours[largest]) > 500)
                {
                    Rect box = boundingRect(contours[largest]);

                    rectangle(frame, box,
                              Scalar(0, 255, 0), 2);
                }
            }

            imshow("Mask", mask);
        }

        // Режим 2: обнаружение лица
        if (mode == 2)
        {
            cvtColor(frame, gray, COLOR_BGR2GRAY);

            equalizeHist(gray, gray);

            vector<Rect> faces;

            faceCascade.detectMultiScale(
                gray,
                faces,
                1.1,
                5,
                0,
                Size(30, 30)
            );

            // Обводим найденные лица
            for (int i = 0; i < faces.size(); i++)
            {
                rectangle(frame, faces[i],
                          Scalar(0, 255, 0), 2);
            }

            putText(frame,
                    "Faces: " + to_string(faces.size()),
                    Point(20, 35),
                    FONT_HERSHEY_SIMPLEX,
                    0.8,
                    Scalar(0, 255, 0), 2);

            // Запись видео
            if (recording)
            {
                putText(frame, "RECORDING",
                        Point(20, 70),
                        FONT_HERSHEY_SIMPLEX,
                        0.8,
                        Scalar(0, 0, 255), 2);

                video.write(frame);
            }
        }

        imshow("Camera", frame);

        char key = (char)waitKey(30);

        // Переключение режимов
        if (key == '1')
        {
            mode = 1;
            recording = false;

            if (video.isOpened())
                video.release();
        }

        if (key == '2')
        {
            mode = 2;
        }

        // Начало и остановка записи
        if (key == 'r' || key == 'R')
        {
            if (mode == 2)
            {
                if (!recording)
                {
                    video.open(
                        "face_detection.avi",
                        VideoWriter::fourcc('M', 'J', 'P', 'G'),
                        20,
                        frame.size()
                    );

                    if (video.isOpened())
                    {
                        recording = true;
                        cout << "Recording started!" << endl;
                    }
                    else
                    {
                        cout << "Video recording error!" << endl;
                    }
                }
                else
                {
                    recording = false;
                    video.release();

                    cout << "Recording saved!" << endl;
                }
            }
        }

        if (key == 'q' || key == 'Q')
            break;
    }

    camera.release();

    if (video.isOpened())
        video.release();

    destroyAllWindows();

    return 0;
}
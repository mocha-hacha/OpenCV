
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    // Подключение камеры
    VideoCapture cap(0);

    if (!cap.isOpened())
    {
        cout << "Ошибка открытия камеры!" << endl;
        return -1;
    }

    bool colorCorrection = false;
    bool rotated = false;

    Mat frame, result;

    cout << "Управление:" << endl;
    cout << "S - включить/выключить цветокоррекцию" << endl;
    cout << "T - повернуть изображение на 180 градусов" << endl;
    cout << "ESC - выход" << endl;

    while (true)
    {
        // Получаем кадр с камеры
        cap >> frame;

        if (frame.empty())
            break;

        // Копируем исходный кадр
        result = frame.clone();

        // Цветокоррекция: увеличение красного канала на 10%
        if (colorCorrection)
        {
            vector<Mat> channels;
            split(result, channels);

            // BGR: индекс 2 - красный канал
            channels[2] *= 1.1;

            merge(channels, result);
        }

        // Поворот на 180 градусов
        if (rotated)
        {
            rotate(result, result, ROTATE_180);
        }

        // Отображение изображения
        imshow("OpenCV Lab5", result);

        int key = waitKey(30);

        // S - цветокоррекция
        if (key == 's' || key == 'S')
        {
            colorCorrection = !colorCorrection;

            cout << "Цветокоррекция: "
                 << (colorCorrection ? "ON" : "OFF")
                 << endl;
        }

        // T - поворот
        if (key == 't' || key == 'T')
        {
            rotated = !rotated;

            cout << "Поворот: "
                 << (rotated ? "180 градусов" : "обычное положение")
                 << endl;
        }

        // ESC - выход
        if (key == 27)
            break;
    }

    cap.release();
    destroyAllWindows();

    return 0;
}

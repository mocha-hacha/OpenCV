
#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>

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

    // Загружаем GIF-анимацию
    string logoPath = "logo.gif";

    VideoCapture logo(logoPath);

    if (!logo.isOpened())
    {
        cout << "Logo GIF not found!" << endl;
    }

    bool showSignature = false;
    bool showLogo = false;

    Mat frame, logoFrame;

    while (true)
    {
        camera >> frame;

        if (frame.empty())
            break;

        // Подпись
        if (showSignature)
        {
            putText(frame, "ЭТО НАДПИСЬ :0 !!!!!! :/",
                    Point(30, frame.rows - 30),
                    FONT_HERSHEY_COMPLEX,
                    0.8, Scalar(255, 255, 255), 2);
        }

        // GIF-логотип
        if (showLogo && logo.isOpened())
        {
            if (!logo.read(logoFrame) || logoFrame.empty())
            {
                logo.set(CAP_PROP_POS_FRAMES, 0);
                logo.read(logoFrame);
            }

            if (!logoFrame.empty())
            {
                Mat resizedLogo;

                resize(logoFrame, resizedLogo, Size(100, 100));

                int x = frame.cols - resizedLogo.cols - 20;
                int y = 20;

                if (x >= 0 && y + resizedLogo.rows <= frame.rows)
                {
                    resizedLogo.copyTo(
                        frame(Rect(x, y,
                        resizedLogo.cols, resizedLogo.rows))
                    );
                }
            }
        }

        // Получаем текущее время
        time_t now = time(0);
        tm* localTime = localtime(&now);

        char timeText[20];
        strftime(timeText, sizeof(timeText), "%H:%M:%S", localTime);

        // Тень времени
        putText(frame, timeText,
                Point(52, 82),
                FONT_HERSHEY_COMPLEX,
                1.5, Scalar(0, 0, 0), 6);

        // Само время
        putText(frame, timeText,
                Point(45, 75),
                FONT_HERSHEY_COMPLEX,
                1.5, Scalar(255, 220, 0), 2);

        imshow("My Video Production", frame);

        char key = (char)waitKey(30);

        if (key == 'c' || key == 'C')
            showSignature = !showSignature;

        if (key == 'l' || key == 'L')
            showLogo = !showLogo;

        if (key == 'q' || key == 'Q')
            break;
    }

    camera.release();
    logo.release();

    destroyAllWindows();

    return 0;
}

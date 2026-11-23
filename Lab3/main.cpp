
#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>

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

    // Классификаторы
    CascadeClassifier faceCascade;
    CascadeClassifier smileCascade;
    CascadeClassifier eyeCascade;
    CascadeClassifier mouthCascade;

    string path = "/usr/share/opencv4/haarcascades/";

    faceCascade.load(path + "haarcascade_frontalface_default.xml");
    smileCascade.load(path + "haarcascade_smile.xml");
    eyeCascade.load(path + "haarcascade_eye.xml");
    mouthCascade.load(path + "haarcascade_mcs_mouth.xml");

    if (faceCascade.empty() || smileCascade.empty())
    {
        cout << "Face or smile cascade not found!" << endl;
        return -1;
    }

    // Загружаем картинки эмоций
    Mat sad = imread("sad.png");
    Mat fun = imread("fun.png");
    Mat shock = imread("shock.png");
    Mat calmly = imread("calmly.png");
    Mat zamn = imread("zamn.png");

    if (sad.empty() || fun.empty() || shock.empty() ||
        calmly.empty() || zamn.empty())
    {
        cout << "Emotion images not found!" << endl;
        return -1;
    }

    namedWindow("Face Detection");

    Mat frame, gray;

    bool recording = false;
    int imageCounter = 0;
    int frameCounter = 0;

    cout << "R - Start/stop saving faces" << endl;
    cout << "Q - Exit" << endl;

    while (true)
    {
        camera >> frame;

        if (frame.empty())
            break;

        cvtColor(frame, gray, COLOR_BGR2GRAY);
        equalizeHist(gray, gray);

        vector<Rect> faces;

        // Ищем все лица
        faceCascade.detectMultiScale(
            gray,
            faces,
            1.1,
            5,
            0,
            Size(60, 60)
        );

        for (int i = 0; i < faces.size(); i++)
        {
            Rect face = faces[i];

            // Вырезаем область лица
            Mat faceROI = gray(face);

            // Область рта
            Mat lowerFace = faceROI(
                Rect(0, face.height / 2,
                     face.width, face.height / 2)
            );

            vector<Rect> smiles;
            vector<Rect> eyes;
            vector<Rect> mouths;

            smileCascade.detectMultiScale(
                faceROI,
                smiles,
                1.7,
                20,
                0,
                Size(20, 10)
            );

            eyeCascade.detectMultiScale(
                faceROI,
                eyes,
                1.05,
                3,
                0,
                Size(15, 10)
            );

            // Определяем открытый рот
            if (!mouthCascade.empty())
            {
                mouthCascade.detectMultiSale(
                    lowerFace,
                    mouths,
                    1.1,
                    5,
                    0,
                    Size(5, 15)
                );
            }

            // Выбираем эмоцию
            Mat emotion;
            string emotionName;

            if (!smiles.empty())
            {
                emotion = fun;
                emotionName = "Smile";
            }
            else if (!mouths.empty())
            {
                Rect mouth = mouths[0];

                if ((float)mouth.height / mouth.width > 0.4)
                {
                    emotion = zamn;
                    emotionName = "Zamn";
                }
                else
                {
                    emotion = shock;
                    emotionName = "Surprise";
                }
            }
            else if (eyes.empty())
            {
                emotion = sad;
                emotionName = "Sad";
            }
            else
            {
                emotion = calmly;
                emotionName = "Calm";
            }

            // Рамка лица
            rectangle(frame, face,
                      Scalar(0, 255, 0), 2);

            // Выводим название эмоции
            putText(frame, emotionName,
                    Point(face.x, face.y - 10),
                    FONT_HERSHEY_SIMPLEX,
                    0.6, Scalar(0, 255, 0), 2);

            // Выводим картинку эмоции
            Mat emotionSmall;

            resize(emotion, emotionSmall, Size(60, 60));

            int iconX = face.x + face.width + 5;
            int iconY = face.y;

            if (iconX + 60 < frame.cols &&
                iconY + 60 < frame.rows)
            {
                emotionSmall.copyTo(
                    frame(Rect(iconX, iconY, 60, 60))
                );
            }

            // Сохраняем только вырезанные лица
            if (recording && frameCounter % 5 == 0)
            {
                Mat colorFace = frame.clone()(face);

                stringstream filename;

                filename << "video_faces/face_"
                         << setw(5) << setfill('0')
                         << imageCounter++ << ".jpg";

                imwrite(filename.str(), colorFace);
            }
        }

        // Информация о количестве лиц
        putText(frame,
                "Faces: " + to_string(faces.size()),
                Point(20, 35),
                FONT_HERSHEY_SIMPLEX,
                0.8, Scalar(255, 255, 0), 2);

        if (recording)
        {
            putText(frame, "RECORDING",
                    Point(20, 70),
                    FONT_HERSHEY_SIMPLEX,
                    0.8, Scalar(0, 0, 255), 2);
        }

        imshow("Face Detection", frame);

        char key = (char)waitKey(30);

        // Начало и остановка сохранения
        if (key == 'r' || key == 'R')
        {
            recording = !recording;

            if (recording)
                cout << "Saving faces started!" << endl;
            else
                cout << "Saving faces stopped!" << endl;
        }

        if (key == 'q' || key == 'Q')
            break;

        frameCounter++;
    }

    camera.release();
    destroyAllWindows();

    return 0;
}

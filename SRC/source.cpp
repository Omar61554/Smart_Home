#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "Serial module/SerialPort.h"

using namespace cv;
using namespace std;

int main()
{
    // Load the cascade classifiers from the XML files
    CascadeClassifier face_cascade, body_cascade, hand_cascade;
    if (!face_cascade.load("Resources/haarcascade_frontalface_default.xml") 
    || !body_cascade.load("Resources/haarcascade_upperbody.xml") 
    || !hand_cascade.load("Resources/cascadehand.xml"))
    {
        // cout << "Failed to load cascade classifier." << endl;
        return -1;
    }

    // Create a SerialPort object to establish a connection with the Arduino board
    char portName[] = "COM4"; // Replace "COM4" with the name of the serial port connected to your Arduino board
    SerialPort arduino(portName);

    // Check if the serial port is opened successfully
    if (!arduino.isConnected())
    {
        // cout << "Error: Could not open serial port." << endl;
        return -1;
    }

    // Open the default camera
    VideoCapture cap(0);

    // Check if the camera was opened successfully
    if (!cap.isOpened())
    {
        //  cout << "Failed to open the camera." << endl;
        return -1;
    }

    bool hand_detected = false, face_detected = false, body_detected = false, lights_on = false, lights2 = false;
    int counter = 0, counter2 = 0;

    while (true)
    {
        // Capture a frame from the camera
        Mat frame;
        if (!cap.read(frame))
        {
            //  cout << "Failed to capture a frame." << endl;
            break;
        }

        // Convert the frame to grayscale
        Mat gray, blur, canny, dilation;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        GaussianBlur(gray, blur, Size(5, 5), 4, 4);

        // Detect objects in the grayscale image using the cascade classifiers
        vector<Rect> faces, bodies, hands;
        face_cascade.detectMultiScale(blur, faces, 1.1, 10);
        body_cascade.detectMultiScale(blur, bodies, 1.2, 10);
        hand_cascade.detectMultiScale(blur, hands, 1.3, 10);
        if (bodies.size() > 0 && faces.size() > 0)
        {
            if (!body_detected && !face_detected)
            {
                body_detected = true;
                face_detected = true;

                counter2++;
                //  cout << "Face and hand detected. Counter: " << counter << endl;
            }
        }
        else
        {
            body_detected = false;
            face_detected = false;
        }

        // Reset the counter every 10 frames (1 second)
        if (counter2 > 0 && counter2 % 10 == 0)
        {
            counter2 = 0;
        }

        // Turn on the lights if the counter is even, turn off the lights if the counter is odd
        if (counter2 % 2 == 0)
        {
            if (!lights2)
            {
                char command[] = "3";                              // Command to turn lights on
                arduino.writeSerialPort(command, sizeof(command)); // Send command to turn lights on
                lights2 = true;
                //  cout << "Lights turned on." << endl;
            }
        }
        else
        {
            if (lights2)
            {
                char command[] = "4";                              // Command to turn lights off
                arduino.writeSerialPort(command, sizeof(command)); // Send command to turn lights off
                lights2 = false;
                //  cout << "Lights turned off." << endl;
            }
        }
        waitKey(70);
        // cout << counter2<<endl;

        // Draw the contours around the detected objects
        for (size_t i = 0; i < faces.size(); i++)
        {
            rectangle(frame, faces[i], Scalar(200, 255, 0), 2);
        }

        for (size_t i = 0; i < bodies.size(); i++)
        {
            rectangle(frame, bodies[i], Scalar(255, 0, 0), 2);
        }

        for (size_t i = 0; i < hands.size(); i++)
        {
            rectangle(frame, hands[i], Scalar(0, 160, 255), 2);
        }

        // Check if a face and a hand are detected
        if (faces.size() > 0 && hands.size() > 0)
        {
            if (!face_detected && !hand_detected)
            {
                face_detected = true;
                hand_detected = true;
                counter++;
                //  cout << "Face and hand detected. Counter: " << counter << endl;
            }
        }
        else
        {
            face_detected = false;
            hand_detected = false;
        }

        // Reset the counter every 10 frames (1 second)
        if (counter > 0 && counter % 10 == 0)
        {
            counter = 0;
        }

        // Turn on the lights if the counter is even, turn off the lights if the counter is odd
        if (counter % 2 == 0)
        {
            if (!lights_on)
            {
                char command[] = "1";                              // Command to turn lights on
                arduino.writeSerialPort(command, sizeof(command)); // Send command to turn lights on
                lights_on = true;
                //  cout << "Lights turned on." << endl;
            }
        }
        else
        {
            if (lights_on)
            {
                char command[] = "0";                              // Command to turn lights off
                arduino.writeSerialPort(command, sizeof(command)); // Send command to turn lights off
                lights_on = false;
                //  cout << "Lights turned off." << endl;
            }
        }

        // Delay for 10 frames (100ms) before checking the objects again
        waitKey(1);

        // Display the frame
        imshow("Camera", frame);

        // Wait for a key press
        if (waitKey(1) == 'q')
        {
            break;
        }
    }

    // Turn off the lights before closing the serial port
    char command[] = "4";                              // Command to turn lights off
    arduino.writeSerialPort(command, sizeof(command)); // Send command to turn lights off
                                                       // cout << "Lights turned off." << endl;

    // Close the serial port    arduino.~SerialPort();

    // Release the camera and destroy the windows
    cap.release();
    // destroyAllWindows();

    return 0;
}
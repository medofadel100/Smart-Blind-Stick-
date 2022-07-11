# USAGE
# python3 object_detection_with_Text_to_Speech.py --prototxt MobileNetSSD_deploy.prototxt.txt --model MobileNetSSD_deploy.caffemodel --source webcam

# import the necessary packages
from imutils.video import VideoStream
import numpy as np
import sys
import argparse
import imutils
import time
import cv2
import vlc
from translate import Translator
from gtts import gTTS   
import os 

last = 0

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
# ap.add_argument("--prototxt", required=True,
#   help="path to Caffe 'deploy' prototxt file")
# ap.add_argument("--model", required=True,
#   help="path to Caffe pre-trained model")
# ap.add_argument("--source", required=True, 
#   help="Source of video stream (webcam/host)")
ap.add_argument("-c", "--confidence", type=float, default=0.2,
    help="minimum probability to filter weak detections")
args = vars(ap.parse_args())

# initialize the list of class labels MobileNet SSD was trained to
# detect, then generate a set of bounding box colors for each class
CLASSES = ["background", "aeroplane", "bicycle", "bird", "boat",
    "bottle", "bus", "car", "cat", "chair", "cow", "diningtable",
    "dog", "horse", "motorbike", "person", "pottedplant", "sheep",
    "sofa", "train", "tvmonitor"]
      
COLORS = np.random.uniform(0, 255, size=(len(CLASSES), 3))

# load our serialized model from disk
print("[INFO] loading model...")
net = cv2.dnn.readNetFromCaffe("MobileNetSSD_deploy.prototxt.txt", "MobileNetSSD_deploy.caffemodel")

# initialize the video stream, allow the cammera sensor to warmup,
print("[INFO] starting video stream...")

vs = cv2.VideoCapture(0)

time.sleep(2.0)

detected_objects = []
# loop over the frames from the video stream
while True:
    # grab the frame from the threaded video stream and resize it
    # to have a maximum width of 400 pixels
    ret, frame = vs.read()
    
    
    frame = imutils.resize(frame, width=800)
        
    # grab the frame dimensions and convert it to a blob
    (h, w) = frame.shape[:2]
    blob = cv2.dnn.blobFromImage(cv2.resize(frame, (300, 300)),
        0.007843, (300, 300), 127.5)

    # pass the blob through the network and obtain the detections and
    # predictions
    net.setInput(blob)
    detections = net.forward()
    
    # loop over the detections
    for i in np.arange(0, detections.shape[2]):
        # extract the confidence (i.e., probability) associated with
        # the prediction
        confidence = detections[0, 0, i, 2]

        # filter out weak detections by ensuring the `confidence` is
        # greater than the minimum confidence
        if confidence > args["confidence"]:
            # extract the index of the class label from the
            # `detections`, then compute the (x, y)-coordinates of
            # the bounding box for the object
            idx = int(detections[0, 0, i, 1])
            box = detections[0, 0, i, 3:7] * np.array([w, h, w, h])
            (startX, startY, endX, endY) = box.astype("int")

            # draw the prediction on the frame
            label = "{}".format(CLASSES[idx],
                confidence * 100)
            detected_objects.append(label)
            cv2.rectangle(frame, (startX, startY), (endX, endY),
                COLORS[idx], 2)
            y = startY - 15 if startY - 15 > 15 else startY + 15
            cv2.putText(frame, label, (startX, y),
                cv2.FONT_HERSHEY_SIMPLEX, 0.5, COLORS[idx], 2)
            if (last != label):
                print (label)
                last = label
                
                
                
                translator= Translator(from_lang="english",to_lang="arabic")
                translation = translator.translate(label)
                print(translation)
                
                
                language = 'en'
                myobj = gTTS(text=translation, lang=language, slow=False) 
                myobj.save("output.mp3") 
  
                # Play the converted file 
                #os.system("start output.mp3")
                # creating vlc media player object
                media = vlc.MediaPlayer("output.mp3")
 
                # start playing video
                media.play()
    
    # show the output frame
    #cv2.imshow("Frame", frame)
    key = cv2.waitKey(1) & 0xFF

    # if the `q` key was pressed, break from the loop
    if key == ord("q"):
        break


# do a bit of cleanup
cv2.destroyAllWindows()

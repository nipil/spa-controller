#!/usr/bin/env python3

# OpenCV 3.2 from distro
# apt-get install sudo apt-get install python3-opencv
# ./sync_img.py ../capture/boot_buttons.mp4 result/rising_clock.txt 

import os
import sys
import cv2

from spa_blocks import blockize

class App:

    OUTPUT_DIR = "screenshots"

    FIRST_B_EVENT_USEC = 37075311
    FIRST_V_EVENT_SEC = 3.05 #00:00:03:05

    BLOCKS = [ 1, 7, 7, 7, 7, 2, 2, 2, 1, 8, 8, 8, 6, 2, 2, 6 ]

    def __init__(self, video_file, event_file):
        self.DELTA_VIDEO_EVENT_USEC = int(self.FIRST_B_EVENT_USEC - self.FIRST_V_EVENT_SEC * 1000000)
        print("delta_usec", self.DELTA_VIDEO_EVENT_USEC, file=sys.stderr)

        self.video_file = video_file
        self.event_file = event_file

        self.event_handle = None
        self.vidcap = None
        self.current_image = None


    def search_frame(self, event_usec):
        success, self.current_image = self.vidcap.read()
        while success:
            # get event equivalent usec frame time
            frame_usec = int(self.vidcap.get(cv2.CAP_PROP_POS_MSEC) * 1000)
            adj_frame_usec = frame_usec + self.DELTA_VIDEO_EVENT_USEC
            print("event_usec", event_usec, "frame_usec", frame_usec, "adj_frame_usec", adj_frame_usec, end='', file=sys.stderr)
            # decide action
            if adj_frame_usec >= event_usec: 
                print(" found", file=sys.stderr)
                break
            print(" skipped", file=sys.stderr)
            success, self.current_image = self.vidcap.read()
        else:
            print("error getting video frame", file=sys.stderr)
        return success

    def frame_embed_bits(self, spa_bits, dsp_bits):
        spa_text = "SPA {}".format(" ".join(blockize(spa_bits)))
        dsp_text = "DSP {}".format(" ".join(blockize(dsp_bits)))
        print(spa_text)
        print(dsp_text)
        font = cv2.FONT_HERSHEY_SIMPLEX
        x = 10
        y = 500
        white = (255,255,255)
        cv2.putText(self.current_image, spa_text, (x,y), font, 1, white, 2, cv2.LINE_AA)
        cv2.putText(self.current_image, dsp_text, (x,y+100), font, 1, white, 2, cv2.LINE_AA)

    def prepare_output(self):
        try:
            os.mkdir(self.OUTPUT_DIR)
        except FileExistsError:
            pass

    def process(self):
        with open(self.event_file) as events:
            self.vidcap = cv2.VideoCapture(self.video_file)
            # parse events
            for event in events:
                # skip events up to start of video
                event_tokens = event.split(" ")
                event_usec = int(event_tokens[5])
                if event_usec < self.DELTA_VIDEO_EVENT_USEC:
                    print("event_usec", event_usec, "skipped", file=sys.stderr)
                    continue
                # skip frames up to event
                if not self.search_frame(event_usec):
                    # if frame not found, stop processing
                    break
                # process_frame
                self.frame_embed_bits(event_tokens[1], event_tokens[3])
                img_name = "{}/{:012d}.jpg".format(self.OUTPUT_DIR, event_usec)
                cv2.imwrite(img_name, self.current_image)

    def run(self):
        self.prepare_output()
        self.process()

if __name__ == '__main__':
    if len(sys.argv) == 3:
        os.chdir
        app = App(sys.argv[1], sys.argv[2])
        app.run()
    else:
        print("Usage:", sys.argv[0], "videofile", "eventfile")





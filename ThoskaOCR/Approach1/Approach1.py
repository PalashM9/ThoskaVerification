def perform_OCR():
    from PIL import Image
    import pytesseract
    import cv2

    pytesseract.pytesseract.tesseract_cmd = r"C:\Program Files\Tesseract-OCR\tesseract.exe"
    # The function opens the RTSP stream and starts reading frames in a loop.
    cap = cv2.VideoCapture('rtsp://192.168.0.112:8554/mjpeg/1')
    while True:
        _, frame = cap.read()
        if _:
            assert not isinstance(frame, type(None)), 'frame not found'
        # It then extracts the height and width of the frame and sets the region of interest (ROI) to the entire frame
        imgH, imgW, _ = frame.shape
        x1, y1, w1, h1 = 0, 0, imgH, imgW
        # The image_to_string() method of pytesseract is called to perform OCR on the frame, and the result is stored in the imgchar variable.
        imgchar = pytesseract.image_to_string(frame)
        imgboxes = pytesseract.image_to_boxes(frame)
        # Iterates through each bounding box, extracts the coordinates of the box, and draws a rectangle around it using OpenCV's rectangle() method.
        for boxes in imgboxes.splitlines():
            boxes = boxes.split(' ')
            x, y, w, h = int(boxes[1]), int(boxes[2]), int(boxes[3]), int(boxes[4])
            cv2.rectangle(frame, (x, imgH - y), (w, imgH - h), (0, 0, 255), 3)
            cv2.putText(frame, imgchar, (x1 + int(w1 / 50), y1 + int(h1 / 50)), cv2.FONT_HERSHEY_COMPLEX, 0.7, (0, 0, 255),
                        2)
            cv2.imshow('text', frame)
            print(imgchar)
            if cv2.waitKey(2) & 0xFF == ord('q'):
                break
    cap.release()
    cv2.destroyAllWindows()

if __name__ == '__main__':
    perform_OCR('PyCharm')

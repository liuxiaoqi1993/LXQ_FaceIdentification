#!/usr/bin/env python 
# -*- coding:utf-8 -*-
import cv2
import numpy as np
from PIL import Image,ImageFilter
import skindetetc as my
RGB_RED = (255,0,0)
RGB_GREEN = (0,255,0)
RGB_BLUE = (0,0,255)
RGB_YELLOW = (255,255,0)
S_SIZE = (480,320)
S_RATIO = 0.65
S_ORIGIN_POINT = (0,0)
def _remove_background(frame):
    fgbg = cv2.createBackgroundSubtractorMOG2()
    # fgmask = bgModel.apply(frame)
    fgmask = fgbg.apply(frame)
    # kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3))
    # res = cv2.morphologyEx(fgmask, cv2.MORPH_OPEN, kernel)
    kernel = np.ones((3, 3), np.uint8)
    fgmask = cv2.erode(fgmask, kernel, iterations=1)
    res = cv2.bitwise_and(frame, frame, mask=fgmask)
    return res

def _bodyskin_detetc(frame):
    # 肤色检测: YCrCb之Cr分量 + OTSU二值化
    ycrcb = cv2.cvtColor(frame, cv2.COLOR_BGR2YCrCb) # 分解为YUV图像,得到CR分量
    (_, cr, _) = cv2.split(ycrcb)
    cr1 = cv2.GaussianBlur(cr, (5, 5), 0) # 高斯滤波
    _, skin = cv2.threshold(cr1, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)  # OTSU图像二值化
    return skin

def convexhull_demo():
    cap = cv2.VideoCapture(0)
    while True:
        '''
         图片处理
         背景处理
         皮肤检测    
         二值化
        '''
        _,frame = cap.read()
        frame = cv2.bilateralFilter(frame, 5, 50, 100)  # 双边滤波
        image = Image.fromarray(frame)
        image = image.transpose(Image.FLIP_LEFT_RIGHT)
        #缩略图
        image.thumbnail(S_SIZE,Image.ANTIALIAS)
        array = np.asarray(image)
        imgcopy = array.copy()

        #选取兴趣区域
        w,h = int(np.rint(S_SIZE[0]*S_RATIO)),int(np.rint(S_SIZE[1]*S_RATIO))
        arraycrop = imgcopy[:h,:w]
        cv2.rectangle(imgcopy, S_ORIGIN_POINT,(w,h), color=RGB_RED, thickness=2)
        array = _remove_background(arraycrop)
        thresh = _bodyskin_detetc(array)

        #二值图显示
        cv2.imshow('thresh',thresh)

        # 寻找轮廓
        copyImage, contours, hireachy = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        # 画出所有轮廓
        # cv2.drawContours(imgcopy,contours, -1, (255,0,0),1)

        # 找到最大的轮廓
        largecontours = max(contours,key = lambda x:cv2.contourArea(x))
        cv2.drawContours(imgcopy, (largecontours), -1, RGB_RED, 1)

        # 轮廓外矩形
        aroundrect = cv2.boundingRect(largecontours)
        x, y, w, h = aroundrect
        cv2.rectangle(imgcopy, (x, y), (x + w, y + h),color= RGB_RED, thickness=2)


        # 找到凸包
        hull = cv2.convexHull(largecontours)


        # 凸性检测 检测一个曲线是不是凸的
        ishull = cv2.isContourConvex(hull)


        # 凸缺陷
        hull = cv2.convexHull(largecontours, returnPoints=False)
        defects = cv2.convexityDefects(largecontours, hull)

        #计算轮廓中心点
        #M = cv2.moments(largecontours)
        #cx = int(M['m10'] / M['m00'])
        #cy = int(M['m01'] / M['m00'])
        if defects is not None:
            for j in range(defects.shape[0]):
                s, e, f, d = defects[j, 0]
                start = tuple(largecontours[s][0])
                end = tuple(largecontours[e][0])
                far = tuple(largecontours[f][0])
                cv2.line(imgcopy, start, end, RGB_YELLOW, 3)
                cv2.circle(imgcopy, far, 5, RGB_GREEN, -1)
                #cv2.line(imgcopy, start, (cx,cy), RGB_YELLOW, 3)
                #cv2.circle(imgcopy, (cx,cy), 20, RGB_GREEN)

        #结果显示
        cv2.imshow('convexhull image', imgcopy)
        if cv2.waitKey(1) & ord('q')==0xff:
            break
    cap.release()
    cv2.destroyAllWindows()



if __name__ == '__main__':
    convexhull_demo()
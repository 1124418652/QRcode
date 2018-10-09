# -*- coding: utf-8 -*-
"""
# project: the algrithom of QR code recognition
# author:  xhj
# email:   1124418652@qq.com
# date:    2018/ 9/18
"""

import os
import cv2
import numpy as np 

def open_img(path, type = "RGB"):
	img = cv2.imread(path)

	if "bgr" != type.lower():
		img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

	return img


def main():
	path = os.path.join(os.path.abspath("."), "../../prectice_in_book/src/er.jpg")
	res_gray = open_img(path, "gray")
	bina = cv2.threshold(res_gray, 0, 255, cv2.THRESH_BINARY|cv2.THRESH_OTSU)
	contours = cv2.findContours(bina[1], cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
	print(len(contours))
	cv2.imshow("res_gray", res_gray)
	cv2.imshow("threshold", bina[1])
	cv2.waitKey(0)

if __name__ == '__main__':
	main()

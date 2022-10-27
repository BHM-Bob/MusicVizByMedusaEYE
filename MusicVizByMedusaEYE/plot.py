'''
Author: BHM-Bob G 2262029386@qq.com
Date: 2022-07-08 00:23:22
LastEditors: BHM-Bob
LastEditTime: 2022-10-27 18:15:33
Description: Draw MeduzaEye
'''
import turtle, numpy as np, time

dotsAP,  dotsBP = [], []
sideSize, interval, loopTimes = 800, 10, 3

def CacuInterAndSlope(dotsAX:np.ndarray, dotsAY:np.ndarray, dotsBX:np.ndarray, dotsBY:np.ndarray):
    """func for cacu y = a * x + b from(x1, y1) and (x2, y2)"""
    linesSlope = (dotsAY-dotsBY)/(dotsAX-dotsBX)
    linesIntercept = dotsAY - linesSlope * dotsAX
    # return b, a
    return linesIntercept, linesSlope
def DrawLinesWithXY(dotsA:list[np.ndarray], dotsB:list[np.ndarray], loopTime:int, sideIdx:int):
    """func for put dots to global var with given dots(x, y)"""
    global dotsAP,  dotsBP
    linesIntercept, linesSlope = CacuInterAndSlope(dotsA[0], dotsA[1], dotsB[0], dotsB[1])
    for idx in range(dotsA[0].shape[0]):
        dotsAP.append([dotsA[0][idx], dotsA[1][idx], loopTime, sideIdx])
        dotsBP.append([dotsB[0][idx], dotsB[1][idx], loopTime, sideIdx])
    return linesIntercept, linesSlope
def CacuIntercetingDots(linesIntercept:np.ndarray, linesSlope:np.ndarray, i:int):
    """func for cacu intersection dots between a group of lines in format of y = a * x + b\n
    the useful dots exist between the two adjacent lines\n
    i: idx of sides """
    # create seq, data like 0(padding), b1, b2 or b1, b2, 0(padding)
    seqSize = linesIntercept.shape[0]
    Bi = linesIntercept[0: seqSize - 1]
    Ai = linesSlope[0: seqSize - 1]
    Bj = linesIntercept[1: seqSize]
    Aj = linesSlope[1: seqSize]
    # x = (Bi - Bj) / (Aj - Ai), i = j - 1
    x = (Bi - Bj) / (Aj - Ai)
    # y = Ai * x + Bi
    y = Ai * x + Bi
    # select data for side i, get idx
    xMask = x >= (sideSize / 2)
    yMask = y >= (sideSize / 2)
    xMaskVal, yMaskVal = 1 if i <= 1 else 0, 1 if (1 <= i and i <= 2) else 0
    mask = (xMask == xMaskVal) * (yMask == yMaskVal)
    # return selected data in format of X, Y
    return [x[mask], y[mask]]

# usefull var for fast understanding
sideSteps = int(sideSize / interval)
fromInt2S = np.arange(interval, sideSteps*interval, interval)
fromS2Int = sideSteps*interval - fromInt2S
all0 = np.zeros(sideSteps-1)
allS = np.ones(sideSteps-1)*sideSize
intercetingDotsPack = [None] * 4
intercetingDotsPack[0] = [fromInt2S, all0]
intercetingDotsPack[1] = [allS, fromInt2S]
intercetingDotsPack[2] = [fromS2Int, allS]
intercetingDotsPack[3] = [all0, fromS2Int]
# draw loops
for loopTime in range(0, 3):
    sideDots = [intercetingDotsPack[i] for i in [0,1,2,3,0]]
    intercetingDotsPack.clear()
    for i in range(4):
        linesIntercept, linesSlope = DrawLinesWithXY(sideDots[i], sideDots[i+1], loopTime, i)
        dots = CacuIntercetingDots(linesIntercept, linesSlope, i)
        intercetingDotsPack.append(dots)
# GUI

dotsLayer = 2
for dotsSide in range(4):
    print(dotsSide, len(list(filter(lambda x : x[-2] == dotsLayer and x[-1] == dotsSide, dotsAP))))

turtle.screensize(canvwidth=sideSize, canvheight=sideSize, bg='black')
turtle.pencolor('white')
turtle.speed(0)
for dotsA, dotsB in zip(dotsAP,  dotsBP):
    # if dotsA[-2] == 2:
        turtle.penup()
        turtle.goto(dotsA[0]-sideSize/2,dotsA[1]-sideSize/2)
        turtle.pendown()
        turtle.goto(dotsB[0]-sideSize/2,dotsB[1]-sideSize/2)
        # time.sleep(0.5)
time.sleep(999)
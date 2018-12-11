
/*
IplImage *img;
//int preTime;
clock_t preTime;

CvPoint prePoint;

void mymouse(int event, int x, int y, int flag, void* param)
{
if (event == CV_EVENT_LBUTTONDOWN) {
preTime = clock();
prePoint = cvPoint(x, y);
cvCircle(img, cvPoint(x, y), 8, cvScalar(0, 0, 255));
}
if (event == CV_EVENT_LBUTTONUP) {
cout << clock() - preTime<<endl;
int thick = ((int)clock() - preTime)/50;
cvLine(img,prePoint, cvPoint(x, y), cvScalar(255, 0, 0),thick);
cvCircle(img, cvPoint(x, y), 8, cvScalar(0, 0, 255));
}
if (event == CV_EVENT_MOUSEMOVE) {
//cvCircle(img, cvPoint(x, y), 2, cvScalar(0, 255, 0));
}
if (event == CV_EVENT_RBUTTONDOWN) {
cvFloodFill(img, cvPoint(x, y), cvScalar(133, 133, 0));
}
cvShowImage("小畫家", img);
}

int win_size = 10;
const int MAX_COUNT = 500;
CvPoint2D32f* points[2] = { 0,0 }, *swap_points;
char* status = 0;
int count1 = 0;
int flags;
int init = 1;
*/
/*
//VideoCapture video(0);
VideoCapture video("MOV_0024.mp4");
if (!video.isOpened()) {
return -1;
}
Size videoSize = Size((int)video.get(CV_CAP_PROP_FRAME_WIDTH), (int)video.get(CV_CAP_PROP_FRAME_HEIGHT));
namedWindow("video demo", CV_WINDOW_AUTOSIZE);
Mat videoFrame;
//IplImage *img;
while (true) {
video >> videoFrame;
if (videoFrame.empty()) {
break;
}
//cvtColor(videoFrame, videoFrame, CV_BGR2YCrCb);
//cvCvtColor(videoFrame, img, CV_BGR2YCrCb);
//inRange(videoFrame, );

//cvInRangeS(img, cvScalar(0, 137, 11), cvScalar(255, 177, 127), grey);
imshow("video demo", videoFrame);
waitKey(33);
}
return 0;
*/
/*
CvCapture *capture;
IplImage *frame;

capture = cvCaptureFromCAM(CV_CAP_ANY);

if (!capture) {
cout << "fail to open cam"<<endl;
return -1;
}
cvNamedWindow("frame", 0);
cvNamedWindow("mywindow", CV_WINDOW_AUTOSIZE);
while (1) {
waitKey(30);
//frame = cvQueryFrame(capture);
//cvShowImage("frame", frame);

IplImage* frame = cvQueryFrame(capture);
if (!frame) {
//fprintf(stderr, "ERROR: frame is null...\n");
cout << "2fail to open cam" << endl;
getchar();
break;
}
cvShowImage("mywindow", frame);
}*/
/*
img = cvCreateImage(cvSize(600, 400), 8, 3);
cvSet(img, cvScalar(255, 255, 255));
cvShowImage("小畫家", img);

cvSetMouseCallback("小畫家", mymouse);
cvWaitKey(0);


img = cvLoadImage("face3.jpg");
cvShowImage("a", img);
cvWaitKey(0);

cvCircle(img, cvPoint(100, 100), 50, cvScalar(255, 0, 0));
cvLine(img, cvPoint(100, 100), cvPoint(200, 200), cvScalar(0, 255, 0));
cvRectangle(img, cvPoint(100, 100), cvPoint(200, 200), cvScalar(0, 0, 255));
cvShowImage("a", img);
cvWaitKey(0);
*/
/*
//抓取攝影機
VideoCapture cap(0);
//嘗試開啟攝影機
if (!cap.isOpened()) return -1;

//用矩陣紀錄抓取的每張frame
Mat frame;
Mat GaussianBlur;
//建立一個視窗,名稱為camera
namedWindow("camera", 1);
for (;;)
{
//把取得的影像放置到矩陣中
cap >> frame;
blur(frame, GaussianBlur, Size(5, 5));

//GaussianBlur(frame, GaussianBlur, Size(5, 5), 0, 0);
//GaussianBlur()
IplImage *img = &IplImage(frame);
IplImage *grey = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
cvCvtColor(img, img, CV_BGR2YCrCb);
cvInRangeS(img, cvScalar(0, 137, 11), cvScalar(255, 177, 127), grey);

//顯示frame到camera名稱的視窗
cvShowImage("a", grey);
//imshow("frame", frame);
if (waitKey(30) >= 0) break;
}
system("PAUSE");
*/
/*
#pragma region canvas
static Size inputSize(input.size());
Point nowP((inputSize.width - avrX)*vanvasSize.width / inputSize.width, (avrY)*vanvasSize.height / inputSize.height);
static Point preP;
//cout << nowP.x << "," << nowP.y << endl;
if (avrX >= 0 && avrX < vanvasSize.width&&avrY >= 0 && avrY < vanvasSize.height) {
line(vanvas, nowP, preP, Scalar(255, 255, 255), 2);
//putText(vanvas, "Point=(" + nowP.x + ',' + nowP.y + ')', Point(10, 10), FONT_HERSHEY_DUPLEX,1, Scalar(255, 255, 255));
}
preP = nowP;
imshow("vanvas", vanvas);
#pragma endregion
}*/
/* Canvas

void mymouse(int event, int x, int y, int flag, void* param);

Size vanvasSize(1000, 1000);
Mat vanvas(vanvasSize, CV_8U, Scalar(0, 0, 0));
enum class BoundReCalState { none, busy, done };
BoundReCalState boundState;
int boundCalCount = 0;
const int maxBoundCalCount = 50;
=======================================================

#pragma region clearVanvas
cvSetMouseCallback("vanvas", mymouse);
#pragma endregion
=======================================================
#pragma region boundCalculate
//static long long int H = 0, S = 0, V = 0;
//static long long int H = 0;
if (boundState == BoundReCalState::busy) {
if ((boundCalCount > 0)) {
Scalar meanHSV = mean(frame, mask );
boundSum += meanHSV;
cout <<"i="<<boundCalCount <<", H=" << meanHSV.val[0] << ",S=" << meanHSV.val[1] << ",V=" << meanHSV.val[2] <<"sum="<<boundSum.val[0]<< endl;
boundCalCount--;
//H += meanHSV.val[0];
}
else if (boundCalCount <= 0) {
boundState = BoundReCalState::done;
//Scalar size(maxBoundCalCount, maxBoundCalCount, maxBoundCalCount);
boundSum.val[0] = boundSum.val[0] / (maxBoundCalCount);
boundSum.val[1] = boundSum.val[1] / (maxBoundCalCount);
boundSum.val[2] = boundSum.val[2] / (maxBoundCalCount);
//boundSum /= size;

//boundSum.val[0] /= (double)(maxBoundCalCount - 2);
//cout << boundSum.val[0] <<","<<H<< endl;

lowerBound = boundSum - sigma;
higherBound = boundSum + sigma;
//cout << "scalor=" << size.val[0]<<endl;
cout << "average, H=" << boundSum.val[0] << ",S=" << boundSum.val[1] << ",V=" << boundSum.val[2] << endl;
boundSum = Scalar(0, 0, 0);
}
}

#pragma endregion
=======================================================
void mymouse(int event, int x, int y, int flag, void* param)
{
if (event == CV_EVENT_LBUTTONDOWN) {
vanvas=Mat(vanvasSize, CV_8U, Scalar(0, 0, 0));
}
if (event == CV_EVENT_RBUTTONDOWN) {
if (boundState != BoundReCalState::busy) {
boundState = BoundReCalState::busy;
boundCalCount = maxBoundCalCount;
}
}
}
*/

/*
#pragma region Segment test
		Segment seg(Point2d(5, 0), Point2d(2, 1));
		if (coordinate.leftWall.collide(seg)) {
			line(input, coordinate.transToPixel(Point2d(2, 1)), coordinate.transToPixel(Point2d(5, 0)),  Scalar(255, 255, 0), 2);
			//Point2d interSection = coordinate.leftWall.findInterSection(seg);
			//interSection = coordinate.transToPixel(interSection);
			//circle(input, interSection, 5, Scalar(255, 255, 0),2);
			//Point2d projectPoint = seg.findMirrorPoint(coordinate.leftWall.line);
			//projectPoint = coordinate.transToPixel(projectPoint);
			//circle(input, projectPoint, 5, Scalar(255, 255, 0), 2);
			Point2d projectPoint = seg.findProjectPoint(coordinate.leftWall.line);
			projectPoint = coordinate.transToPixel(projectPoint);
			circle(input, projectPoint, 5, Scalar(255, 255, 0), 2);

			Segment reflectSeg = seg.findReflectSeg(coordinate.leftWall);
			line(input, coordinate.transToPixel(reflectSeg.p1), coordinate.transToPixel(reflectSeg.p2), Scalar(255, 255, 0), 2);
			if (coordinate.rightWall.collide(reflectSeg)) {
				Point2d interSection = coordinate.rightWall.findInterSection(reflectSeg);
				interSection = coordinate.transToPixel(interSection);
				circle(input, interSection, 5, Scalar(255, 255, 0),2);

				Point2d projectPoint = reflectSeg.findProjectPoint(coordinate.rightWall.line);
				cout << "p1=" << projectPoint << endl;
				projectPoint = coordinate.transToPixel(projectPoint);
				circle(input, projectPoint, 5, Scalar(255, 255, 0), 2);

				Segment reflectSeg2 = reflectSeg.findReflectSeg(coordinate.rightWall);
				line(input, coordinate.transToPixel(reflectSeg2.p1), coordinate.transToPixel(reflectSeg2.p2), Scalar(255, 255, 0), 2);
				//cout << "p1=" << reflectSeg2.p1 << ", p2=" << reflectSeg2.p2 << endl;
			}
		}
#pragma endregion
		*/
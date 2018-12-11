#include <stdio.h>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>


//#include "SerialPort.h"

#define ARDUINO_WAIT_TIME 2000
#define MAX_DATA_LENGTH 255
#define HIT_BACK_DIST 5
#define TRUST_TRIGGER 2
#define DEFENSE_MODE false
#define ACTIVE_MODE true
#include <string>
#include <windows.h>
#include <stdlib.h>


#define Vector2d Point2d
#include <ctime>
using namespace cv;
using namespace std;

class SerialPort
{
private:
	HANDLE handler;
	bool connected;
	COMSTAT status;
	DWORD errors;
	clock_t lastHitTime;
	clock_t lastCalTime = 0;
	Point2d lastSendPoint;
	bool hitting;
public:
	SerialPort(char *portName);
	~SerialPort();

	int readSerialPort(char *buffer, unsigned int buf_size);
	bool writeSerialPort(char *buffer, unsigned int buf_size);
	bool isConnected(); 
	void sendDataToArduino(Point2d point, bool hit);
	void autoBack(Point2d center);
};

class Line
{
public:
	double a, b, c;
	Line();
	Line(Point2d p1, Point2d p2);
	double formula(Point2d newP);
	void print();
private:
};

class Segment
{
public:
	Segment();
	Segment(Point2d newP1, Point2d newP2);
	bool collide(Segment seg);					//detect if two segment colides (the parameter is the line)
	bool direction();							//detect if the direction of the segment vector are correct
	Point2d findInterSection(Segment seg);		//find the intersection result 
	Point2d findProjectPoint(Line line);
	Point2d findMirrorPoint(Line line);
	Segment findReflectSeg(Segment seg);
	double calInterSectionDist(Segment seg);
	double calLength();
	double calAngle( Segment seg);
	double calPointDist(Point point);
	Vector2d calUnitVec();
	Segment operator+(Point2d move);
	Segment operator-(Point2d move);
	Segment operator+(Segment move);
	Segment operator-(Segment move);
	Segment operator*(double t);
	Segment operator/(double t);

	Point2d p1;
	Point2d p2;
	Line line;
private:
};
class Shoot
{
public:
	int predictSize;
	Segment moveSeg;
	Vector2d frameSeg;
	Shoot();
	void pushNewPoint(Point2d newPoint);
	void updateFirstPoint(Point2d newPoint);
	void lineUpdate();
	double trustValue;
	double calTrustValue();
	bool trustSystem();
	Segment calAvrPredictLine();

private:
	double trustPool;
	bool workingFlag;
	Point2d firstPoint;
};
class Coordinate {								//use index 1 = origin
private:										//		0		3
	Segment leftLocal, rightLocal, buttomLocal;	//		d1		d3
	double d1, d2, d3;							//		1	d2	2
#pragma region init
	Point2d standardLocationPoint[4];
	//Point2d offset[4];
	bool initDone;
#pragma endregion
	double X_axisDist, Y_axisDist;
	double ballRadius, hitterRadius;
	bool setLocalPixelPoint();
	Point2d getAvrOffset();
public:
	Point2d locationPoint[4];
	Segment leftWall, rightWall, buttomWall;
	Shoot shoot;
	Coordinate(double newD1, double newD2, double newD3, double newX_axisDist, double newY_axisDist, double newBallRadius, double newHitterRadius);
	void init();
	//void updateLocationPoint();
	Point2d transToNumeral(Point2d input);
	Point2d transToPixel(Point2d input);
	void printOutWalls(Mat mat);
	bool checkDir(Segment seg);
	int getCollideWay(Segment seg, int lastWall);
	int collidePredict(Segment seg, vector<Point2d>& points);
	bool hittingWall();
	void show(Mat mat, Segment seg, Scalar scalar);
};

Point2d machineSize(38000, 28000);
Point2d moveSize(52, 33);
#define safeProtectDist 8.5   //warning should be update when you change the yDist

Point findColorCenter(Mat inputMat, Scalar lowerScaalr, Scalar higherScalar, double minArea);
int findMaxAreaIndex(vector<vector<Point>>& edges, double minArea);
string sendDataGenerate(Point2d point);
Vector2d calHitBackVec(Segment seg);

int main(int argc, const char** argv) {

	double preClock = clock();
	VideoCapture webcam;								//get camera container
	waitKey(50);										//delay for 50 ms
	webcam.open(0);										//choose webcam sourse
	waitKey(50);										//delay for 50 ms
	if (!webcam.isOpened()){//**EDITED**
		std::cout << "CANNOT OPEN CAM" << std::endl;
		return -1;
	}
	SerialPort arduino("\\\\.\\COM4");
	if (!arduino.isConnected()) {
		cout << "can't open serial" << endl; 
		return -1;
	}
	Mat input;
	Mat frame;
	Point2d prePoint = Point2d(-1, -1);
	Point2d ballPoint;
#pragma region setting
	Scalar lowerBound(40, 50, 50);								//range of HSV in CV =0~180, and green is about 120/360
	Scalar higherBound(70, 255, 255);
	Scalar cornerLowerBound(115, 50, 50);						//red = 115~135
	Scalar cornerHigherBound(135, 255, 255);					//green = 40~60
	Scalar sigma(5, 80, 100);									//blue = 10
	Scalar boundSum(0, 0, 0);									//human = 100~120

	Point corners[] = { Point(50,450),Point(510,450),Point(50,30),Point(510,30) };
						//p0         ,p1            ,p2          ,p3
	Size cornerSize(60, 60); //detection range
	Point2d leftSidePoint(5,-7);
	Point2d rightSidePoint(41,-7);

	Coordinate coordinate(60, 55, 60, 2.5, 8.5, 3.5, 4);
	                    //d1, d2, d3, xD , yD , BR , HR//color detect range
#pragma endregion
	while (true){
#pragma region video preprocess
		webcam >> input;										//put video into Mat
		frame = input.clone();
		GaussianBlur(frame, frame, Size(7, 7), 0, 0);			//Gaussian filter
																//blur(frame, frame, Size(7, 7));						//average blur
		cvtColor(frame, frame, COLOR_RGB2HSV);
#pragma endregion
#pragma region detectCorners
		for (int i = 0; i < 4; i++) {
			Rect cornerRect(Point(corners[i].x - cornerSize.width / 2, corners[i].y - cornerSize.height / 2), cornerSize);
			rectangle(input, cornerRect, Scalar(128, 255, 255), 2);
			Mat cornerROI = frame(cornerRect);

			Point center = findColorCenter(cornerROI, cornerLowerBound, cornerHigherBound, 10);
			if (center != Point(-1, -1)) {
				center.x += corners[i].x - cornerSize.width / 2;
				center.y += corners[i].y - cornerSize.height / 2;
				coordinate.locationPoint[i] = center;
				circle(input, center, 2, Scalar(0, 0, 255), 2);
			}
			else
				coordinate.locationPoint[i] = Point2d(-1, -1);;
		}
#pragma endregion
#pragma region detectBall
		coordinate.init();
		//dilate(mask, mask, Mat());								//expand the mask
		Point2d center = findColorCenter(frame, lowerBound, higherBound,100);    //filter pixels in range and seperate then by puting into mask
		if (center != Point2d(-1, -1)) {
			circle(input, center, 5, Scalar(0, 0, 255), 2);
			//coordinate.updateLocationPoint();
			ballPoint = coordinate.transToNumeral(center);
			char str[20];
			sprintf_s(str, "(%d,%d)", (int)ballPoint.x, (int)ballPoint.y);
			putText(input, str, center, CV_FONT_NORMAL, 1, (0, 0, 0));
		}
		else
			ballPoint = Point2d(-1, -1);
#pragma endregion

#pragma region moving update
		coordinate.printOutWalls(input);

		if (ballPoint != Point2d(-1, -1)) {
			if (prePoint == Point2d(-1, -1)) {									//the first point of one shoot
				coordinate.shoot.updateFirstPoint(ballPoint);
				prePoint = ballPoint;
			}
			else {
				//coordinate.shoot.updateFirstPoint(prePoint);
				Segment seg(prePoint, ballPoint);
				if (seg.calLength() > 1) {									//speed enough
					if (coordinate.checkDir(seg)) {
						coordinate.shoot.lineUpdate();
						if (!coordinate.hittingWall()){
						//if (true) {
							//do funtion: predict line, send data, etc
							coordinate.shoot.pushNewPoint(ballPoint);			//update now position
							coordinate.show(input, coordinate.shoot.moveSeg, Scalar(255, 255, 0));

							Vector2d newVec = ballPoint - prePoint;
							coordinate.shoot.frameSeg = 0.5*coordinate.shoot.frameSeg+0.5*newVec;
							
							Segment routeSeg(prePoint, ballPoint+ coordinate.shoot.frameSeg *20);
							coordinate.show(input, routeSeg, Scalar(255,0,0));

							prePoint = ballPoint;
#pragma region predictPoints
							vector<Point2d> collidePoints;						//array to keep interSections
							Segment forPredictSeg = coordinate.shoot.calAvrPredictLine();
							coordinate.collidePredict(forPredictSeg, collidePoints);
							Point2d lastPrintPoint = coordinate.transToPixel(coordinate.shoot.moveSeg.p2);
							Point2d nowPrintPoint;
							for (int i = 0; i < collidePoints.size(); i++) {
								nowPrintPoint = coordinate.transToPixel(collidePoints.at(i));
								circle(input, nowPrintPoint, 5, Scalar(255, 255, 0), 2);
								line(input, lastPrintPoint, nowPrintPoint, Scalar(255, 255, 0), 2);
								lastPrintPoint = nowPrintPoint;
							}
#pragma endregion
							if (!collidePoints.empty()) {								//last prediction
								if(DEFENSE_MODE&&(collidePoints.back().x<leftSidePoint.x))
									arduino.sendDataToArduino(leftSidePoint, false);
								else if(DEFENSE_MODE&&(collidePoints.back().x>rightSidePoint.x))
									arduino.sendDataToArduino(rightSidePoint, false);
								else {
									Segment hittingDist(coordinate.shoot.moveSeg.p2, collidePoints.back());
									if (hittingDist.calLength() < 18) { 				    //hit
										Vector2d hitBackVec = calHitBackVec(coordinate.shoot.moveSeg);
										arduino.sendDataToArduino(collidePoints.back() + hitBackVec, true);
									}
									else {
										if (coordinate.shoot.trustSystem())
											arduino.sendDataToArduino(collidePoints.back(), false);
									}
								}
							}
							//coordinate.shoot.updateFirstPoint(prePoint);
						}
						else
							coordinate.shoot.updateFirstPoint(ballPoint);
							//prePoint = Point2d(-1, -1);
					}
					else {                             //if the direction not currect, s
						prePoint = Point2d(-1, -1);
						arduino.sendDataToArduino(Point2d(moveSize.x/2,-7), false);
					}
				}									//if speed enough
				else {
					if (ACTIVE_MODE && ballPoint.y < 15 && ballPoint.y>0) {				//too close with ourside
						arduino.sendDataToArduino(ballPoint, true);
					}
					else
						prePoint = Point2d(-1, -1);
				}
			}	
		}
#pragma endregion
		cout << (clock() - preClock) / CLOCKS_PER_SEC << endl;
		preClock = clock();
		arduino.autoBack(Point2d(moveSize.x / 2, -7));
		imshow("origin", input);
		waitKey(2);
	}
	return 0;
}

Point findColorCenter(Mat inputMat, Scalar lowerScaalr, Scalar higherScalar,double minArea) {

	vector<vector<Point>> cornerContours;
	Mat cornerMask;
	inRange(inputMat, lowerScaalr, higherScalar, cornerMask);
	findContours(cornerMask, cornerContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	int maxAreaIndex = findMaxAreaIndex(cornerContours, minArea);
	if (maxAreaIndex >= 0) {
		Rect boundRect = boundingRect(cornerContours.at(maxAreaIndex));
		return Point(boundRect.x + boundRect.width / 2, boundRect.y + boundRect.height / 2);
	}
	return Point(-1, -1);
}
int findMaxAreaIndex(vector<vector<Point>> &edges,double minArea)
{
	try {
		int maxIndex = 0;
		double maxArea = 0;
		if (edges.size() == 0)
			return -1;
		for (int i = 0; i < edges.size(); i++) {
			double tempArea = contourArea(edges.at(i));
			if (tempArea > maxArea) {
				maxArea = tempArea;
				maxIndex = i;
			}
		}
		if(maxArea < minArea) return -1;
		return maxIndex;
	}
	catch(...){
		return -1;
	}
}
string sendDataGenerate(Point2d point) {
	//machineSize(39000, 28000);
	//moveSize(52, 33);
	int x = (point.x*machineSize.x / moveSize.x);
	int y = ((point.y+ safeProtectDist)*machineSize.y / moveSize.y);
	if (x < 0) x = 0;
	if (x > machineSize.x) x = machineSize.x;
	if (y < 0) y = 0;
	if (y > machineSize.y) y = machineSize.y;

	string output = "M,";
	output += to_string(x);
	output += ",";
	output += to_string(y);
	output += ",";
	output += to_string(x+y);
	output += ";";
	//cout << output << endl;
	//cout << '(' << x << ',' << y << ')' << endl;
	cout << output << endl;
	//return output.size();
	return output;
}
Vector2d calHitBackVec(Segment seg) {
	Vector2d vec = seg.calUnitVec() * HIT_BACK_DIST;
	return vec;
}

#pragma region Serial
SerialPort::SerialPort(char *portName)
{
	hitting = false;
	lastHitTime = 0;
	lastSendPoint = Point2d(-1, -1);
	this->connected = false;

	this->handler = CreateFileA(static_cast<LPCSTR>(portName),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (this->handler == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
		}
		else
		{
			printf("ERROR!!!");
		}
	}
	else {
		DCB dcbSerialParameters = { 0 };

		if (!GetCommState(this->handler, &dcbSerialParameters)) {
			printf("failed to get current serial parameters");
		}
		else {
			dcbSerialParameters.BaudRate = CBR_9600;
			dcbSerialParameters.ByteSize = 8;
			dcbSerialParameters.StopBits = ONESTOPBIT;
			dcbSerialParameters.Parity = NOPARITY;
			dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(handler, &dcbSerialParameters))
			{
				printf("ALERT: could not set Serial port parameters\n");
			}
			else {
				this->connected = true;
				PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
				Sleep(ARDUINO_WAIT_TIME);
			}
		}
	}
}

SerialPort::~SerialPort()
{
	if (this->connected) {
		this->connected = false;
		CloseHandle(this->handler);
	}
}

bool SerialPort::isConnected()
{
	return this->connected;
}
int SerialPort::readSerialPort(char *buffer, unsigned int buf_size)
{
	DWORD bytesRead;
	unsigned int toRead;

	ClearCommError(this->handler, &this->errors, &this->status);

	if (this->status.cbInQue > 0) {
		if (this->status.cbInQue > buf_size) {
			toRead = buf_size;
		}
		else toRead = this->status.cbInQue;
	}

	if (ReadFile(this->handler, buffer, toRead, &bytesRead, NULL)) return bytesRead;

	return 0;
}

bool SerialPort::writeSerialPort(char *buffer, unsigned int buf_size)
{
	DWORD bytesSend;

	if (!WriteFile(this->handler, (void*)buffer, buf_size, &bytesSend, 0)) {
		ClearCommError(this->handler, &this->errors, &this->status);
		return false;
	}
	else return true;
}

void SerialPort::sendDataToArduino(Point2d point, bool hit) {
	//if(getTime lastSendTime)
	double nowClock = lastCalTime = clock();
	if (hitting && ((nowClock - lastHitTime) / CLOCKS_PER_SEC < 0.5))
		return;
	else hitting = false;
	Segment seg(lastSendPoint, point);
	if (seg.calLength() < 1)							 //if displacement too short, break
		return;
	lastSendPoint = point;
	string data;
	if (hit && !hitting) {
		data = sendDataGenerate(point + Point2d(0, 5));  //extra y direction displacement
		hitting = true;									 //make sure this command have enough time to excute
		lastHitTime = nowClock;
	}
	else
		data = sendDataGenerate(point);

	char serialOut[40] = "";
	for (size_t i = 0; i < data.size(); i++)
		serialOut[i] = data.at(i);
	writeSerialPort(serialOut, data.size());
}
void SerialPort::autoBack(Point2d center) {
	double nowClock = clock();
	if ((nowClock - lastCalTime) / CLOCKS_PER_SEC > 0.5) {
		cout << "hitting\n";
		sendDataToArduino(center, false);
	}
}
#pragma endregion 
#pragma region line
Line::Line(Point2d p1, Point2d p2){
	a = p2.y - p1.y;
	b = p1.x - p2.x;
	c = -b*p1.y - a*p1.x;
}
Line::Line(){
	a = b = c = 0;
}
double Line::formula(Point2d newP){
	return a*newP.x + b*newP.y + c;
}
void Line::print() {
	printf_s("%lf X + %lf Y + %lf = 0\n", a, b, c);
}
#pragma endregion
#pragma region Segment
Segment::Segment(){
	line = Line(p1, p2);
}
Segment::Segment(Point2d newP1, Point2d newP2){
	p1 = newP1;
	p2 = newP2;
	line = Line(p1, p2);
}
bool Segment::collide(Segment seg) {										//detect if two segment colides (the parameter is the line)
	if (seg.line.formula(p1)*seg.line.formula(p2) > 0)						//mean the same side
		return false;
	else if(calPointDist(seg.p2)<calPointDist(seg.p1))
		return true;
	return false;
}
bool Segment::direction(){													//detect if the direction of the segment vector are correct
	if (p2.y - p1.y > 0)
		return true;
	return false;
}
Point2d Segment::findInterSection(Segment seg) {							//find the intersection of two segment
	double y = (line.c*seg.line.a - seg.line.c*line.a) / (seg.line.b*line.a - line.b*seg.line.a);
	double x = (-line.c - line.b*y) / line.a;								//calculate x value by y      *************************if line.a == 0 -> this code will crash
	return Point2d(x, y);
}
Point2d Segment::findProjectPoint(Line line) {								//find the project point of p1 on the line 
	double t = (line.formula(p1) / (line.a*line.a + line.b*line.b));		//(line.a,line.b) is the normal vector of the line
	return Point2d(p1.x - line.a*t , p1.y - line.b*t);						//go t times normal vector reach to the project line
}
Point2d Segment::findMirrorPoint(Line line) {								//find the reflected point of p1 at the other side of the line
	double t = (line.formula(p1) / (line.a*line.a + line.b*line.b));		//(line.a,line.b) is the normal vector of the line
	return Point2d(p1.x - line.a * 1.5 * t, p1.y - line.b * 1.5 * t);			//go 2*t times normal vector reach to the mirror point
}
Segment Segment::findReflectSeg(Segment seg){
	Point2d intersecP = this->findInterSection(seg);						//the intersection on the line
	Point2d mirrorP = this->findMirrorPoint(seg.line);						//the reflected point of this p1
	return Segment(mirrorP, intersecP);
}
double Segment::calInterSectionDist(Segment seg) {
	Point2d intersection = findInterSection(seg);
	double dx = intersection.x - p2.x;
	double dy = intersection.y - p2.y;
	return sqrt(dx*dx + dy*dy);
}
double Segment::calLength() {
	double dx = p1.x - p2.x;
	double dy = p1.y - p2.y;
	return sqrt(dx*dx + dy*dy);
}
double Segment::calPointDist(Point point) {
	return abs(line.formula(point) / sqrt(line.a*line.a + line.b*line.b));
}
double Segment::calAngle(Segment seg2) {
	Segment seg1(p1, p2);
	seg2 = seg2 - seg1.p1;
	seg1 = seg1 - seg1.p1;
	return (seg1.p2.x*seg2.p2.x + seg1.p2.y*seg2.p2.y) / seg1.calLength()/seg2.calLength();
}
Vector2d Segment::calUnitVec() {
	double len = calLength();
	return Vector2d ((p1 - p2) / len);
}
Segment Segment::operator+(Point2d move) {
	return Segment(p1 + move, p2 + move);
}
Segment Segment::operator-(Point2d move) {
	return Segment(p1 - move, p2 - move);
}
Segment Segment::operator+(Segment move) {
	return Segment(p1 + move.p1, p2 + move.p2);
}
Segment Segment::operator-(Segment move) {
	return Segment(p1 - move.p1, p2 - move.p2);
}
Segment Segment::operator*(double t) {
	return Segment(p1*t, p2*t);
}
Segment Segment::operator/(double t) {
	return Segment(p1/t, p2/t);
}
#pragma endregion
#pragma region shoot
Shoot::Shoot() {
	firstPoint = Point2d(-1, -1);
	trustValue = 0;
	trustPool = 0;
}
void Shoot::pushNewPoint(Point2d newPoint) {
	moveSeg.p2 = newPoint;
	moveSeg.line = Line(moveSeg.p1, moveSeg.p2);
}
void Shoot::updateFirstPoint(Point2d newPoint) {
	firstPoint = newPoint;
	moveSeg.p1 = firstPoint;
}
void Shoot::lineUpdate() {
	moveSeg.line = Line(moveSeg.p1, moveSeg.p2);
}
double Shoot::calTrustValue() {
	double dist = (60-moveSeg.p2.y) / 70 * 4;
	double len = moveSeg.calLength() / 5;
	trustValue = 3 / ((float)predictSize+1) * (dist + len);
	//printf("trust= %#.5lf,   dist= %#.5lf,   len= %#.5lf\n", trustValue, dist, len);
	return trustValue;
}
bool Shoot::trustSystem() {
	trustPool += calTrustValue();
	if (trustPool >= TRUST_TRIGGER) {
		trustPool = 0; //-= TRUST_TRIGGET;
		return true;
	}
	return false;
}
Segment Shoot::calAvrPredictLine() {
	return Segment(moveSeg + frameSeg*moveSeg.calLength() / sqrt(frameSeg.x*frameSeg.x+frameSeg.y*frameSeg.y));
}
#pragma endregion
#pragma region Coordinate
Coordinate::Coordinate(double newD1, double newD2, double newD3, double newX_axisDist, double newY_axisDist, double newBallRadius, double newHitterRadius) {
	d1 = newD1; d2 = newD2; d3 = newD3;			//d1 = 60, d2 = 55, d3 = 60
	X_axisDist = newX_axisDist; Y_axisDist = newY_axisDist;			//X_axisDist = 2.5, Y_axisDist = 8.5
	ballRadius = newBallRadius; hitterRadius = newHitterRadius;		//ballRadius = 3.5, hitterRadius = 4
	leftLocal = Segment(Point2d(0, 0), Point2d(0, d1));
	rightLocal = Segment(Point2d(d2, 0), Point2d(d2, d1));
	buttomLocal = Segment(Point2d(0, 0), Point2d(d2, 0));
	leftWall = leftLocal + Point2d(ballRadius - X_axisDist, 0);
	rightWall = rightLocal - Point2d(ballRadius - X_axisDist, 0);
	buttomWall = buttomLocal + Point2d(0, -1 * Y_axisDist + hitterRadius + ballRadius);
	initDone = false;
}
void Coordinate::init() {
	if (!initDone) {
		for (int i = 0; i < 4; i++) {
			if (locationPoint[i] == Point2d(-1, -1))
				return;
			standardLocationPoint[i] = locationPoint[i];
		}
		double deltaX = (locationPoint[1].y - locationPoint[0].y)*d2 / (locationPoint[1].y - locationPoint[3].y);//fix the error of x axis at index 0,2
		double deltaY = (locationPoint[3].x - locationPoint[1].x)*d1 / (locationPoint[1].x - locationPoint[0].x);
		leftLocal = Segment(Point2d(0, 0), Point2d(deltaX, d1));
		rightLocal = Segment(Point2d(d2, 0), Point2d(d2 + deltaX, d1));
		buttomLocal = Segment(Point2d((ballRadius - X_axisDist), 0), Point2d(d2 - (ballRadius - X_axisDist), -deltaY));
		//determine the collision wall by location point
		leftWall = leftLocal + Point2d(ballRadius - X_axisDist, 0);
		rightWall = rightLocal - Point2d(ballRadius - X_axisDist, 0);
		//buttomWall = buttomLocal + Point2d(0, -1 * Y_axisDist + hitterRadius + ballRadius);
		buttomWall = buttomLocal + Point2d(0, -1 * Y_axisDist);
		leftWall.p1 = buttomWall.p1;
		rightWall.p1 = buttomWall.p2;
		initDone = true;
		cout << "corners init done\n";
	}
}
Point2d Coordinate::getAvrOffset() {
	Point2d avrOffset = Point(0, 0);
	int count = 0;
	for (int i = 0; i < 4; i++) {
		if (locationPoint[i] != Point2d(-1, -1)) {
			avrOffset += locationPoint[i] - standardLocationPoint[i];
			count++;
		}
	}
	if (count != 0)
		return avrOffset /= count;
	else
		return Point(-1, -1);
}
bool Coordinate::setLocalPixelPoint() {
	for (int index = 0; index < 4; index++) {
		if (locationPoint[index] == Point2d(-1, -1)) {
			//cout << "couldn't find corner number " << index << endl;
			Point2d offset = getAvrOffset();
			if (offset != Point2d(-1, -1))
				locationPoint[index] += standardLocationPoint[index] + offset;
			else
				return false;
		}
	}
	return true;
}
Point2d Coordinate::transToNumeral(Point2d input) {
	if (initDone&&setLocalPixelPoint()) {
		return(Point2d(d2 * (input.y - locationPoint[1].y) / (locationPoint[3].y - locationPoint[1].y)
			, d1 * (input.x - locationPoint[1].x) / (locationPoint[0].x - locationPoint[1].x)));
	}
	else
		return Point2d(-1, -1);
}
Point2d Coordinate::transToPixel(Point2d input) {							//only used after setLocalPixelOffset (first trandToPixel)
	if (initDone) {
		return(Point2d(input.y*(locationPoint[0].x - locationPoint[1].x) / d1 + locationPoint[1].x
			, input.x*(locationPoint[3].y - locationPoint[1].y) / d2 + locationPoint[1].y));
	}
	else
		return Point2d(-1, -1);
}
void Coordinate::printOutWalls(Mat mat) {
	line(mat, transToPixel(leftWall.p1), transToPixel(leftWall.p2), Scalar(128, 128, 0), 3);
	line(mat, transToPixel(rightWall.p1), transToPixel(rightWall.p2), Scalar(128, 128, 0), 3);
	line(mat, transToPixel(buttomWall.p1), transToPixel(buttomWall.p2), Scalar(128, 128, 0), 3);
}
bool Coordinate::checkDir(Segment seg) {
	if (seg.p1 == Point2d(-1, -1) || seg.p2 == Point2d(-1, -1))
		return false;
	else if (seg.p1.y - seg.p2.y > 0)
		return true;
	else return false;
}
int Coordinate::getCollideWay(Segment seg, int lastWall = 0) {
	if (leftWall.collide(seg) && lastWall != 1) return 1;
	if (buttomWall.collide(seg) && lastWall != 2) return 2;
	if (rightWall.collide(seg) && lastWall != 3) return 3;
	return 0;
}//if lastWall == 0, detect collide wall by distance
int Coordinate::collidePredict(Segment seg, vector<Point2d>& points) {
	int hitWall = getCollideWay(seg);
	shoot.predictSize = 0;
	while (hitWall != 2) {
		if (hitWall == 1) {
			points.push_back(leftWall.findInterSection(seg));
			seg = seg.findReflectSeg(leftWall);
		}
		else if (hitWall == 3){
			points.push_back(rightWall.findInterSection(seg));
			seg = seg.findReflectSeg(rightWall);
		}
		shoot.predictSize++;
		hitWall = getCollideWay(seg,hitWall);
		if (shoot.predictSize >10 ) return shoot.predictSize;
	}
	if (hitWall == 2) {
		points.push_back(buttomWall.findInterSection(seg));
		shoot.predictSize++;
	}
	return shoot.predictSize;
}
void Coordinate::show(Mat mat,Segment seg,Scalar scalar) {
	line(mat, transToPixel(seg.p1), transToPixel(seg.p2), scalar, 4);
}
bool Coordinate::hittingWall() {
	static double lastDist = -1;
	double nowDist;
	static bool lastAppro = false;
	bool nowAppro;

	int hitWall = getCollideWay(shoot.moveSeg, 0);
	if (hitWall == 1)
		nowDist = leftWall.calInterSectionDist(shoot.moveSeg);
	else if (hitWall == 3)
		nowDist = rightWall.calInterSectionDist(shoot.moveSeg);
	else {
		lastAppro = false;
		lastDist = 30;
		return false;
	}

	nowAppro = (nowDist - lastDist > 0) ? false : true;
	if (lastAppro && !nowAppro) {
		lastAppro = nowAppro;
		lastDist = nowDist;
		return true;
	}
	lastAppro = nowAppro;
	lastDist = nowDist;
	return false;
}
#pragma endregion

#include <iostream>
#include <opencv2/opencv.hpp>
#include <queue>
#include <utility>
#include <limits.h>

using namespace std;
using namespace cv;

//Display the image with window name message
void display_image(Mat &image , String message) {
	namedWindow(message,CV_WINDOW_FREERATIO);
	imshow(message,image);
	waitKey(0);
}

int main() {
	
	//Read "path2.jpg" from the current directory and store it in a Mat object
	Mat image = imread("a-star-image.jpg",1);
	
	//Display the initial image (before the path is found out)
	display_image(image,"Preview");
	
	//rows and cols store the width and height of the image respectively
	int rows , cols;
	
	rows = image.rows;
	cols = image.cols;
	
	//G[i][j] denotes the category to which the pixel (i,j) belong to
	int G[rows][cols];
	
	//Parent[i][j] denotes the parent of the pixel (i,j)
	pair<int,int> Parent[rows][cols];
	
	//Closed_List[i][j] denotes whether the pixel (i,j) belongs to the closed list(should not be looked at again)
	//and whose final distance is calculated
	int Closed_List[rows][cols];
	
	struct dist {
		//g is the movement cost to move from starting point to that point
		int g;
		//h is the diagonal shortcut distance (estimated movement cost to move from that point to the destination)
		//h is just a guess and ignores all the obstacles and other conditions
		int h;
		//f is the addition of g and h
		int f;
	};
	
	//Distance[i][j] will store the 3 different distance of the pixel(i,j)
	dist Distance[rows][cols];
	
	//Initialisation
	for(int i=0 ; i<rows ; i++) {
		for(int j=0 ; j<cols ; j++) {
			G[i][j] = 1; //white pixel
			Distance[i][j].f = INT_MAX; //initialise each point to be at max distance from the start point
			Closed_List[i][j] = 0; //no pixel is on closed list yet
		}
	}
	
	//Temporary variables to store the bgr properties of a pixel
	int bvalue,gvalue,rvalue;
	
	//Categorise the image into different regions
	for(int i=0 ; i<rows ; i++) {
		for(int j=0 ; j<cols ; j++) {
			bvalue = image.at<Vec3b>(i,j)[0];
			gvalue = image.at<Vec3b>(i,j)[1];
			rvalue = image.at<Vec3b>(i,j)[2];
			if((bvalue<50)&&(gvalue>200)&&(rvalue<50)) {
				G[i][j] = 2; //green pixel
			}
			else if((bvalue<50)&&(gvalue<50)&&(rvalue>200)) {
				G[i][j] = 3; //red pixel
			}
			else if((bvalue<50)&&(gvalue<50)&&(rvalue<50)) {
				G[i][j] = 0; //black pixel
			}
		}
	}
	
	int start_count = 0;	//Stores the number of starting points
	int start_x_sum = 0;	//Stores the sum of x-coordinates of all the starting points
	int start_y_sum = 0;	//Stores the sum of y-coordinates of all the starting points
	int destination_count = 0;	//Stores the number of destination points
	int destination_x_sum = 0;	//Stores the sum of x-coordinates of all the destination points
	int destination_y_sum = 0;	//Stores the sum of y-coordinates of all the destination points
	
	//start and destination denotes the starting and destination points respectively
	pair<int,int> start,destination;
	
	//find the start and destination points using the idea of centre of mass where each point is of unit mass
	for(int i=0 ; i<rows ; i++) {
		for(int j=0 ; j<cols ; j++) {
			if(G[i][j]==2) {	//start(green) pixel
				start_x_sum = start_x_sum + i;
				start_y_sum = start_y_sum + j;
				start_count++;
			}
			else if(G[i][j]==3) {	//destination(red) pixel
				destination_x_sum = destination_x_sum + i;
				destination_y_sum = destination_y_sum + j;
				destination_count++;
			}
		}
	}
	
	int start_x = start_x_sum/start_count;				//x-coordinate of starting point
	int start_y = start_y_sum/start_count;				//y-coordinate of starting point
	int destination_x = destination_x_sum/destination_count;	//x-coordinate of destination point
	int destination_y = destination_y_sum/destination_count;	//y-coordinate of destination point
	
	start = make_pair(start_x,start_y);
	destination = make_pair(destination_x,destination_y);
	
	
	//Temporary variables to store the distance in X and Y directions
	int xDist , yDist;
	
	//Calculate h distances by diagonal shortcut method
	for(int i=0 ; i<rows ; i++) {
		for(int j=0 ; j<cols ; j++) {
			//diagonal shortcut distance
			xDist = abs(i-start.first);
			yDist = abs(j-start.second);
			if(xDist>yDist) {
				Distance[i][j].h = 14*yDist + 10*(xDist-yDist);
			}
			else {
				Distance[i][j].h = 14*xDist + 10*(yDist-xDist);
			}
		}
	}
	
	//To store the value of distances in sorted order
	priority_queue<pair<int,pair<int,int> > , vector<pair<int,pair<int,int> > > , greater<pair<int,pair<int,int> > > > Q;
	
	//Temporary pair to extract the topmost pair of the queue
	pair<int,pair<int,int> > top;
	
	//Temporary variables whose names describe the value stored by them
	int x , y , xnext , ynext , dist_next_g , dist_next_f , d ;
	
	//(N,M,w)==>(i,j,weight) to traverse the 8 adjacent vertices of a point
	int N[] = {-1,0,1,-1,1,-1,0,1};
	int M[] = {-1,-1,-1,0,0,1,1,1};
	int W[] = {14,10,14,10,10,14,10,14};
	
	//f and g distance of start point is zero
	Distance[start.first][start.second].g = 0;
	Distance[start.first][start.second].f = 0;
	
	//Parent of starting point is starting point itself
	Parent[start.first][start.second] = make_pair(start.first,start.second);
	
	//Initiate exploration from starting point
	Q.push(make_pair(Distance[start.first][start.second].f , make_pair(start.first,start.second)));
	
	//Carry-on exploration
	while((!Q.empty()) && (!Closed_List[destination.first][destination.second]) ) {
		top = Q.top();
		d = top.first;
		x = top.second.first;
		y = top.second.second;
		Q.pop();
		if(!Closed_List[x][y]) {
			Closed_List[x][y] = 1;
			for(int k=0 ; k<8 ; k++) {
				xnext = x + N[k];
				ynext = y + M[k];
				if((xnext>=0)&&(xnext<rows)&&(ynext>=0)&&(ynext<cols)) {
					if((G[xnext][ynext])&&(!Closed_List[xnext][ynext])) {
						dist_next_g = W[k]+Distance[x][y].g;
						dist_next_f = dist_next_g + Distance[xnext][ynext].h;
						if(dist_next_f<Distance[xnext][ynext].f) {
							Distance[xnext][ynext].g = dist_next_g;
							Distance[xnext][ynext].f = dist_next_f;
							Parent[xnext][ynext] = make_pair(x,y);
							Q.push(make_pair(dist_next_f , make_pair(xnext,ynext)));
						}
					}
				}
			}
			
		}
	}
	
	//Initialise a temporary pair to traverse through the path found
	pair<int,int> temp = destination;
	
	//Traverse through the path and mark it on the image
	while(temp!=Parent[temp.first][temp.second]) {
		x = temp.first;
		y = temp.second;
		image.at<Vec3b>(x,y)[0] = 0;
		image.at<Vec3b>(x,y)[1] = 250;
		image.at<Vec3b>(x,y)[2] = 0;
		temp = Parent[temp.first][temp.second];
	}
	
	//Display the final image after highlighting the path found
	display_image(image,"Output");
	
	return 0;
	
}

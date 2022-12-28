 #include <iostream>
 #include <chrono>
 #include <pthread.h>
 #include <unistd.h>
 
 #include <opencv2/dnn_superres.hpp>
 
 #include <opencv2/imgproc.hpp>
 #include <opencv2/highgui.hpp>
 #include <opencv2/quality.hpp>
 
 using namespace std;
 using namespace cv;
 using namespace dnn_superres;
 using namespace std::chrono;

 double getPSNR(const Mat& I1, const Mat& I2)
{
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2
    Scalar s = sum(s1);        // sum elements per channel
    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels
    if( sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double mse  = sse / (double)(I1.channels() * I1.total());
        double psnr = 10.0 * log10((255 * 255) / mse);
        return psnr;
    }
}

bool step=false;


vector<Mat> frames;
vector<Rect> mCells;
DnnSuperResImpl sr,sr2,sr3,sr4,sr5,sr6,sr7,sr8,sr9,sr10,sr11,sr12;
int m=0;
bool video=true;
int frame_count=0;

 int main(int argc, char *argv[])
 {
 
    
    string algorithm = "espcn";
    int scale = 2;
    string path = "ESPCN_x2.pb";

    //VideoCapture input_video(input_path);
    VideoCapture input_video("/dev/video2");
    /* ex = static_cast<int>(input_video.get(CAP_PROP_FOURCC));
    Size S = Size((int) input_video.get(CAP_PROP_FRAME_WIDTH) * scale,
                (int) input_video.get(CAP_PROP_FRAME_HEIGHT) * scale);

    VideoWriter output_video;
    output_video.open(output_path, ex, input_video.get(CAP_PROP_FPS), S, true);*/

    if (!input_video.isOpened())
    {
        std::cerr << "Could not open the video." << std::endl;
        return -1;
    }
    
    
    sr.readModel(path);
    sr.setModel(algorithm, scale);
    
     
      
	double psnr_x4=0,total_psnr=0,total_ssim=0;
	int i=0;
    bool start=true;
     for(;;)
     {     
        auto start = high_resolution_clock::now();
        Mat frame, output_frame,resized;
        input_video >> frame;
        if ( frame.empty() )
             break;
        frame_count++;

        cout << "Width : " << frame.size().width << endl;
        cout << "Height: " << frame.size().height << endl;
        cout << "Super Resolution method: "<<algorithm<<endl; 
        cout << "Super Resolution scale: "<<scale<<endl;
        if(frame_count==1)
            cout<<"!!! I Frame !!!"<<endl;
        else
        {
            cout<<"!!! P Frame !!!"<<endl;
        }  
        //resize(frame, resized, cv::Size(450, 150));
        
        sr.upsample(frame, output_frame);
		Mat resized_x4;
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout << "duration="<<duration.count()<<"ms" << endl; 
        cout<<endl;
      
     }
	
     input_video.release();
     
 
     return 0;
 }



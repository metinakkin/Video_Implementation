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
typedef struct pThreadWorker {
    pthread_t                           thread;                             //!< Thread identifier
    string                              setting;
    string                              value;                            

}pThreadWorker;
pThreadWorker                               pthreads[12];
void *Super_Resolution                    (void *arg);

vector<Mat> frames;
vector<Rect> mCells;
DnnSuperResImpl sr,sr2,sr3,sr4,sr5,sr6,sr7,sr8,sr9,sr10,sr11,sr12;
int m=0;
bool video=true;
bool start_threads=true;
int frame_count=0;
int thred_numbers=0;
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

    sr2.readModel(path);
    sr2.setModel(algorithm, scale);

    sr3.readModel(path);
    sr3.setModel(algorithm, scale);

    sr4.readModel(path);
    sr4.setModel(algorithm, scale);

    sr5.readModel(path);
    sr5.setModel(algorithm, scale);

    sr6.readModel(path);
    sr6.setModel(algorithm, scale);

    sr7.readModel(path);
    sr7.setModel(algorithm, scale);

    sr8.readModel(path);
    sr8.setModel(algorithm, scale);

    sr9.readModel(path);
    sr9.setModel(algorithm, scale);

    sr10.readModel(path);
    sr10.setModel(algorithm, scale);

    sr11.readModel(path);
    sr11.setModel(algorithm, scale);

    sr12.readModel(path);
    sr12.setModel(algorithm, scale);

     
      
	double psnr_x4=0,total_psnr=0,total_ssim=0;
	int i=0;
    
     for(;;)
     {     
        
       
             
         Mat frame, output_frame,resized;
         input_video >> frame;
         if ( frame.empty() )
             break;

        frame_count++;
         step=false;         
        
        int width = frame.size().width;
        int height = frame.size().height;
        int GRID_SIZE = 160;
        cout<<"Width : "<<width<<endl;
        cout<<"Height: "<<height<<endl;
        cout << "Super Resolution method: "<<algorithm<<endl; 
        cout << "Super Resolution scale: "<<scale<<endl;
        if(frame_count==1)
            cout<<"!!! I Frame !!!"<<endl;
        else
        {
            cout<<"!!! P Frame !!!"<<endl;
        }    
      
            for (int y = 0; y <= height - GRID_SIZE; y += GRID_SIZE) {
                for (int x = 0; x <= width - GRID_SIZE; x += GRID_SIZE) {
                    int k = x*y + x;
                    Rect grid_rect(x, y, GRID_SIZE, GRID_SIZE);
                    cout << grid_rect<< endl;
                    mCells.push_back(grid_rect);
                    rectangle(frame, grid_rect, Scalar(0, 255, 0), 1);
                    frames.push_back(frame);
                    //imshow("src", frame);
                    //imshow(format("grid%d",k), frame(grid_rect));
                    //waitKey(100);
                }
            }    
            
                if(start_threads==true)
            {
        
                
                for(int i = 0; i < 12; i++ ) { 
                int create_result = pthread_create(&pthreads[i].thread, NULL, Super_Resolution, (void *)i);
                if (create_result != 0)
                {   
                        cout<<"Error creating pose_data reader worker thread"<<endl;      
                }              

                }

                    start_threads=false;
            }
        
    
            while(!step);
            
            frames.clear();
        
         cout<<endl;
        
      
     }
	
     input_video.release();
     
 
     return 0;
 }


void *Super_Resolution(void *arg)
{
    //cout<<"test="<<(long)arg<<endl;
    if((long)arg==0)
    {
        auto start = high_resolution_clock::now();
        Mat  output_frame;
        //cout<<"tile="<<(long)arg+1<<endl;
        
        //imshow("src1", frames[(long)arg](mCells[(long)arg]));
        sr.upsample(frames[(long)arg](mCells[(long)arg]), output_frame);
       //imshow(format("grid1"), frames[(long)arg](mCells[(long)arg]));
       //imshow(format("grid2"), output_frame);
         auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout <<"step="<<(long)arg+1<<"      "<< "duration="<<duration.count()<<"ms" << endl;
        //waitKey(100);
        thred_numbers++;
        
    }
    if((long)arg==1)
    {
        auto start2 = high_resolution_clock::now();
        Mat  output_frame2;
        //cout<<"tile2="<<(long)arg+1<<endl;
        
        //imshow("src", frames[0]);
        sr2.upsample(frames[(long)arg](mCells[(long)arg]), output_frame2);
        //imshow(format("grid3"), frames[(long)arg](mCells[(long)arg]));
        //imshow(format("grid4"), output_frame2);
         auto stop2 = high_resolution_clock::now();
        auto duration2 = duration_cast<milliseconds>(stop2 - start2);
        cout <<"step="<<(long)arg+1<<"      "<< "duration="<<duration2.count()<<"ms" << endl;
        //waitKey(100);
        thred_numbers++;
       
    }
    if((long)arg==2)
    {
        auto start3 = high_resolution_clock::now();
        Mat  output_frame3;
        //cout<<"tile3="<<(long)arg+1<<endl;
        
        //imshow("src", frames[0]);
        sr3.upsample(frames[(long)arg](mCells[(long)arg]), output_frame3);
        //imshow(format("grid5"), frames[(long)arg](mCells[(long)arg]));
        //imshow(format("grid6"), output_frame3);
         auto stop3 = high_resolution_clock::now();
        auto duration3 = duration_cast<milliseconds>(stop3 - start3);
        cout <<"step="<<(long)arg+1<<"      "<< "duration="<<duration3.count()<<"ms" << endl;
        //waitKey(100);
        thred_numbers++;
      
    }

   if((long)arg==3)
    {
        auto start4 = high_resolution_clock::now();
        Mat  output_frame4;
        //cout<<"tile4="<<(long)arg+1<<endl;
        
        //imshow("src", frames[0]);
        sr4.upsample(frames[(long)arg](mCells[(long)arg]), output_frame4);
        //imshow(format("grid7"), frames[(long)arg](mCells[(long)arg]));
        //imshow(format("grid8"), output_frame4);
         auto stop4 = high_resolution_clock::now();
        auto duration4 = duration_cast<milliseconds>(stop4 - start4);
        cout <<"step="<<(long)arg+1<<"      "<< "duration="<<duration4.count()<<"ms" << endl;
        //waitKey(100);
        thred_numbers++;
       
    }

     if((long)arg==4)
    {
        auto start5 = high_resolution_clock::now();
        Mat  output_frame5;
        //cout<<"tile5="<<(long)arg+1<<endl;
        
        //imshow("src", frames[0]);
        sr5.upsample(frames[(long)arg](mCells[(long)arg]), output_frame5);
        //imshow(format("grid9"), frames[(long)arg](mCells[(long)arg]));
        //imshow(format("grid10"), output_frame5);
         auto stop5 = high_resolution_clock::now();
        auto duration5 = duration_cast<milliseconds>(stop5 - start5);
        cout <<"step="<<(long)arg+1<<"      "<< "duration="<<duration5.count()<<"ms" << endl;
        //waitKey(100);
        thred_numbers++;
        
    }

    if((long)arg==5)
    {
        auto start6 = high_resolution_clock::now();
        Mat  output_frame6;
        //cout<<"tile6="<<(long)arg+1<<endl;
        
        //imshow("src", frames[0]);
        sr6.upsample(frames[(long)arg](mCells[(long)arg]), output_frame6);
        //imshow(format("grid11"), frames[(long)arg](mCells[(long)arg]));
        //imshow(format("grid12"), output_frame6);
         auto stop6 = high_resolution_clock::now();
        auto duration6 = duration_cast<milliseconds>(stop6 - start6);
        cout <<"step="<<(long)arg+1<<"      "<< "duration="<<duration6.count()<<"ms" << endl;
        //waitKey(100);
        thred_numbers++;
       
    }

    if((long)arg==6)
    {
        auto start7 = high_resolution_clock::now();
        Mat  output_frame7;
        //cout<<"tile7="<<(long)arg+1<<endl;
        
        //imshow("src", frames[0]);
        sr7.upsample(frames[(long)arg](mCells[(long)arg]), output_frame7);
        //imshow(format("grid13"), frames[(long)arg](mCells[(long)arg]));
        //imshow(format("grid14"), output_frame7);
         auto stop7 = high_resolution_clock::now();
        auto duration7 = duration_cast<milliseconds>(stop7 - start7);
        cout <<"step="<<(long)arg+1<<"      "<< "duration="<<duration7.count()<<"ms" << endl;
        //waitKey(100);
        thred_numbers++;
        
    }
     if((long)arg==7)
    {
        auto start8 = high_resolution_clock::now();
        Mat  output_frame8;
        //cout<<"tile8="<<(long)arg+1<<endl;
        
        //imshow("src", frames[0]);
        sr8.upsample(frames[(long)arg](mCells[(long)arg]), output_frame8);
        //imshow(format("grid15"), frames[(long)arg](mCells[(long)arg]));
        //imshow(format("grid16"), output_frame8);
         auto stop8 = high_resolution_clock::now();
        auto duration8 = duration_cast<milliseconds>(stop8 - start8);
        cout <<"step="<<(long)arg+1<<"      "<< "duration="<<duration8.count()<<"ms" << endl;
        //waitKey(100);
        thred_numbers++;
        
    }

     if((long)arg==8)
    {
        auto start9 = high_resolution_clock::now();
        Mat  output_frame9;
        //cout<<"tile9="<<(long)arg+1<<endl;
        
        //imshow("src", frames[0]);
        sr9.upsample(frames[(long)arg](mCells[(long)arg]), output_frame9);
        //imshow(format("grid17"), frames[(long)arg](mCells[(long)arg]));
        //imshow(format("grid18"), output_frame9);
         auto stop9 = high_resolution_clock::now();
        auto duration9 = duration_cast<milliseconds>(stop9 - start9);
        cout <<"step="<<(long)arg+1<<"      "<< "duration="<<duration9.count()<<"ms" << endl;
        //waitKey(100);
        thred_numbers++;
        
    }

     if((long)arg==9)
    {
        auto start10 = high_resolution_clock::now();
        Mat  output_frame10;
        //cout<<"tile10="<<(long)arg+1<<endl;
        
        //imshow("src", frames[0]);
        sr10.upsample(frames[(long)arg](mCells[(long)arg]), output_frame10);
        //imshow(format("grid19"), frames[(long)arg](mCells[(long)arg]));
        //imshow(format("grid20"), output_frame10);
         auto stop10 = high_resolution_clock::now();
        auto duration10 = duration_cast<milliseconds>(stop10 - start10);
        cout <<"step="<<(long)arg+1<<"      "<< "duration="<<duration10.count()<<"ms" << endl;
        //waitKey(100);
        thred_numbers++;
        
    }

      if((long)arg==10)
    {
        auto start11 = high_resolution_clock::now();
        Mat  output_frame11;
        //cout<<"tile11="<<(long)arg+1<<endl;
        
        //imshow("src", frames[0]);
        sr11.upsample(frames[(long)arg](mCells[(long)arg]), output_frame11);
        //imshow(format("grid21"), frames[(long)arg](mCells[(long)arg]));
        //imshow(format("grid22"), output_frame11);
         auto stop11 = high_resolution_clock::now();
        auto duration11 = duration_cast<milliseconds>(stop11 - start11);
        cout <<"step="<<(long)arg+1<<"      "<< "duration="<<duration11.count()<<"ms" << endl;
        //waitKey(100);
        thred_numbers++;
        
    }

     if((long)arg==11)
    {
        auto start12 = high_resolution_clock::now();
        Mat  output_frame12;
        //cout<<"tile12="<<(long)arg+1<<endl;
        
        //imshow("src", frames[0]);
        sr12.upsample(frames[(long)arg](mCells[(long)arg]), output_frame12);
        //imshow(format("grid23"), frames[(long)arg](mCells[(long)arg]));
        //imshow(format("grid24"), output_frame12);
         auto stop12 = high_resolution_clock::now();
        auto duration12 = duration_cast<milliseconds>(stop12 - start12);
        cout <<"step="<<(long)arg+1<<"      "<< "duration="<<duration12.count()<<"ms" << endl;
        //waitKey(100);
        thred_numbers++;
        
    }
    
    if(thred_numbers==12)
    {
        start_threads=true;
        step=true;
         for(int i = 0; i < 12; i++ ) {
        pthread_cancel(pthreads[i].thread); 
        if (pthreads[i].thread != 0)
            pthread_join(pthreads[i].thread, NULL);   
     }
       
    }
    

    
    
}


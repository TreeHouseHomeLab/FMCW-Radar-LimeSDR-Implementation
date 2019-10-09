#include "main.h"

using namespace std;
ofstream Out1("Band1Rx.txt");

int main(){

    Setup();
    Stream();

}

void writebuff(){
    std::copy(bufferrx[0],bufferrx[0]+2*(chirp_size),std::back_inserter(Output));
    }

void writeout(){ //RxBuffer being written to file

    for (auto it = Output.begin();it!=Output.end();it++){
        Out1<<*it<<",";
                }

}

void ChirpGen(){ //generate frequency sweep

    float d=0;
    float t=0;
    float interval =  0.1*pow(10,-3);
    float chirp_size = 2000;
    ofstream Out3("chirp.txt");
    for(int i = 0;i<chirp_size;i++){
         d= i/chirp_size;
         t= interval*d;

         buffertx[2*i]=cos(2*PI*t*(+((dF)/2)-(0.5*dF*t/(interval))));
         buffertx[2*i+1]=-sin(2*PI*t*(+(dF/2)-(0.5*dF*t/(interval))));
         Out3 << buffertx[2*i]<< ","<<buffertx[2*i+1]<<",";

        }
    for(int j = chirp_size;j<chirp_size+deadsamp;j++){
             buffertx[2*j]=0;
             buffertx[2*j+1]=0;}

    Out3.close();

}

int error(){

  cout << "ERROR: " << LMS_GetLastErrorMessage() << endl;
  if (device !=NULL)
    LMS_Close(device);
  exit(-1);
}


void Setup(){


    int n;
    if((n = LMS_GetDeviceList(NULL))<0)
        error();
    cout<<"Device found: "<<n<<endl;


    if (n > 0){
        //open first device
        if (LMS_Open(&device,NULL,NULL)!=0)
            error();
        //initial config
        if (LMS_Init(device)!=0)
            error();

        //Get number of channels
        if ((n = LMS_GetNumChannels(device, LMS_CH_RX)) < 0)
            error();

        cout << "Number of RX channels: " << n << endl;

        if ((n = LMS_GetNumChannels(device, LMS_CH_TX)) < 0)
            error();
        cout << "Number of TX channels: " << n << endl;

         //Enable RX channel
        if (LMS_EnableChannel(device, LMS_CH_RX, 0, true) != 0)
            error();

        //Enable TX channels
        if (LMS_EnableChannel(device, LMS_CH_TX, 1, true) != 0)
            error();


        //Set RX center frequency
        if (LMS_SetLOFrequency(device, LMS_CH_RX, 0, 2.4e9) != 0)
            error();

        //Set TX center frequency
        if (LMS_SetLOFrequency(device, LMS_CH_TX, 1, 2.4e9) != 0)
            error();

        if (LMS_SetSampleRate(device,Fs,0)!=0)
            error();

        if (LMS_SetGaindB(device, LMS_CH_RX, 0, 20) != 0)
            error();

    //Set TX gain
        if (LMS_SetGaindB(device, LMS_CH_TX, 1, 70) != 0)
            error();
    }
}

int Stream(){

    //Initialize streams
    for (int i = 0; i < chCount; ++i){

            rx_streams[i].channel = i; //channel number
            rx_streams[i].fifoSize = (chirp_size +deadsamp)*2; //fifo size in samples
            rx_streams[i].throughputVsLatency = 1; //some middle ground
            rx_streams[i].isTx = false; //RX channel
            rx_streams[i].dataFmt = lms_stream_t::LMS_FMT_F32;
            if (LMS_SetupStream(device, &rx_streams[i]) != 0)
                error();
            if (i<1){
            tx_streams.channel = 1; //channel number
            tx_streams.fifoSize = (chirp_size +deadsamp)*2; //fifo size in samples
            tx_streams.throughputVsLatency = 1; //some middle ground
            tx_streams.isTx = true; //TX channel
            tx_streams.dataFmt = lms_stream_t::LMS_FMT_F32;
            if (LMS_SetupStream(device, &tx_streams) != 0)
                error();}
        }


     for (int i = 0; i < chCount; ++i)
        {
        //buffer to hold complex values (2*samples))
        bufferrx[i] = new float[(chirp_size + deadsamp)* 2];
        }

    buffertx = new float[(chirp_size +deadsamp) * 2];
    Output.reserve(10000*2*chirp_size);

    ChirpGen();


    //Start streaming
    for (int i = 0; i < chCount; ++i){

        LMS_StartStream(&rx_streams[i]);
        if (i<1){
            LMS_StartStream(&tx_streams);}
        }



    rx_metadata.flushPartialPacket = false; //currently has no effect in RX
    rx_metadata.waitForTimestamp = false;
    tx_metadata.flushPartialPacket = false; //do not force sending of incomplete packet
    tx_metadata.waitForTimestamp = true;


    int k = 0;
    while (k<10000) { //run for 30 seconds
        LMS_RecvStream(&rx_streams[0], bufferrx[0],(chirp_size +deadsamp),&rx_metadata,1000);
        tx_metadata.timestamp = rx_metadata.timestamp+1024*150;
        if( k<1000)
            writebuff();
        LMS_SendStream(&tx_streams, buffertx, (chirp_size +deadsamp),&tx_metadata,1000);
        k++;
        }

    cout<<"Writing Out"<<endl;
    writeout();
    //Stop streaming
    for (int i = 0; i < chCount; ++i)
        {
        LMS_StopStream(&rx_streams[i]); //stream is stopped but can be started again with LMS_StartStream()
        if(i<1){
            LMS_StopStream(&tx_streams);}
        }
    for (int i = 0; i < chCount; ++i)
        {
        LMS_DestroyStream(device, &rx_streams[i]); //stream is deallocated and can no longer be used
         if(i<1){
            LMS_DestroyStream(device, &tx_streams);}

        delete[] bufferrx[i];
        }
    delete[] buffertx;
    //Close device
    LMS_Close(device);
    Out1.close();
    Out2.close();
    return 0;

}

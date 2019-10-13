#include <iostream>
#include "/media/ciaran/DATA/FinalYear/Thesis/sdr_src/LimeSuite/src/lime/LimeSuite.h"
#include <chrono>
#include <math.h>
#define PI 3.14159265358979323846
#include <fstream> // To use ifstream
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <thread>
lms_device_t* device;


const float interval = 0.1*pow(10,-3);
const float Fe = 20e6;
const float Fst =  0;
const float Fs = 20e6;
const int chirp_size = 2000;
const float dF = Fe-Fst;
int deadsamp = 200;
const int chCount = 1;
float * buffertx;
float * bufferlo;
float * bufferrx[chCount];
bool oscil = true;

std::vector<float> Output;
std::vector<float> Output2;


lms_stream_meta_t rx_metadata;
lms_stream_meta_t tx_metadata;
lms_stream_t rx_streams[chCount];
lms_stream_t tx_streams;
lms_stream_t lo_streams;

void writebuff();
void ChirpGen();
void Setup();
int Stream();
int error();
void writeout();
void osc();




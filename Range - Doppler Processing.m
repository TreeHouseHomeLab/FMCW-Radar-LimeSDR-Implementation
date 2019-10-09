
Tx = importdata("chirp.txt",",");
  Rx1 = importdata("Band1Rx.txt",",");

 TxI= Tx(1:2:end-1);
 TxQ= Tx(2:2:end);
 Tx_sig = TxI+1j*TxQ;

figure();
plot(t,Tx_sig);

%%
Fs = 20e6;            % Sampling frequency
BW = 20e6;
Tc = 0.1*10^-3;         % length of chirp
T = 1/Fs;                % Sampling period
L = Fs*Tc;               % Length of signal
sampgap = Tc+200/Fs;
%%
Rx1I= Rx1(L*146+1 : 2 : end-1);
Rx1Q= Rx1(L*146+2 : 2 : end);
Rx_sig = Rx1I+1j*Rx1Q;
%%
NoChirps = length(Rx_sig)/L;
S = BW/Tc;

%%Time specifications:
t = (0:T:Tc-T).';
t2 = (1:1:length(Rx_sig)).';
N = size(t,1);
%%Frequency specifications:
dF = Fs/N;   % hertz
f = -Fs/2:dF:Fs/2-dF;
R = 900;
%%


Chirps = reshape(Rx_sig,L,[]);


%
figure();
x = Chirps(:,150);
plot(t,x);



%%
  figure();
  plot(t2,Rx_sig);

ChirpFFT=zeros(L,NoChirps);
DopplerFFT=zeros(L,NoChirps);
Shortdop = zeros(L,R);
Shortdoptp = zeros(R,L);
DopplerFFTtp = zeros(NoChirps,L);


RangeWindow = transpose(blackman(L));
DopplerWindow = blackman(NoChirps);
DopplerWindow2 = blackman(R);



%%
for i=1:NoChirps
    X = fftshift(fft(Chirps(:,i).*RangeWindow.'));
    ChirpFFT(:,i) = X;
    if i <R+1
        Shortdop(:,i) = X;
    end
end

for i=1:L
    X = fftshift(fft(ChirpFFT(i,:).*DopplerWindow.'));
    X1 = fftshift(fft(Shortdop(i,:).*DopplerWindow2.'));
    DopplerFFT(i,:) = X;
    Shortdop(i,:) = X1;

end


Shortdoptp = transpose(Shortdop);
DopplerFFTtp = transpose(DopplerFFT);

figure();
v = 3e8/(2*2.4e9*sampgap);

r = 3e8*L/(2*BW);
r1= r/L;
 x = [-r/2-r1 r/2];
 y = [-v/2 v/2];
imagesc(x,y,20*log10(abs(Shortdoptp)));
imagesc(x,y,20*log10(abs(DopplerFFTtp)));
ylabel('Velocity');
xlabel('Distance');
colorbar
colormap jet




%%
X = fftshift(fft(Chirps(:,150)));
figure();
plot(f,abs(X)/N);
xlabel('Frequency (in hertz)');
title('Magnitude Response');
X = sprintf('Distance: %dm',Distances(1,1));
disp(X);

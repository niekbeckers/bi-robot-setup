%% make_forcingfunction

clear;close all;clc;

% signal properties
fs = 1000;  % sampling frequency
dt = 1/fs; % time step
fn = fs/2; % nyquist frequency
T = 12; % duration
df = 2*pi/T; % frequency resolution
N = T*fs; % number of samples
Nf = 35; % number of frequencies
freqmin = 0.5;
freqmax = 300;
t = (0:dt:T-dt)';

% frequency vector: equally spaced frequencies
freq = 2*pi*logspace(log10(freqmin), log10(freqmax), Nf).';
freq2 = 2*pi*logspace(log10(90), log10(130), 6);
freq = union(freq,freq2);
% Prevent leakage: make sure that each frequency is a multiple of 1/T.
freq = floor(freq / df)*df;
freq_n = round(freq/df);

% amplitudes
amp = ones(size(freq));
% amp(freq>2*pi*100) = 0.7;
% amp = amp*(5^2)/2;
amp = amp/2500;


% plot 
figure('color','w');
stem(freq,amp);
set(gca,'xscale','log');

% random frequency
phase = 2*pi*randn(size(freq));

% Complex amplitude vector
amp_c   = amp.*exp(phase*1i);

% Crested multisine
figure('color','w');
cx = msinclip(freq, amp_c);
w_input  = msinprep(freq, cx, N, 2*pi*fs); % input requested torque

% scale fofu_t
Pmax = 0.005; % 0.5Nm max amplitude (on arm)
scale = Pmax/max(abs(w_input));
% w_input = scale*w_input;

% % cx2 = cx*2; cx2(1) = cx2(1)/2;
cx2 = cx;
w2 = zeros(size(t));
for ii = 1:size(freq,1)
    w2 = w2 + amp(ii)*sin(freq(ii)*t+angle(cx2(ii)));
end
% w2 = Pmax*w2/max(abs(w2));


figure('color','w')
plot(t,w_input);
xlabel('time [s]'); ylabel('pos reference input [Nm]');
hold on
plot(t,w2,'--');

% % plot noise limit
% plot(t,0.0017*ones(size(t)),'k--');
% plot(t,-0.0017*ones(size(t)),'k--');

input_signal = [t w_input];
phase = angle(cx);
save('data_inputsignal2.mat','freq','amp','phase','freq_n');

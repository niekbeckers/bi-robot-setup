%% make_forcingfunction

clear;close all;clc;

% signal properties
fs = 1000;  % sampling frequency
dt = 1/fs; % time step
fn = fs/2; % nyquist frequency
Tm = 12; % duration
df = 2*pi/Tm; % frequency resolution
N = Tm*fs; % number of samples
Nf = 15; % number of frequencies
freqmin = 10;
freqmax = 80;
t = (0:dt:Tm-dt)';

% frequency vector: equally spaced frequencies
w = 2*pi*logspace(log10(freqmin), log10(freqmax), Nf).';
% freq2 = 2*pi*logspace(log10(90), log10(130), 6);
% freq = union(freq,freq2);
% Prevent leakage: make sure that each frequency is a multiple of 1/T.
w = floor(w / df)*df;
n = round(w/df);

% amplitudes
A = ones(size(w));
A(w>2*pi*50) = 0.1;
% amp = amp*(5^2)/2;
% amp = amp/1000;

p = sum(A.^2)/2;    % position signal variance       
k = sqrt(0.000025/p);  % scaling factor; disturbance half the power of the target signal
A = k*A;            % scale
s2 = sum(A.^2)/2


% plot 
figure('color','w');
stem(w,A);
set(gca,'xscale','log');

% random frequency
ph = 2*pi*randn(size(w));

% Complex amplitude vector
A_c   = A.*exp(ph*1i);

% Crested multisine
figure('color','w');
cx = msinclip(w, A_c,[],[],1000);
ph = angle(cx)+0.5*pi;


[p,v,a] = msin(t,A,w,ph);

figure('color','w')
plot(t,p,'.-');
xlabel('time [s]'); ylabel('pos reference input');
hold on

% save('data_inputsignal.mat','A','n','ph','Tm')


% w_input  = msinprep(freq, cx, N, 2*pi*fs); % input requested torque

% % scale fofu_t
% Pmax = 0.005; % 0.5Nm max amplitude (on arm)
% scale = Pmax/max(abs(w_input));
% % w_input = scale*w_input;
% 
% % % cx2 = cx*2; cx2(1) = cx2(1)/2;
% cx2 = cx;
% w2 = zeros(size(t));
% for ii = 1:size(freq,1)
%     w2 = w2 + amp(ii)*sin(freq(ii)*t+angle(cx2(ii)));
% end
% % w2 = Pmax*w2/max(abs(w2));
% 
% 
% figure('color','w')
% plot(t,w_input);
% xlabel('time [s]'); ylabel('pos reference input [Nm]');
% hold on
% plot(t,w2,'--');
% 
% % % plot noise limit
% % plot(t,0.0017*ones(size(t)),'k--');
% % plot(t,-0.0017*ones(size(t)),'k--');
% 
% input_signal = [t w_input];
% phase = angle(cx);
% % save('data_inputsignal5.mat','freq','amp','phase','freq_n');


function [p,v,a] = msin(t,A,w,ph)

p = zeros(size(t));
v = zeros(size(t));
a = zeros(size(t));

for ii = 1:length(w)
    p = p + A(ii)*sin(w(ii)*t+ph(ii));
    v = v + w(ii)*A(ii)*cos(w(ii)*t+ph(ii));
    a = a + -w(ii)^2*A(ii)*sin(w(ii)*t+ph(ii));
end

end

clear all; close all; clc;

load('data_sysid_x_nom_run1.mat');
load('data_inputsignal.mat');

T = 12;
fs = 1000;
dt = 1/fs;
fn = fs/2;

data = trialdata.trial;

t = data.time; t = t-t(1);
t = 0:dt:length(t)*dt-dt;

xref = data.x_ref_BROS1;
x1 = data.x_BROS1;
x2 = data.x_AbsEnc_BROS1;

[XREF,w] = fft_mod(xref(:,1),t);
[X1]      = fft_mod(x1(:,1),t);
[X2]      = fft_mod(x2(:,1),t);

H1 = X1./XREF;
H2 = X2./XREF;


figure('color','w');
% loglog(w/(2*pi),abs(H),'-','color', [0.8 0.8 0.8 0.5]); hold on;
loglog(w(freq_n)/(2*pi),abs(H1(freq_n)),'.-'); hold on;
loglog(w(freq_n)/(2*pi),abs(H2(freq_n)),'.-');


% figure
% plot(t,xref(:,1)); hold on
% plot(t,x(:,1));


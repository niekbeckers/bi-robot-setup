% clear all; close all; clc;

load('data_sysid_x_nom_run15.mat');
load('data_inputsignal_100Hz.mat');

T = 12;
fs = 1000;
dt = 1/fs;
fn = fs/2;

idx_trials = 7:12;%length(trialdata.trial);

figure('color','w');
for ii = 1:length(idx_trials)
    data = trialdata.trial(idx_trials(ii));

    t = data.time; t = t-t(1);
    t = 0:dt:length(t)*dt-dt;

    xref = data.x_ref_BROS1;
    x1 = data.x_BROS1;
    % x2 = data.x_AbsEnc_BROS1;

    [XREF,w] = fft_mod(xref(:,2),t);
    [X1]      = fft_mod(x1(:,2),t);
    % [X2]      = fft_mod(x2(:,1),t);

    H1 = X1./XREF;
    % H2 = X2./XREF;


    
    % loglog(w/(2*pi),abs(H),'-','color', [0.8 0.8 0.8 0.5]); hold on;
    loglog(w(n)/(2*pi),abs(H1(n)),'.-'); hold on;
% loglog(w(n)/(2*pi),abs(H2(n)),'.-');
end

loglog(xlim,10^-0.15*[1 1],'-','color',0.8*[1 1 1])

% figure
% plot(t,xref(:,1)); hold on
% plot(t,x(:,1));


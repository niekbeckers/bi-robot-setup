%% measurement of transmission ratio

clear; close all; clc;

%% FM1
%% q5 (motor 2)

% load measurement 1 data
data = [];
files = dir('data_transmission_meas1_fm1_q5_part*.mat');
for ii = 1:length(files)
    load(files(ii).name);
    name = files(ii).name;
    num = str2double(name(strfind(name,'part')+4:end-3));
    eval(['data_temp = data_' num2str(num) ';']);
    data = [data data_temp];
    clear data_*
end
% extract positions from encoders
abs_pos_m1_q5 = data(23,:)'; 
abs_pos_m1_q5 = abs_pos_m1_q5 - mean(abs_pos_m1_q5); % demean
inc_pos_m1_q5 = data(20,:)';
inc_pos_m1_q5 = inc_pos_m1_q5 - mean(inc_pos_m1_q5); % demean
t1 = data(1,:)'; t1 = t1 - t1(1);
% use regression to fit transmission ratio
[b_fit_m1_q5,~,~,~,stats_fit_m1_q5] = regress(abs_pos_m1_q5, [ones(size(inc_pos_m1_q5)) inc_pos_m1_q5]);
% calculate fit
inc_pos1_fit = inc_pos_m1_q5*b_fit_m1_q5(2) + b_fit_m1_q5(1);

% load measurement 2 data
data = [];
files = dir('data_transmission_meas2_fm1_q5_part*.mat');
for ii = 6:10
    load(['data_transmission_meas2_fm1_q5_part' num2str(ii) '.mat']);
    eval(['data_temp = data_' num2str(ii) ';']);
    data = [data data_temp];
    clear data_*
end
% extract positions from encoders
abs_pos_m2_q5 = data(23,:)'; 
abs_pos_m2_q5 = abs_pos_m2_q5 - mean(abs_pos_m2_q5); % demean
inc_pos_m2_q5 = data(20,:)';
inc_pos_m2_q5 = inc_pos_m2_q5 - mean(inc_pos_m2_q5); % demean
t2 = data(1,:)'; t2 = t2 - t2(1);
% use regression to fit transmission ratio
[b_fit_m2_q5,~,~,~,stats_fit_m2_q5] = regress(abs_pos_m2_q5, [ones(size(inc_pos_m2_q5)) inc_pos_m2_q5]);

% calculate fit
inc_pos2_fit = inc_pos_m2_q5*b_fit_m2_q5(2) + b_fit_m2_q5(1);

%% plot results q5
figure('color','w');
subplot(211); title('meas 1')
plot(t1,abs_pos_m1_q5,t1,inc_pos_m1_q5); hold on;
plot(t1,inc_pos1_fit,'--');
ylabel('position, rad');
text(0.02,0.06,['transmissionRatio = ' num2str(1/b_fit_m1_q5(2),'%.3f')],'units','normalized');

subplot(212); title('meas 2')
plot(t2,abs_pos_m2_q5,t2,inc_pos_m2_q5); hold on;
plot(t2,inc_pos2_fit,'--');
ylabel('Position, rad'); xlabel('time,s');
legend({'Abs Enc','Inc Enc',['Fit: R^2 = ' num2str(stats_fit_m1_q5(1),'%.2f')]},'FontSize',8)
text(0.02,0.06,['transmissionRatio = ' num2str(1/b_fit_m2_q5(2),'%.3f')],'units','normalized');

suptitle('Transmission measurement - FM1 - q5 - 161116')

print('test_transmission_fm1_q5.png','-dpng','-r300')

%% q1 (motor 1)

% load measurement 1 data
data = [];
files = dir('data_transmission_meas1_fm1_q1_part*.mat');
for ii = 1:length(files)
    load(files(ii).name);
    name = files(ii).name;
    num = str2double(name(strfind(name,'part')+4:end-3));
    eval(['data_temp = data_' num2str(num) ';']);
    data = [data data_temp];
    clear data_*
end
% extract positions from encoders
abs_pos_m1_q1 = data(10,:)'; 
abs_pos_m1_q1 = abs_pos_m1_q1 - mean(abs_pos_m1_q1); % demean
inc_pos_m1_q1 = data(7,:)';
inc_pos_m1_q1 = inc_pos_m1_q1 - mean(inc_pos_m1_q1); % demean
t1 = data(1,:)'; t1 = t1 - t1(1);
% use regression to fit transmission ratio
[b_fit_m1_q1,~,~,~,stats_fit_m1_q1] = regress(abs_pos_m1_q1, [ones(size(inc_pos_m1_q1)) inc_pos_m1_q1]);
% calculate fit
inc_pos1_fit = inc_pos_m1_q1*b_fit_m1_q5(2) + b_fit_m1_q1(1);

% load measurement 2 data
data = [];
files = dir('data_transmission_meas2_fm1_q1_part*.mat');
for ii = 5:10
    load(['data_transmission_meas2_fm1_q1_part' num2str(ii) '.mat']);
    eval(['data_temp = data_' num2str(ii) ';']);
    data = [data data_temp];
    clear data_*
end
% extract positions from encoders
abs_pos_m2_q1 = data(10,:)'; 
abs_pos_m2_q1 = abs_pos_m2_q1 - mean(abs_pos_m2_q1); % demean
inc_pos_m2_q1 = data(7,:)';
inc_pos_m2_q1 = inc_pos_m2_q1 - mean(inc_pos_m2_q1); % demean
t2 = data(1,:)'; t2 = t2 - t2(1);
% use regression to fit transmission ratio
[b_fit_m2_q1,~,~,~,stats_fit_m2_q1] = regress(abs_pos_m2_q1, [ones(size(inc_pos_m2_q1)) inc_pos_m2_q1]);

% calculate fit
inc_pos2_fit = inc_pos_m2_q1*b_fit_m2_q1(2) + b_fit_m2_q1(1);

%% plot results q5
figure('color','w');
subplot(211); title('meas 1')
plot(t1,abs_pos_m1_q1,t1,inc_pos_m1_q1); hold on;
plot(t1,inc_pos1_fit,'--');
ylabel('position, rad');
text(0.02,0.06,['transmissionRatio = ' num2str(1/b_fit_m1_q1(2),'%.3f')],'units','normalized');

subplot(212); title('meas 2')
plot(t2,abs_pos_m2_q1,t2,inc_pos_m2_q1); hold on;
plot(t2,inc_pos2_fit,'--');
ylabel('Position, rad'); xlabel('time,s');
legend({'Abs Enc','Inc Enc',['Fit: R^2 = ' num2str(stats_fit_m2_q1(1),'%.2f')]},'FontSize',8)
text(0.02,0.06,['transmissionRatio = ' num2str(1/b_fit_m2_q1(2),'%.3f')],'units','normalized');

suptitle('Transmission measurement - FM1 - q1 - 161116')

print('test_transmission_fm1_q1.png','-dpng','-r300')
